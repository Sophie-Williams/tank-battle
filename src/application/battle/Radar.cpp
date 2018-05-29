#include "Radar.h"

Radar::Radar(const DrawableObjectRef& installedInObject, float scanSpeed) : _startScanAt(-1), _range(100), _rayAngle(0), _scanSpeed(scanSpeed),_ownerObject(installedInObject) {

}

Radar::~Radar() {

}

void Radar::setRange(float range) {
	_range = range;
}

float Radar::getRange() const {
	return _range;
}

void Radar::update(float t) {
	if (_ownerObject->isAvailable() == false) {
		this->destroy(t);
		return;
	}

	if (_startScanAt < 0) {
		_startScanAt = t;
	}

	_rayAngle += (t - _startScanAt) * _scanSpeed;

	float x = cos(_rayAngle);
	float y = cos(_rayAngle);
}

void Radar::draw() {
	ci::gl::ScopedColor color(1.0f, 1.0f, 1.0f);
	for (auto it = _detectedObjects.begin(); it != _detectedObjects.end(); it++) {

	}
}