#pragma once
#include "GameInterface.h"

enum GameObjectType_ : int {
	GAME_TYPE_UNKNOWN = -1,
	GAME_TYPE_TANK = 0,
	GAME_TYPE_BARRIER,
	GAME_TYPE_BULLET
};

class GameInterfaceImpl : public GameInterface {

private:

public:
	GameInterfaceImpl();
	virtual ~GameInterfaceImpl();

	float getObjectSpeed(GameObjectType) const;
	float getObjectHealth(GameObjectId id) const;

	GameObjectType getObjectype(GameObjectId id) const;
};
