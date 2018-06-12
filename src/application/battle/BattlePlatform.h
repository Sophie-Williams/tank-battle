#pragma once
#include "cinder/gl/gl.h"
#include "Engine/Tank.h"

class SnapshotObject {
public:
	std::vector<ci::vec2> objectBound;
	DrawableObjectRef _ref;
	SnapshotObject();
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

	static BattlePlatform* getInstance();

	const ci::Rectf& getMapArea() const;
	std::list<std::shared_ptr<SnapshotObject>>& getSnapshotObjects();
};