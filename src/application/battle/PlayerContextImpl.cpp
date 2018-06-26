#include "PlayerContextImpl.h"

TankPlayerContextImpl::TankPlayerContextImpl() {

}

TankPlayerContextImpl::~TankPlayerContextImpl() {

}

const RawArray<RawPoint>* TankPlayerContextImpl::getRadarSnapshot() const {
	return nullptr;
}

const RawArray<RawPoint>* TankPlayerContextImpl::getCameraSnapshot() const {
	return nullptr;
}

bool TankPlayerContextImpl::isAlly(GameObjectId id) const {
	return false;
}

bool TankPlayerContextImpl::isEnemy(GameObjectId id) const {
	return false;
}

RawRay TankPlayerContextImpl::getMyGun() const {
	RawRay gun;
	return gun;
}

float TankPlayerContextImpl::getMyHealth() const {
	return -1;
}

TankOperations TankPlayerContextImpl::getCurrentOperations() const {
	return TANK_NULL_OPERATION;
}