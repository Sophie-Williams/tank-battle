#pragma once;
#include "GameControllerInterfaces.h"

class GAME_CONTROLLER_INTERFACE GameInterface {
public:
	GameInterface();
	virtual ~GameInterface();
	static GameInterface* getInstance();

	virtual float getObjectSpeed(GameObjectType) const = 0;
	virtual float getObjectHealth(GameObjectId id) const = 0;
	virtual float getTime() const = 0;

	virtual GameObjectType getObjectype(GameObjectId id) const = 0;
};