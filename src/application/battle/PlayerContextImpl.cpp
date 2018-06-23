#include "PlayerContextImpl.h"

TankPlayerContextImpl::TankPlayerContextImpl() {

}

TankPlayerContextImpl::~TankPlayerContextImpl() {

}

const RawArray<GameObjectId>* TankPlayerContextImpl::getRadarSnapshot() const {
	return nullptr;
}

const RawArray<GameObjectId>* TankPlayerContextImpl::getCameraSnapshot() const {
	return nullptr;
}

const RawArray<RawPoint>* TankPlayerContextImpl::getViewModelBoundary(GameObjectId id) const {
	return nullptr;
}

RawRay TankPlayerContextImpl::getMyGun() const {
	RawRay gun;
	return gun;
}

float TankPlayerContextImpl::getMyHealth() const {
	return -1;
}