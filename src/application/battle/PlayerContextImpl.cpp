#include "PlayerContextImpl.h"
#include "GameInterfaceImpl.h"
#include "Engine/Tank.h"

TankPlayerContextImpl::TankPlayerContextImpl() : _pCameraSnapshots(nullptr), _pRadarSnapshots(nullptr) {
}

TankPlayerContextImpl::~TankPlayerContextImpl() {
}

void TankPlayerContextImpl::setRadarSnapshot(const SnapshotTimeObjectPoints* pRadarSnapshots) {
	_pRadarSnapshots = pRadarSnapshots;
}

void TankPlayerContextImpl::setCameraSnapshot(const SnapshotObjectPoints* pCameraSnapshots) {
	_pCameraSnapshots = pCameraSnapshots;
}

const SnapshotTimeObjectPoints* TankPlayerContextImpl::getRadarSnapshot() const {
	return _pRadarSnapshots;
}

const SnapshotObjectPoints* TankPlayerContextImpl::getCameraSnapshot() const {
	return _pCameraSnapshots;
}

TankOperations TankPlayerContextImpl::getCurrentOperations() const {
	return _myTank->getCurrentOperations();
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