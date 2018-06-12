#pragma once
#include "Engine/DrawableObject.h"
#include "battle/BattlePlatform.h"

class ScannedObject {
protected:
	GameObject* _ref;
public:
	enum Type {
		Unknown,
		Barrier,
		Tank,
	};

	std::vector<ci::vec2> boundingPoly;
	float detectedTime;
};

class Radar : public GameObject {
protected:
	DrawableObjectRef _ownerObject;
	std::list<ScannedObject> _detectedObjects;
	std::list<std::shared_ptr<SnapshotObject>> _modelViewObjects;
	float _scanSpeed;
	float _startScanAt;
	glm::vec2 _scanRaySegment;
public:
	Radar(const DrawableObjectRef& installedInObject, float scanSpeed = glm::half_pi<float>());
	~Radar();
	void setRange(float range);
	float getRange() const;

	void update(float t);
	void draw();
	const glm::vec2& getRay() const;
};