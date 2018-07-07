#pragma once
#include "GameInterface.h"
#include "Engine/DrawableObject.h"
#include <mutex>

enum GameObjectType_ : int {
	GAME_TYPE_UNKNOWN = -1,
	GAME_TYPE_TANK = 0,
	GAME_TYPE_BARRIER,
	GAME_TYPE_BULLET
};

class GameInterfaceImpl : public GameInterface {
private:
	std::map<GameObjectId, DrawableObjectRef> _capturedObjectRefMap;
	mutable std::mutex _capturedObjectRefMapMutex;
public:
	GameInterfaceImpl();
	virtual ~GameInterfaceImpl();
	void doUpdate(float t);

	float getObjectSpeed(GameObjectType) const;
	float getObjectHealth(GameObjectId id) const;
	float getTime() const;

	GameObjectType getObjectype(GameObjectId id) const;
	DrawableObjectRef getObject(GameObjectId id) const;
};
