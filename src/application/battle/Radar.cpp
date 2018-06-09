#include "Radar.h"

Radar::Radar(const DrawableObjectRef& installedInObject, float scanSpeed) : _startScanAt(-1), _scanRaySegment(0, 100), _scanSpeed(scanSpeed),_ownerObject(installedInObject) {
}

Radar::~Radar() {

}

void Radar::setRange(float range) {
	_scanRaySegment = glm::normalize(_scanRaySegment) * range;
}

float Radar::getRange() const {
	return sqrtf (_scanRaySegment.x*_scanRaySegment.x + _scanRaySegment.y*_scanRaySegment.y);
}

void Radar::update(float t) {
	if (_ownerObject->isAvailable() == false) {
		this->destroy(t);
		return;
	}

	if (_startScanAt < 0) {
		_startScanAt = t;
	}

	auto angle = (t - _startScanAt) * _scanSpeed;
	auto cosAngle = cos(angle);
	auto sinAngle = sin(angle);

	// roate ray around origin
	float x = _scanRaySegment.x * cosAngle - _scanRaySegment.y * sinAngle;
	float y = _scanRaySegment.y * cosAngle + _scanRaySegment.x * sinAngle;

	_scanRaySegment.x = x;
	_scanRaySegment.y = y;
}

const glm::vec2& Radar::getRay() const {
	return _scanRaySegment;
}