#pragma once
#include "cinder/gl/gl.h"
#include "Engine/Tank.h"

class SnapshotObject {
public:
	ci::mat4 transform;
	ci::Rectf tankBoundary;
};

class TankSnapshot : public SnapshotObject {
public:
	ci::vec3 gunVector;
};

class BattlePlatform {
	ci::Rectf _battleArea;
	std::list<std::shared_ptr<SnapshotObject>> _snapshotObjects;
public:
	BattlePlatform(float width, float height);
	~BattlePlatform();

	const ci::Rectf& getMapArea() const;
	const std::list<std::shared_ptr<SnapshotObject>>& getSnapshotObjects();
};