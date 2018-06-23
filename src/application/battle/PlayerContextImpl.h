#pragma once
#include "PlayerContext.h"

class TankPlayerContextImpl : public TankPlayerContext {
public:
	TankPlayerContextImpl();
	virtual ~TankPlayerContextImpl();

	const RawArray<GameObjectId>* getRadarSnapshot() const;
	const RawArray<GameObjectId>* getCameraSnapshot() const;
	const RawArray<RawPoint>* getViewModelBoundary(GameObjectId id) const;
	RawRay getMyGun() const;
	float getMyHealth() const;
};