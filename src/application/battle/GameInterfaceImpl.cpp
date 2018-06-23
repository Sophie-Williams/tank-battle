#include "GameInterfaceImpl.h"

GameInterfaceImpl::GameInterfaceImpl() {
}

GameInterfaceImpl::~GameInterfaceImpl() {
}

float GameInterfaceImpl::getObjectSpeed(GameObjectType) const {
	return -1;
}

float GameInterfaceImpl::getObjectHealth(GameObjectId id) const {
	return -1;
}

// client should use freeRawArray to free array of point after using
RawArray<RawPoint> GameInterfaceImpl::getWorldBoundary(GameObjectId id) const {
	RawArray<RawPoint> arr;
	arr.data = nullptr;
	arr.elmCount = 0;
	return arr;
}

bool GameInterfaceImpl::isAlly(GameObjectId id) const {
	return false;
}

bool GameInterfaceImpl::isEnemy(GameObjectId id) const {
	return false;
}

GameObjectType GameInterfaceImpl::getObjectype(GameObjectId id) const {
	return -1;
}