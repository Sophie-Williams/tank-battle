#include "Radar.h"
#include "EngineSpecific/Barrier.h"
#include "../common/Geometry.h"
#include "Engine/GameEngine.h"

Radar::Radar(const std::shared_ptr<ObjectViewContainer>& objectViewContainer, float scanSpeed) : _lastScanAt(-1), _range(100), _scanSpeed(scanSpeed), _objectViewContainer(objectViewContainer) {
	_lastScanAngle = 0;
	_angle = 0;

	_lastRay = _ray = ci::vec2(0, _range);
}

Radar::~Radar() {

}

void Radar::setRange(float range) {
	_range = range;
}

float Radar::getRange() const {
	return _range;
}

inline ci::vec2 computeRay(const float& range, const float& angle) {
	return ci::vec2(-range * sin(angle), range *  cos(angle));
}

void Radar::update(float t) {
	if (_owner->isAvailable() == false) {
		return;
	}

	if (_lastScanAt < 0) {
		_lastScanAt = t;
	}

	auto angle = (t - _lastScanAt) * _scanSpeed;
	_lastScanAt = t;
	_angle += angle;

	_ray = computeRay(_range, _angle);

 float scanMinAngle = glm::pi<float>() / 24;

	// update the detected object list during time event
	auto expiredTime = 2 * glm::pi<float>() / _scanSpeed;
	for(auto it = _detectedGroupObjects.begin(); it != _detectedGroupObjects.end();) {
		auto& detectedGroup = *it;

		auto& detectedObjects = detectedGroup.second;
		for (auto jt = detectedObjects->begin(); jt != detectedObjects->end();) {
			if (t - (*jt)->detectedTime >= expiredTime) {
				auto jtTemp = jt;
				jt++;
				detectedObjects->erase(jtTemp);
			}
			else {
				jt++;
			}
		}

		if (detectedObjects->size() == 0) {
			auto itTemp = it;
			it++;
			_detectedGroupObjects.erase(itTemp);
		}
		else {
			it++;
		}
	}

	if (_angle == _lastScanAngle) return;
	auto rangeFromLastScan = std::min(std::abs(_angle - _lastScanAngle), std::abs(_angle + 2 * glm::pi<float>() - _lastScanAngle));
	if (_angle > 2 * glm::pi<float>()) {
		_angle -= 2 * glm::pi<float>();
	}

	if (rangeFromLastScan >= scanMinAngle) {
		// roate ray around origin
		auto& rayStart = _lastRay;
		auto& rayEnd = _ray;

		std::vector<ci::vec2> rayArea = {
			{ 0, 0 }, // origin
			{ rayStart.x,  rayStart.y }, // last end point of ray's
			{ rayEnd.x,  rayEnd.y }, // current end point of ray's
		};

		_lastScanAngle = _angle;
		_lastRay = _ray;

		auto& objects = _objectViewContainer->getModelViewObjects();

		for (auto it = objects.begin(); it != objects.end(); it++) {
			const auto& viewModelObjectPoly = (*it)->objectBound;

			auto scannedObject = std::make_shared<ScannedObject>();
			if (geometry::polyIntersect(rayArea, viewModelObjectPoly, scannedObject->scannedPart) && scannedObject->scannedPart.size() > 2) {
				scannedObject->detectedTime = t;

				ScannedObjectGroupRef dummy;
				auto groupObject = std::make_pair((*it)->_ref, dummy);
				auto lt = _detectedGroupObjects.insert(groupObject);
				if (lt.second == true) {
					lt.first->second = std::make_shared<ScannedObjectGroup>();
				}

				lt.first->second->push_back(scannedObject);
			}
		}
	}
}

glm::vec2 Radar::getRay() const {
	return _ray;
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
	cam.setOrtho(-range, range, -range, range, 1, -1);
	ci::gl::ScopedProjectionMatrix scopeMatrices;
	ci::gl::setProjectionMatrix(cam.getProjectionMatrix());
	ci::ColorAf color;

	auto t = GameEngine::getInstance()->getCurrentTime();

	// update the detected object list during time event
	auto expiredTime = 2 * glm::pi<float>() / _scanSpeed;
	for (auto it = _detectedGroupObjects.begin(); it != _detectedGroupObjects.end(); it++) {
		auto& detectedGroup = *it;

		auto& ownerRef = detectedGroup.first;
		auto& detectedObjects = detectedGroup.second;

		if (dynamic_cast<const Barrier*>(ownerRef.get()) != nullptr) {
			color.r = 0.4f;
			color.g = 0.4f;
			color.b = 0.4f;
		}
		else {
			color.r = 1.0f;
			color.g = 0.0f;
			color.b = 0.0f;
		}

		for (auto jt = detectedObjects->begin(); jt != detectedObjects->end();jt++) {
			auto& scannedObjectRef = *jt;

			color.a = 1.0f - (t - scannedObjectRef->detectedTime)/ expiredTime;
			if (color.a < 0) {
				color.a = 0;
			}
			ci::gl::ScopedColor scopeColor(color);
			drawSolidPolygon(scannedObjectRef->scannedPart);
		}
	}
}

const ScannedObjectGroupMap& Radar::getGroupObjects() const {
	return _detectedGroupObjects;
}

const std::shared_ptr<ObjectViewContainer>& Radar::getView() const {
	return _objectViewContainer;
}