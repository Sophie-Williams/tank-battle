#include "DumpPlayer.h"
#include "../GameControllers.h"
#include "../common/GameUtil.hpp"
#include "GameInterface.h"
#include "TankCommandsBuilder.h"

#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

extern "C" {
	GAME_CONTROLLER_API TankController* createController() {
		return new DumpPlayer();
	}
}

DumpPlayer::DumpPlayer() : _commandBuider(_operation)
{
}

DumpPlayer::~DumpPlayer()
{
}

void DumpPlayer::setup(TankPlayerContext* playerContext) {
	GameInterface* gameInterface = GameInterface::getInstance();
	_prevGeometry = playerContext->getMyGeometry();
	_prevTime = gameInterface->getTime();
	_commandBuider.freeze();
	_prevDesireMove = 1;
	_commandBuider.move(_prevDesireMove);

	_rotationStuckCounters[0] = 0;
	_rotationStuckCounters[1] = 0;
}

int DumpPlayer::updateMovingStep(TankPlayerContext* player) {
	GameInterface* gameInterface = GameInterface::getInstance();

	auto currentGeometry = player->getMyGeometry();
	auto currentTime = gameInterface->getTime();

	// use geometry information to estimate stuck situation
	if (_prevDesireMove != 0) {
		auto movingSpeed = player->getMovingSpeed();
		auto velocity = _prevGeometry.coord - currentGeometry.coord;
		auto disance = sqrtf(velocity.x * velocity.x + velocity.y * velocity.y);

		auto expectedDistance = movingSpeed * (currentTime - _prevTime);

		if (disance < expectedDistance / 5) {
			_movingStuckCount++;
		}
		else {
			_movingStuckCount = 0;
		}
		if (_movingStuckCount > 5) {
			_commandBuider.move(-_prevDesireMove);
		}
		else {
			_commandBuider.move(_prevDesireMove);
		}
	}

	if (_prevDesireTurn != 0) {
		auto turningSpeed = player->getTurningSpeed();
		auto expectedAngle = turningSpeed * (currentTime - _prevTime);
		if (std::abs(_prevGeometry.rotation - currentGeometry.rotation) < expectedAngle / 5) {
			_turningStuckCount++;
		}
		else {
			_turningStuckCount = 0;
			_rotationStuckCounters[0] = 0;
			_rotationStuckCounters[1] = 0;
		}

		if (_turningStuckCount > 5) {
			_commandBuider.turn(-_prevDesireTurn);
			if (_rotationStuckCounters[0] == 0) {
				_rotationStuckCounters[0] = 1;
			}
			else {
				_rotationStuckCounters[1] = 1;
			}
		}
		else {
			_commandBuider.turn(_prevDesireTurn);
		}
	}

	// check if a bullet hit the tank then change moving direction
	auto collisions = player->getCollisionsAtThisTurn();
	if (collisions->elmCount != 0 || collisions->data != nullptr) {
		auto collisionStart =  collisions->data;
		auto collisionEnd = collisionStart + collisions->elmCount;
		for (auto collision = collisionStart; collision != collisionEnd; collision++) {
			if (collision->isExplosion) {
				if (collision->collisionPosition == CollisionPosition::Front) {
					_commandBuider.move(-1);
					_commandBuider.turn(-1);
					_runAfterGetHurtCount = STEP_BUFFER_COUNT;
					break;
				}
				if (collision->collisionPosition == CollisionPosition::Bottom) {
					_commandBuider.move(1);
					_commandBuider.turn(1);
					_runAfterGetHurtCount = STEP_BUFFER_COUNT;
					break;
				}
				if (collision->collisionPosition == CollisionPosition::Left || collision->collisionPosition == CollisionPosition::Right) {
					_commandBuider.move(1);
					break;
				}
				_hurtStepCount = 0;
			}
		}
	}
	_hurtStepCount++;

	return 0;
}

int DumpPlayer::runAfterGetHurt(TankPlayerContext* player) {
	_runAfterGetHurtCount--;
	int oldCount = _runAfterGetHurtCount;

	if (updateMovingStep(player)) {
		return 1;
	}

	if (_movingStuckCount > 5) {
		_stuckWhileRun = true;
	}

	if (_runAfterGetHurtCount <= 0) {
		_stuckWhileRun = false;
	}

	// check if get hurt one more time after going back from stuck
	if (_runAfterGetHurtCount > oldCount && _stuckWhileRun == true) {
		_changeWay = true;
	}

	if (_changeWay) {
		if (_runAfterGetHurtCount > STEP_BUFFER_COUNT / 2) {
			_commandBuider.move(0);
			_commandBuider.turn(1);
		}
		else {
			_commandBuider.move(1);
			_commandBuider.turn(1);
		}
		return 1;
	}

	return _runAfterGetHurtCount <= 0 ? 0 : 1;
}

int DumpPlayer::targetEnemyCheck(TankPlayerContext* player) {
	auto cameraViewObjects = player->getCameraSnapshot();
	if (cameraViewObjects == nullptr || cameraViewObjects->elmCount == 0) {
		return 0;
	}

	// check if the gun can fired a target
	auto objectStart = cameraViewObjects->data;
	auto objectEnd = objectStart + cameraViewObjects->elmCount;
	auto gunRay = player->getMyGun();

	auto& P = gunRay.start;
	auto& u = gunRay.dir;

	GeneralLine<float> gunLine;
	gunLine.build(P, u);

	// check if this tank's is targeting an enemy
	for (auto pObject = objectStart; pObject != objectEnd; pObject++) {

		if (player->isEnemy(pObject->id) == false) continue;

		auto pStart = pObject->shape.data;
		auto pEnd = pStart + pObject->shape.elmCount;
		if (isRayIntersect(P, u, pStart, pObject->shape.elmCount)) {
			auto centerPoly = center(pStart, pEnd);

			// fire target if the distance from the center of target to gun's line is
			// smaller than 2 metters
			if (std::abs(gunLine.directionalDistance(centerPoly)) < 2.0f) {
				_commandBuider.fire();
				if (_hurtStepCount > STEP_BUFFER_COUNT) {
					_commandBuider.move(0);
					_commandBuider.turn(0);
					_hurtStepCount = 0;
				}

				return 1;
			}
		}
	}

	double angleMin = 100;
	double angle;

	// if the gun is not target an enemy find a best way to rotate the gun
	int enemiesDetectedByCamera = 0;
	for (auto pObject = objectStart; pObject != objectEnd; pObject++) {
		if (player->isEnemy(pObject->id) == false) continue;
		enemiesDetectedByCamera++;

		auto pStart = pObject->shape.data;
		auto pEnd = pStart + pObject->shape.elmCount;
		auto centerPoly = center(pStart, pEnd);

		angle = directionalAngle(u, centerPoly - P);
		if (std::abs(angle) < std::abs(angleMin)) {
			angleMin = angle;
		}
	}

	if (angleMin > 0) {
		_commandBuider.spinGun(1);
	}
	else if (angleMin < 0) {
		_commandBuider.spinGun(-1);
	}
	else {
		_commandBuider.spinGun(0);
	}

	return enemiesDetectedByCamera == 0 ? 0 : 1;
}

int DumpPlayer::targetEnemyCheckByRadar(TankPlayerContext* player) {
	auto radarViewObjects = player->getRadarSnapshot();
	if (radarViewObjects == nullptr || radarViewObjects->elmCount == 0) {
		return 0;
	}

	// check if the gun can fired a target
	auto objectStart = radarViewObjects->data;
	auto objectEnd = objectStart + radarViewObjects->elmCount;
	auto gunRay = player->getMyGun();

	auto& P = gunRay.start;
	auto& u = gunRay.dir;

	GeneralLine<float> gunLine;
	gunLine.build(P, u);

	// check if this tank's is targeting an enemy
	for (auto pObject = objectStart; pObject != objectEnd; pObject++) {
		if (player->isEnemy(pObject->id) == false) continue;

		auto pStart = pObject->shape.data;
		auto pEnd = pStart + pObject->shape.elmCount;
		if (isRayIntersect(P, u, pStart, pObject->shape.elmCount)) {
			auto centerPoly = center(pStart, pEnd);

			// fire target if the distance from the center of target to gun's line is
			// smaller than 2 metters
			if (std::abs(gunLine.directionalDistance(centerPoly)) < 2.0f) {
				_commandBuider.fire();
				return 1;
			}
		}
	}

	double angleMin = 100;
	double angle;

	// if the gun is not target an enemy find a best way to rotate the gun
	for (auto pObject = objectStart; pObject != objectEnd; pObject++) {
		if (player->isEnemy(pObject->id) == false) continue;

		auto pStart = pObject->shape.data;
		auto pEnd = pStart + pObject->shape.elmCount;
		auto centerPoly = center(pStart, pEnd);

		angle = directionalAngle(u, centerPoly - P);
		if (std::abs(angle) < std::abs(angleMin)) {
			angleMin = angle;
		}
	}

	if (angleMin > 0) {
		_commandBuider.spinGun(1);
	}
	else if (angleMin < 0) {
		_commandBuider.spinGun(-1);
	}
	else {
		_commandBuider.spinGun(0);
	}

	return 0;
}

int DumpPlayer::findEnemy(TankPlayerContext* player) {
	auto radarSnapshots = player->getRadarSnapshot();
	if (radarSnapshots == nullptr || radarSnapshots->elmCount == 0) {
		return 0;
	}
	auto gunRay = player->getMyGun();

	auto& P = gunRay.start;
	auto& u = gunRay.dir;

	// try to turn around to find enemies
	auto objectStart = radarSnapshots->data;
	auto objectEnd = objectStart + radarSnapshots->elmCount;

	double angleMin = 100;
	double angle;

	for (auto pObject = objectStart; pObject != objectEnd; pObject++) {
		if (player->isEnemy(pObject->id) == false) continue;

		auto pStart = pObject->shape.data;
		auto pEnd = pStart + pObject->shape.elmCount;
		auto centerPoly = center(pStart, pEnd);

		angle = directionalAngle(u, centerPoly - P);
		if (std::abs(angle) < std::abs(angleMin)) {
			angleMin = angle;
		}
	}
	if (angleMin > 0) {
		_commandBuider.turn(1);
	}
	else if (angleMin < 0) {
		_commandBuider.turn(-1);
	}
	else {
		_commandBuider.turn(0);
	}
	_commandBuider.spinGun(0);

	return 0;
}

TankOperations DumpPlayer::giveOperationsInternal(TankPlayerContext* player) {
	GameInterface* gameInterface = GameInterface::getInstance();
	if (gameInterface == nullptr) {
		return TANK_NULL_OPERATION;
	}

	//if (_runAfterGetHurtCount > 0) {
	//	if (runAfterGetHurt(player)) {
	//		return _operation;
	//	}
	//}

	auto movingDir = _commandBuider.getMovingDir();
	auto turningDIr =_commandBuider.getTurnDir();

	_commandBuider.freeze();
	_commandBuider.move(movingDir);
	_commandBuider.turn(turningDIr);

	// pre set moving step
	if (updateMovingStep(player)) {
		return _operation;
	}

	if (targetEnemyCheck(player)) {
		return _operation;
	}

	// if there is no enemy detected by the camera
	if (_rotationStuckCounters[0] && _rotationStuckCounters[1]) {
		targetEnemyCheckByRadar(player);
	}
	else {
		findEnemy(player);
	}

	return _operation;
}

TankOperations DumpPlayer::giveOperations(TankPlayerContext* player) {
	TankOperations operations = giveOperationsInternal(player);

	// update state before return
	TankCommandsBuilder comandBuilder(operations);

	char oldTurning = _prevDesireTurn;

	if (IS_NULL_COMMAND( operations )) {
	}
	else {
		_prevDesireTurn = comandBuilder.getTurnDir();
		_prevDesireMove = comandBuilder.getMovingDir();
	}
	_prevGeometry = player->getMyGeometry();
	if (oldTurning != _prevDesireTurn) {
		_beginTurningAngle = _prevGeometry.rotation;
	}

	_prevTime = GameInterface::getInstance()->getTime();

	return operations;
}