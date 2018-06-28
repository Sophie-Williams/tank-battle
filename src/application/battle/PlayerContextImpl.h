#pragma once
#include "PlayerContext.h"
#include "Engine/Tank.h"
#include "TankCamera.h"
#include "Radar.h"

class TankPlayerContextImpl : public TankPlayerContext {
	std::shared_ptr<Tank> _myTank;
	std::shared_ptr<TankCamera> _camera;
	std::shared_ptr<Radar> _radar;
	mutable SnapshotObjectPoints _cameraSnapshots;
	mutable SnapshotObjectPoints _radarSnapshots;
public:
	TankPlayerContextImpl();
	virtual ~TankPlayerContextImpl();

	const SnapshotObjectPoints* getRadarSnapshot() const;
	const SnapshotObjectPoints* getCameraSnapshot() const;
	RawRay getMyGun() const;
	float getMyHealth() const;
	TankOperations getCurrentOperations() const;

	bool isAlly(GameObjectId id) const;
	bool isEnemy(GameObjectId id) const;
};