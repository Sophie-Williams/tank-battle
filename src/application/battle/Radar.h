#pragma once
#include "Engine/DrawableObject.h"
#include "ObjectViewContainer.h"
#include "BattlePlatform.h"
#include <mutex>

typedef std::list<std::shared_ptr<ScannedObject>> ScannedObjectGroup;
typedef std::shared_ptr<ScannedObjectGroup> ScannedObjectGroupRef;
typedef std::map<DrawableObjectRef, ScannedObjectGroupRef> ScannedObjectGroupMap;


class Radar : public GameComponent {
protected:
	std::shared_ptr<ObjectViewContainer> _objectViewContainer;
	std::map<DrawableObjectRef, ScannedObjectGroupRef> _detectedGroupObjects;
	mutable std::mutex _detectedGroupObjectsMutex;
	float _scanSpeed;
	float _lastScanAt;
	float _range;
	ci::vec2 _ray;
	ci::vec2 _lastRay;

	float _angle;
	float _lastScanAngle;
public:
	Radar(const std::shared_ptr<ObjectViewContainer>& objectViewContainer, float scanSpeed = glm::half_pi<float>());
	~Radar();
	void setRange(float range);
	float getRange() const;

	void update(float t);
	void draw();
	glm::vec2 getRay() const;
	const std::map<DrawableObjectRef, ScannedObjectGroupRef>& getGroupObjects() const;

	void accessGrouoObjectsMultithread(const std::function<void(ScannedObjectGroupMap&)>& access);
};