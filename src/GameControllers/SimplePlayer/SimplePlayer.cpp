#include "SimplePlayer.h"
#include "../GameControllers.h"
#include "../common/GameUtil.hpp"
#include "GameInterface.h"
#include "TankCommandsBuilder.h"

#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

extern "C" {
	GAME_CONTROLLER_API TankController* createController() {
		return new SimplePlayer();
	}
}

SimplePlayer::SimplePlayer()
{
}

SimplePlayer::~SimplePlayer()
{
}

void SimplePlayer::setup(TankPlayerContext*) {
	static bool seedGenerated = false;
	if (seedGenerated == false) {
		srand((unsigned int)time(NULL));
		seedGenerated = true;
	}

	_moveDir = 1;
	_moveCount = (rand() % 50 + 50);
}

TankOperations SimplePlayer::giveOperations(TankPlayerContext* player) {
	GameInterface* gameInterface = GameInterface::getInstance();
	if (gameInterface == nullptr) {
		return TANK_NULL_OPERATION;
	}
	TankOperations opration = TANK_NULL_OPERATION;
	TankCommandsBuilder commandBuider(opration);

	auto gunRay = player->getMyGun();
	auto cameraViewObjects = player->getCameraSnapshot();
	if (cameraViewObjects == nullptr || cameraViewObjects->elmCount == 0) {
		auto radarSnapshots = player->getRadarSnapshot();
		if (radarSnapshots == nullptr || radarSnapshots->elmCount == 0) {
			return TANK_NULL_OPERATION;
		}
	}

	// check if the gun can fired a target
	auto objectStart = cameraViewObjects->data;
	auto objectEnd = objectStart + cameraViewObjects->elmCount;

	auto& P = gunRay.start;
	auto& u = gunRay.dir;

	commandBuider.freeze();

	commandBuider.move(_moveDir);
	_moveCount--;
	if (_moveCount <= 0) {
		_moveCount = (rand() % 50 + 50);
		if (rand() % 2) {
			_moveDir = -_moveDir;
		}
	}

	GeneralLine<float> gunLine;
	gunLine.build(P, u);

	for (auto pObject = objectStart; pObject != objectEnd; pObject++) {

		if (player->isEnemy(pObject->id) == false) continue;

		auto pStart = pObject->shape.data;
		auto pEnd = pStart + pObject->shape.elmCount;
		if (isRayIntersect(P, u, pStart, pObject->shape.elmCount)) {
			auto centerPoly = center(pStart, pEnd);

			// fire target if the distance from the center of target to gun's line is
			// smaller than 2 metters
			if (std::abs( gunLine.directionalDistance(centerPoly) ) < 2.0f) {
				commandBuider.fire();
				return opration;
			}
		}
	}

	
	double angleMin = 100;
	double angle;

	int enemiesDetectedByCamera = 0;

	for (auto pObject = objectStart; pObject != objectEnd; pObject++) {
		if (player->isEnemy(pObject->id) == false) continue;
		enemiesDetectedByCamera++;

		auto pStart = pObject->shape.data;
		auto pEnd = pStart + pObject->shape.elmCount;
		auto centerPoly = center(pStart, pEnd);

		angle = directionalAngle(u, centerPoly - P);
		if ( std::abs(angle) < std::abs(angleMin)) {
			angleMin = angle;
		}
	}

	if (angleMin > 0) {
		commandBuider.spinGun(1);
	}
	else if (angleMin < 0) {
		commandBuider.spinGun(-1);
	}
	else {
		commandBuider.spinGun(0);
	}

	if(enemiesDetectedByCamera == 0)
	{
		auto radarSnapshots = player->getRadarSnapshot();
		if (radarSnapshots == nullptr || radarSnapshots->elmCount == 0) {
			return TANK_NULL_OPERATION;
		}

		auto objectStart = radarSnapshots->data;
		auto objectEnd = objectStart + radarSnapshots->elmCount;

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
			commandBuider.turn(1);
		}
		else if (angleMin < 0) {
			commandBuider.turn(-1);
		}
		else {
			commandBuider.turn(0);
		}
		commandBuider.spinGun(0);
	}

	return opration;
}