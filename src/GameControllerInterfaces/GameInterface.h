#pragma once;
#include "GameControllerInterfaces.h"

class GAME_CONTROLLER_INTERFACE GameInterface {
public:
	GameInterface();
	virtual ~GameInterface();
	static GameInterface* getInstance();

	virtual float getObjectSpeed(GameObjectType) const = 0;
	virtual float getObjectHealth(GameObjectId id) const = 0;

	// client should use freeRawArray to free array of point after using
	virtual RawArray<RawPoint> getWorldBoundary(GameObjectId id) const = 0;
	virtual bool isAlly(GameObjectId id) const = 0;
	virtual bool isEnemy(GameObjectId id) const = 0;
	virtual GameObjectType getObjectype(GameObjectId id) const = 0;
};