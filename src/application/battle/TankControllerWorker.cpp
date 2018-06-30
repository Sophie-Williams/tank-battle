#include "TankControllerWorker.h"
#include "Engine/GameEngine.h"
#include "PlayerContextImpl.h"
#include "TankCamera.h"
#include "Radar.h"

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
	_tank(tank)
{
}

TankControllerWorker::~TankControllerWorker() {}

long long getCurrentTimeStamp() {
	using namespace std::chrono;
	auto now = system_clock::now();
	auto now_ms = time_point_cast<milliseconds>(now);
	return now_ms.time_since_epoch().count();
}

void TankControllerWorker::run() {
	constexpr unsigned int requestControlInterval = 200;
	unsigned int timeLeft = 0;

	TankOperations tankCommands = TANK_NULL_OPERATION;
	GameEngine* gameEngine = GameEngine::getInstance();
	TankPlayerContextImpl playerContext;

	std::shared_ptr<TankCamera> tankCamera;
	std::shared_ptr<Radar> radar;

	SnapshotObjectPoints cameraSnapshots;
	SnapshotTimeObjectPoints radarSnapshots;

	initRawArray(cameraSnapshots);
	initRawArray(radarSnapshots);

	// free resource when the function exit
	std::unique_ptr<void, std::function<void(void*)>> freeResource((void*)0, 
		[&cameraSnapshots, &radarSnapshots](void*) {
		freeSnapshotsRaw(cameraSnapshots);
		freeSnapshotsRaw(radarSnapshots);
	});

	// take camera snapshot object and convert to raw objects
	std::function<void(SnapshotRefObjects&)> takeCameraSnhapShot = [&cameraSnapshots](SnapshotRefObjects& snapshotObjects) {
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
			}

			snapShotRaw++;
		}
	};

	// take radar snapshot object and convert to raw objects
	std::function<void(ScannedObjectGroupMap&)> takeRadarSnhapShot = [&radarSnapshots](ScannedObjectGroupMap& groupObjects) {
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

	do
	{
		auto t1 = getCurrentTimeStamp();

		// free previous raw snapshots
		freeSnapshotsRaw(cameraSnapshots);
		freeSnapshotsRaw(radarSnapshots);

		// take new raw snapshots from camera
		if (tankCamera) {
			tankCamera->accessSeenObjects(takeCameraSnhapShot);
		}
		else {
			initRawArray(cameraSnapshots);
		}

		// take new raw snapshots from radar
		if (radar) {
			radar->accessGrouoObjectsMultithread(takeRadarSnhapShot);
		}
		else {
			initRawArray(radarSnapshots);
		}

		// set the snapshot to player context
		playerContext.setCameraSnapshot(&cameraSnapshots);
		playerContext.setRadarSnapshot(&radarSnapshots);

		// acquire commands from tank's controller
		tankCommands = _tankController->giveOperations(&playerContext);

		// set commands to the tank
		if (!IS_NULL_COMMAND(tankCommands) && _tank && _tank->isAvailable()) {
			auto tank = _tank;
			auto task = [tank, tankCommands](float t) {
				if (tank->isAvailable() == false) return;

				auto scopeCommands = tankCommands;
				TankCommandsBuilder commandsBuilder(scopeCommands);
				tank->move(commandsBuilder.getMovingDir(), t);
				tank->turn(commandsBuilder.getTurnDir(), t);
				tank->spinBarrel(commandsBuilder.getSpinningGunDir(), t);
			};

			gameEngine->postTask(task);
		}

		// pause some moments before go to nex turn
		auto t2 = getCurrentTimeStamp();
		timeLeft = (unsigned int)(t2 - t1);
		timeLeft = timeLeft > requestControlInterval ? 0 : requestControlInterval - timeLeft;
		// check if a stop signal was sent then exit the loop
	} while (_stopSignal.waitSignal(timeLeft) == false);
}

void TankControllerWorker::stop() {
	_stopSignal.signal();
}

void TankControllerWorker::pause() {
}

void TankControllerWorker::resume() {
}