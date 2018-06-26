#pragma once
#include "PlayerContext.h"

class TankPlayerContextImpl : public TankPlayerContext {
public:
	TankPlayerContextImpl();
	virtual ~TankPlayerContextImpl();

	const RawArray<RawPoint>* getRadarSnapshot() const;
	const RawArray<RawPoint>* getCameraSnapshot() const;
	RawRay getMyGun() const;
	float getMyHealth() const;
	TankOperations getCurrentOperations() const;

	bool isAlly(GameObjectId id) const;
	bool isEnemy(GameObjectId id) const;
};