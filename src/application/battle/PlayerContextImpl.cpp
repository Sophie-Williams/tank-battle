#include "PlayerContextImpl.h"
#include "GameInterfaceImpl.h"
#include "Engine/Tank.h"

TankPlayerContextImpl::TankPlayerContextImpl() {
	initRawArray(_cameraSnapshots);
	initRawArray(_radarSnapshots);
}

TankPlayerContextImpl::~TankPlayerContextImpl() {

}

const SnapshotObjectPoints* TankPlayerContextImpl::getRadarSnapshot() const {
	freeRawArray(_radarSnapshots);
	initRawArray(_radarSnapshots);

	if (_radar) {
	}

	return &_radarSnapshots;
}

const SnapshotObjectPoints* TankPlayerContextImpl::getCameraSnapshot() const {
	freeRawArray(_cameraSnapshots);

	if (_camera) {
		_camera->accessSeenObjects([this](const SnapshotRefObjects& snapshotObjects) {
			recreateRawArray(_cameraSnapshots, (int)snapshotObjects.size());
			for (auto it = snapshotObjects.begin(); it != snapshotObjects.end(); it++) {

			}
		});
	}
	else {
		initRawArray(_cameraSnapshots);
	}

	return &_cameraSnapshots;
}

bool TankPlayerContextImpl::isAlly(GameObjectId id) const {
	auto object = ((GameInterfaceImpl*) GameInterfaceImpl::getInstance())->getObject(id);
	if (object) {
		auto pTank = dynamic_cast<Tank*>(object.get());
		if (pTank) {
			return (pTank->getGroupId() == _myTank->getGroupId());
		}
	}
	return false;
}

bool TankPlayerContextImpl::isEnemy(GameObjectId id) const {
	auto object = ((GameInterfaceImpl*)GameInterfaceImpl::getInstance())->getObject(id);
	if (object) {
		auto pTank = dynamic_cast<Tank*>(object.get());
		if (pTank) {
			return (pTank->getGroupId() != _myTank->getGroupId());
		}
	}
	return false;
}

RawRay TankPlayerContextImpl::getMyGun() const {
	auto gun = _myTank->getGun();
	RawRay rawGun;

	rawGun.start.x = gun.x;
	rawGun.start.y = gun.y;

	rawGun.dir.x = gun.z - gun.x;
	rawGun.dir.y = gun.w - gun.y;

	return rawGun;
}

float TankPlayerContextImpl::getMyHealth() const {
	return _myTank->getHealth();
}

TankOperations TankPlayerContextImpl::getCurrentOperations() const {
	return _myTank->getCurrentOperations();
}