#include "TankControllerWorker.h"
#include "Engine/GameEngine.h"
#include "PlayerContextImpl.h"
#include "Engine/Bullet.h"
#include "../common/GameUtil.hpp"

#include <Windows.h>

template <class T>
void freeSnapshotsRaw(T& obj) {
	auto snapShotRaw = obj.data;
	if (snapShotRaw == nullptr) return;
	auto snapShotRawEnd = snapShotRaw + obj.elmCount;

	while (snapShotRaw != snapShotRawEnd) {
		freeRawArray(snapShotRaw->shape);
		snapShotRaw++;
	}

	freeRawArray(obj);
}

TankControllerWorker::TankControllerWorker(const std::shared_ptr<Tank>& tank, 
	const std::shared_ptr<TankController>& tankController) :
	_tankController(tankController),
	_stopSignal(false),
	_tank(tank),
	_pWaitForReadySignal(nullptr),
	_totalProcessingTime(0),
	_frameCount(0)
{

	CollisionDetectedHandler onCollisionDetected = std::bind(&TankControllerWorker::onTankCollision,
		this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	_tankCollisionHandlerId = (_tank->getCollisionHandler() += std::move(onCollisionDetected));
}

TankControllerWorker::~TankControllerWorker() {
	_tank->getCollisionHandler() -= _tankCollisionHandlerId;
}

long long getCurrentTimeStamp() {
	using namespace std::chrono;
	auto now = system_clock::now();
	auto now_ms = time_point_cast<milliseconds>(now);
	return now_ms.time_since_epoch().count();
}

void TankControllerWorker::onTankCollision(DrawableObjectRef other, const CollisionInfo& poistion, float t) {
	ColissionRawInfo collision;
	collision.collisionPosition = (CollisionPosition)poistion.relative;
	collision.isExplosion = dynamic_cast<Bullet*>(other.get()) != nullptr;

	if (collision.collisionPosition == CollisionPosition::Unknown) {
		// estimate relative collision internally
		std::vector<ci::vec2> boundingPoly(4);
		other->getBoundingPoly(boundingPoly);
		
		auto pStart = boundingPoly.data();
		auto pEnd = pStart + boundingPoly.size();
		auto objectCenter = center(pStart, pEnd);
		
		ci::mat4 inverseMatrix = glm::inverse(_tank->getTransformation());
		auto theCenterOnTankView = DrawableObject::transform(objectCenter, inverseMatrix);

		// caculate angle of vector from center of tank to center of colission object and x axis
		auto angle = atan2(theCenterOnTankView.y, theCenterOnTankView.x) * 180 / glm::pi<float>();
		// make angle in range [0->2PI]
		if (angle < 0) {
			angle = 360 + angle;
		}
		if (45 <= angle && angle < 135) {
			collision.collisionPosition = CollisionPosition::Front;
		}
		else if (135 <= angle && angle < 225) {
			collision.collisionPosition = CollisionPosition::Left;
		}
		else if (225 <= angle && angle < 315) {
			collision.collisionPosition = CollisionPosition::Bottom;
		}
		else {
			collision.collisionPosition = CollisionPosition::Right;
		}
	}

	_collisions.push_back(collision);
}

void TankControllerWorker::setUp() {
	auto& components = _tank->getComponents();
	for (auto it = components.begin(); it != components.end(); it++) {
		auto tankCamera = std::dynamic_pointer_cast<TankCamera>(*it);
		if (tankCamera) {
			_tankCamera = tankCamera;
		}
		else {
			auto tankRadar = std::dynamic_pointer_cast<Radar>(*it);
			if (tankRadar) {
				_radar = tankRadar;
			}
		}
	}

	_totalProcessingTime = 0;
	_frameCount = 0;
}

void TankControllerWorker::enablePeripherals(bool enable) {
	auto& components = _tank->getComponents();
	for (auto it = components.begin(); it != components.end(); it++) {
		auto objectViewContainer = dynamic_cast<ObjectViewContainer*>(it->get());
		if (objectViewContainer) {
			objectViewContainer->enableSnapshot(enable);
		}
	}
}

void TankControllerWorker::loop() {
	if (_pWaitForReadySignal) {
		_pWaitForReadySignal->waitSignal();
	}
	GameEngine* gameEngine = GameEngine::getInstance();

	// enable snapshot for camera and radar

	gameEngine->postTask([this](float t) {
		enablePeripherals(true);
	});
	
	constexpr unsigned int requestControlInterval = 20;
	unsigned int timeLeft = 0;

	TankOperations tankCommands = TANK_NULL_OPERATION;
	TankPlayerContextImpl playerContext(_tank);

	std::shared_ptr<TankCamera> tankCamera = _tankCamera;
	std::shared_ptr<Radar> radar = _radar;

	SnapshotObjectPoints cameraSnapshots;
	SnapshotTimeObjectPoints radarSnapshots;
	SnapshotColissions collisionsAtEachTurn;

	initRawArray(cameraSnapshots);
	initRawArray(radarSnapshots);
	initRawArray(collisionsAtEachTurn);

	// free resource when the function exit
	std::unique_ptr<void, std::function<void(void*)>> freeResource((void*)1,
		[&cameraSnapshots, &radarSnapshots, &collisionsAtEachTurn](void*) {
		freeSnapshotsRaw(cameraSnapshots);
		freeSnapshotsRaw(radarSnapshots);
		freeRawArray(collisionsAtEachTurn);
	});

	// take camera snapshot object and convert to raw objects
	std::function<void(const SnapshotRefObjects&)> takeCameraSnhapShot = [&cameraSnapshots](const SnapshotRefObjects& snapshotObjects) {
		if (snapshotObjects.size() == 0) return;

		recreateRawArray(cameraSnapshots, (int)snapshotObjects.size());
		auto snapShotRaw = cameraSnapshots.data;
		for (auto it = snapshotObjects.begin(); it != snapshotObjects.end(); it++) {
			auto& snapshot = *it;
			auto& bound = snapshot->objectBound;

			snapShotRaw->id = snapshot->_ref->getId();
			recreateRawArray(snapShotRaw->shape, (int)bound.size());
			auto rawPoint = snapShotRaw->shape.data;
			for (auto jt = bound.begin(); jt != bound.end(); jt++) {
				rawPoint->x = jt->x;
				rawPoint->y = jt->y;
				rawPoint++;
			}

			snapShotRaw++;
		}
	};

	// take radar snapshot object and convert to raw objects
	std::function<void(const ScannedObjectGroupMap&)> takeRadarSnhapShot = [&radarSnapshots](const ScannedObjectGroupMap& groupObjects) {
		if (groupObjects.size() == 0) return;
		recreateRawArray(radarSnapshots, (int)groupObjects.size());
		auto snapShotRaw = radarSnapshots.data;

		for (auto it = groupObjects.begin(); it != groupObjects.end(); it++) {
			auto& detectedGroup = *it;

			auto& ownerRef = detectedGroup.first;
			auto& detectedObjects = detectedGroup.second;
			int pointCount = 0;

			for (auto jt = detectedObjects->begin(); jt != detectedObjects->end(); jt++) {
				auto& scannedObjectRef = *jt;
				pointCount += (int)scannedObjectRef->scannedPart.size();
			}

			snapShotRaw->id = ownerRef->getId();
			snapShotRaw->t = 0;

			recreateRawArray(snapShotRaw->shape, pointCount);
			auto rawPoint = snapShotRaw->shape.data;

			float t = 0;

			for (auto jt = detectedObjects->begin(); jt != detectedObjects->end(); jt++) {
				auto& scannedObjectRef = *jt;
				auto& bounding = scannedObjectRef->scannedPart;
				t += scannedObjectRef->detectedTime;
				for (auto kt = bounding.begin(); kt != bounding.end(); kt++) {
					rawPoint->x = kt->x;
					rawPoint->y = kt->y;

					rawPoint++;
				}
			}
			snapShotRaw->t = t / detectedObjects->size();
			snapShotRaw++;
		}
	};

	auto takeCollisionAtThisTurn = [this](SnapshotColissions& collisionsAtEachTurn) {
		freeRawArray(collisionsAtEachTurn);
		if (_collisions.size() == 0) {
			initRawArray(collisionsAtEachTurn);
			return;
		}
		recreateRawArray(collisionsAtEachTurn, (int)_collisions.size());

		auto rawPoint = collisionsAtEachTurn.data;
		for (auto it = _collisions.begin(); it != _collisions.end(); it++) {
			*rawPoint++ = *it;
		}

		_collisions.clear();
	};

	_tankController->setup(&playerContext);
	
	while (_stopSignal.waitSignal(timeLeft) == false && _tank->isAvailable()) {
		auto t1 = getCurrentTimeStamp();

		// free previous raw snapshots
		freeSnapshotsRaw(cameraSnapshots);
		freeSnapshotsRaw(radarSnapshots);

		// take new raw snapshots from camera
		gameEngine->accessEngineResource([&]() {
			if (tankCamera) {
				takeCameraSnhapShot(tankCamera->getSeenObjects());
			}
			else {
				initRawArray(cameraSnapshots);
			}

			// take new raw snapshots from radar
			if (radar) {
				takeRadarSnhapShot(radar->getGroupObjects());
			}
			else {
				initRawArray(radarSnapshots);
			}

			// take raw collisions at this turn
			takeCollisionAtThisTurn(collisionsAtEachTurn); 
		});
		

		// set the snapshot to player context
		playerContext.setCameraSnapshot(&cameraSnapshots);
		playerContext.setRadarSnapshot(&radarSnapshots);
		playerContext.setCollisionsAtThisTurn(&collisionsAtEachTurn);

		// acquire commands from tank's controller
		auto pt1 = gameEngine->getCurrentTime();
		tankCommands = _tankController->giveOperations(&playerContext);
		auto pt2 = gameEngine->getCurrentTime();

		_totalProcessingTime += pt2 - pt1;
		_frameCount++;

		// set commands to the tank
		if (_tank && _tank->isAvailable()) {
			if (!IS_NULL_COMMAND(tankCommands)) {
				auto t = gameEngine->getCurrentTime();
				auto scopeCommands = tankCommands;
				TankCommandsBuilder commandsBuilder(scopeCommands);
				_tank->move(commandsBuilder.getMovingDir(), t);
				_tank->turn(commandsBuilder.getTurnDir(), t);
				_tank->spinBarrel(commandsBuilder.getSpinningGunDir(), t);
				if (commandsBuilder.hasFire()) {
					_tank->fire(t);
				}
			}
		}
		else {
			break;
		}

		// pause some moments before go to nex turn
		auto t2 = getCurrentTimeStamp();
		timeLeft = (unsigned int)(t2 - t1);
		timeLeft = timeLeft > requestControlInterval ? 0 : requestControlInterval - timeLeft;
		// check if a stop signal was sent then exit the loop
	}
}

void TankControllerWorker::run() {
	setUp();
	worker = std::thread(std::bind(&TankControllerWorker::loop, this));
}

bool stopAndWait(std::thread& worker, int milisecond) {
	HANDLE hThread = (HANDLE)worker.native_handle();
	DWORD waitRes = WaitForSingleObject(hThread, (DWORD)milisecond);

	if (waitRes == WAIT_OBJECT_0) {
		worker.join();
		return true;
	}
	if (waitRes == WAIT_TIMEOUT) {
		bool res = TerminateThread(hThread, 0) != FALSE;
		ILogger::getInstance()->log(LogLevel::Error, "Warning!!! thread is force terminate!");
		worker.join();
		return res;
	}

	return false;
}

bool TankControllerWorker::stopAndWait(int milisecond) {
	std::unique_ptr<void, std::function<void(void*)>> resetTankCommand((void*)1, [this](void*) {
		auto tank = _tank;
		auto task = [tank](float t) {
			if (tank->isAvailable() == false) return;

			tank->move(0, t);
			tank->turn(0, t);
			tank->spinBarrel(0, t);
		};

		GameEngine::getInstance()->postTask(task);
	});


	// disable snapshot for camera and radar
	//enablePeripherals(false);

	if (worker.joinable()) {
		_stopSignal.signal();
		worker.join();
		return true;
		//return ::stopAndWait(worker, milisecond);
	}

	return false;
}

void TankControllerWorker::pause() {
}

void TankControllerWorker::resume() {
}

void TankControllerWorker::setSignalWaiter(SignalAny* pSignal) {
	_pWaitForReadySignal = pSignal;
}

const std::shared_ptr<Tank>& TankControllerWorker::getAssociatedTank() const {
	return _tank;
}

const std::string& TankControllerWorker::getName() const {
	return _name;
}

void TankControllerWorker::setName(std::string& name) {
	_name = name;
}

float TankControllerWorker::getAverageProcessingTimeOfController() const {
	if (_frameCount == 0) {
		return -1;
	}

	return _totalProcessingTime / _frameCount;
}