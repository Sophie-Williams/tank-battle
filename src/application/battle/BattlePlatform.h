#pragma once
#include "cinder/gl/gl.h"
#include "Engine/Tank.h"

#define CAPTURE_REFRESH_RATE (1.0f/24)

class SnapshotObject {
public:
	std::vector<ci::vec2> objectBound;
	DrawableObjectRef _ref;
	SnapshotObject();
	virtual ~SnapshotObject();
	virtual SnapshotObject* clone();
};

class ScannedObject {
public:
	std::vector<ci::vec2> scannedPart;
	float detectedTime;
};

class TankSnapshot : public SnapshotObject {
public:
	ci::vec4 gun;
public:
	TankSnapshot();
	virtual ~TankSnapshot();
	virtual SnapshotObject* clone();
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