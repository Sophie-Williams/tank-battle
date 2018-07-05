#pragma once
#include "GameControllerInterfaces.h"

typedef RawArray<RawObject> SnapshotObjectPoints;
typedef RawArray<RawTimeObject> SnapshotTimeObjectPoints;

class GAME_CONTROLLER_INTERFACE TankPlayerContext {
public:
	TankPlayerContext();
	virtual ~TankPlayerContext();

	virtual const SnapshotTimeObjectPoints* getRadarSnapshot() const = 0;
	virtual const SnapshotObjectPoints* getCameraSnapshot() const = 0;
	virtual RawRay getMyGun() const = 0;
	virtual float getMyHealth() const = 0;
	virtual GeometryInfo getMyGeometry() const = 0;

	virtual TankOperations getCurrentOperations() const = 0;

	virtual bool isAlly(GameObjectId id) const = 0;
	virtual bool isEnemy(GameObjectId id) const = 0;
};