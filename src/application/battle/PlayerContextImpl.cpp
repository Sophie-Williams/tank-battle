#include "PlayerContextImpl.h"
#include "GameInterfaceImpl.h"
#include "EngineSpecific/Tank.h"

TankPlayerContextImpl::TankPlayerContextImpl(const std::shared_ptr<Tank>& myTank) :
	_pCameraSnapshots(nullptr),
	_pRadarSnapshots(nullptr),
	_pSnapshotCollision(nullptr),
	_myTank(myTank) {
}

TankPlayerContextImpl::~TankPlayerContextImpl() {
}

void TankPlayerContextImpl::setRadarSnapshot(const SnapshotTimeObjectPoints* pRadarSnapshots) {
	_pRadarSnapshots = pRadarSnapshots;
}

void TankPlayerContextImpl::setCameraSnapshot(const SnapshotObjectPoints* pCameraSnapshots) {
	_pCameraSnapshots = pCameraSnapshots;
}

void TankPlayerContextImpl::setCollisionsAtThisTurn(const SnapshotColissions* collisions) {
	_pSnapshotCollision = collisions;
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

const SnapshotColissions* TankPlayerContextImpl::getCollisionsAtThisTurn() const {
	return _pSnapshotCollision;
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

float TankPlayerContextImpl::getMovingSpeed() const {
	return GameInterfaceImpl::getInstance()->getObjectSpeed(GAME_TYPE_TANK);
}

float TankPlayerContextImpl::getTurningSpeed() const {
	return _myTank->getMaxTurningSpeed();
}

GeometryInfo TankPlayerContextImpl::getMyGeometry() const {
	GeometryInfo geo;
	glm::vec3 offset;
	float rotation;
	_myTank->getGeometryInfo(offset, rotation);

	geo.coord.x = offset.x;
	geo.coord.y = offset.y;
	geo.rotation = rotation;

	return geo;
}