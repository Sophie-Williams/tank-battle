#include "Radar.h"
#include "BattlePlatform.h"
#include "Engine/Barrier.h"

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

extern ci::vec2 transform(const ci::vec2& point, const glm::mat4& m);

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

	// convert object from world space to viewspace
	_modelViewObjects.clear();

	ci::mat4 inverseMatrix;
	if (_ownerObject) {
		inverseMatrix = glm::inverse(_ownerObject->getTransformation());
	}

	auto battlePlatform = BattlePlatform::getInstance();
	if (battlePlatform) {
		auto& objects = battlePlatform->getSnapshotObjects();
		for (auto it = objects.begin(); it != objects.end(); it++) {

			if ((*it)->_ref.get() != _ownerObject.get()) {
				auto modelViewSnapshot = std::make_shared<SnapshotObject>();
				modelViewSnapshot->objectBound = (*it)->objectBound;
				modelViewSnapshot->_ref = (*it)->_ref;

				auto& points = modelViewSnapshot->objectBound;
				for (auto jt = points.begin(); jt != points.end(); jt++) {
					*jt = transform(*jt, inverseMatrix);
				}

				_modelViewObjects.push_back(modelViewSnapshot);
			}
		}
	}
}

const glm::vec2& Radar::getRay() const {
	return _scanRaySegment;
}

void drawSolidPolygon(const std::vector<ci::vec2>& poly) {
	if (poly.size() < 3) return;

	for (int i = 2; i < (int)poly.size(); i++) {
		ci::gl::drawSolidTriangle(poly[0], poly[i - 1], poly[i]);
	}
}

void Radar::draw() {
	ci::CameraOrtho cam;
	auto range = getRange();
	cam.setOrtho(-range/2, range/2, -range/2, range/2, 1, -1);
	ci::gl::ScopedProjectionMatrix scopeMatrices;
	ci::gl::setProjectionMatrix(cam.getProjectionMatrix());
	ci::ColorAf color;

	for (auto it = _modelViewObjects.begin(); it != _modelViewObjects.end(); it++) {
		auto& modelViewSnapshot = *it;
		auto& points = modelViewSnapshot->objectBound;

		if (dynamic_cast<Barrier*>(modelViewSnapshot->_ref.get()) != nullptr) {
			color.r = 0.4f;
			color.g = 0.4f;
			color.b = 0.4f;
		}
		else {
			color.r = 1.0f;
			color.g = 0.0f;
			color.b = 0.0f;
		}
		color.a = 1.0f;
		ci::gl::ScopedColor scopeColor(color);
		drawSolidPolygon(points);
	}
}