#pragma once
#include "PlayerContext.h"
#include "Engine/Tank.h"

class TankPlayerContextImpl : public TankPlayerContext {
private:
	std::shared_ptr<Tank> _myTank;
	const SnapshotObjectPoints* _pCameraSnapshots;
	const SnapshotTimeObjectPoints* _pRadarSnapshots;
	const SnapshotColissions* _pSnapshotCollision;
public:
	TankPlayerContextImpl(const std::shared_ptr<Tank>& myTank);
	virtual ~TankPlayerContextImpl();

	void setRadarSnapshot(const SnapshotTimeObjectPoints* pCameraSnapshots);
	void setCameraSnapshot(const SnapshotObjectPoints* pRadarSnapshots);
	void setCollisionsAtThisTurn(const SnapshotColissions* collisions);

	const SnapshotTimeObjectPoints* getRadarSnapshot() const;
	const SnapshotObjectPoints* getCameraSnapshot() const;
	const SnapshotColissions* getCollisionsAtThisTurn() const;

	TankOperations getCurrentOperations() const;

	RawRay getMyGun() const;
	float getMyHealth() const;
	float getMovingSpeed() const;
	float getTurningSpeed() const;
	GeometryInfo getMyGeometry() const;

	bool isAlly(GameObjectId id) const;
	bool isEnemy(GameObjectId id) const;
};

typedef std::shared_ptr<TankPlayerContextImpl> TankPlayerContextImplRef;