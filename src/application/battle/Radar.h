#pragma once
#include "Engine/DrawableObject.h"
#include "battle/BattlePlatform.h"

class ScannedObject {
public:
	std::vector<ci::vec2> scannedPart;
	float detectedTime;
};

typedef std::list<std::shared_ptr<ScannedObject>> ScannedObjectGroup;
typedef std::shared_ptr<ScannedObjectGroup> ScannedObjectGroupRef;

class Radar : public GameObject {
protected:
	DrawableObjectRef _ownerObject;
	std::map<DrawableObjectRef, ScannedObjectGroupRef> _detectedGroupObjects;
	//std::list<std::shared_ptr<SnapshotObject>> _modelViewObjects;
	float _scanSpeed;
	float _lastScanAt;
	glm::vec2 _scanRaySegment;

	float _angle;
	float _lastScanAngle;
	glm::vec2 _lastScanRaySegment;
public:
	Radar(const DrawableObjectRef& installedInObject, float scanSpeed = glm::half_pi<float>());
	~Radar();
	void setRange(float range);
	float getRange() const;

	void update(float t);
	void draw();
	const glm::vec2& getRay() const;
};