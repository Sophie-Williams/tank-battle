#pragma once
#include "GameControllerInterfaces.h"

class GAME_CONTROLLER_INTERFACE TankPlayerContext {
public:
	TankPlayerContext();
	virtual ~TankPlayerContext();

	virtual const RawArray<RawPoint>* getRadarSnapshot() const = 0;
	virtual const RawArray<RawPoint>* getCameraSnapshot() const = 0;
	virtual RawRay getMyGun() const = 0;
	virtual float getMyHealth() const = 0;
	virtual TankOperations getCurrentOperations() = 0;

	virtual bool isAlly(GameObjectId id) const = 0;
	virtual bool isEnemy(GameObjectId id) const = 0;
};