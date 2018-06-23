#pragma once
#include "GameControllerInterfaces.h"

class GAME_CONTROLLER_INTERFACE TankPlayerContext {
public:
	TankPlayerContext();
	virtual ~TankPlayerContext();

	virtual const RawArray<GameObjectId>* getRadarSnapshot() const = 0;
	virtual const RawArray<GameObjectId>* getCameraSnapshot() const = 0;
	virtual const RawArray<RawPoint>* getViewModelBoundary(GameObjectId id) const = 0;
	virtual RawRay getMyGun() const = 0;
	virtual float getMyHealth() const = 0;
};