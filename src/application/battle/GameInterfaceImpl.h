#pragma once
#include "GameInterface.h"

class GameInterfaceImpl : public GameInterface {
public:
	GameInterfaceImpl();
	virtual ~GameInterfaceImpl();

	float getObjectSpeed(GameObjectType) const;
	float getObjectHealth(GameObjectId id) const;

	// client should use freeRawArray to free array of point after using
	RawArray<RawPoint> getWorldBoundary(GameObjectId id) const;
	bool isAlly(GameObjectId id) const;
	bool isEnemy(GameObjectId id) const;
	GameObjectType getObjectype(GameObjectId id) const;
};
