#include "Radar.h"
#include "BattlePlatform.h"
#include "Engine/Barrier.h"
#include "Engine/Geometry.h"
#include "Engine/GameEngine.h"

Radar::Radar(const DrawableObjectRef& installedInObject, float scanSpeed) : _lastScanAt(-1), _scanRaySegment(0, 100), _scanSpeed(scanSpeed),_ownerObject(installedInObject) {
	_lastScanRaySegment = _scanRaySegment;
	_lastScanAngle = 0;
	_angle = 0;
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

	if (_lastScanAt < 0) {
		_lastScanAt = t;
	}

	auto angle = (t - _lastScanAt) * _scanSpeed;
	_angle += angle;

	auto cosAngle = cos(angle);
	auto sinAngle = sin(angle);

	// roate ray around origin
	float x = _scanRaySegment.x * cosAngle - _scanRaySegment.y * sinAngle;
	float y = _scanRaySegment.y * cosAngle + _scanRaySegment.x * sinAngle;

	// convert object from world space to viewspace
	//_modelViewObjects.clear();

	ci::mat4 inverseMatrix;
	if (_ownerObject) {
		inverseMatrix = glm::inverse(_ownerObject->getTransformation());
	}

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

	auto rangeFromLastScan = _angle > _lastScanAngle ? _angle - _lastScanAngle : _angle + 2 * glm::pi<float>() - _lastScanAngle;
	if (_angle > 2 * glm::pi<float>()) {
		_angle = 2 * glm::pi<float>() - _angle;
	}

	auto battlePlatform = BattlePlatform::getInstance();
	if (battlePlatform && rangeFromLastScan >= glm::pi<float>()/36) {
		_lastScanAngle = _angle;

		std::vector<ci::vec2> rayArea = {
			{ 0, 0 }, // origin
			{ _lastScanRaySegment.x,  _lastScanRaySegment.y }, // last end point of ray's
			{ x,  y }, // current end point of ray's
		};

		_lastScanRaySegment.x = x;
		_lastScanRaySegment.y = y;

		auto& objects = battlePlatform->getSnapshotObjects();

		std::vector<ci::vec2> viewModelObjectPoly;
		for (auto it = objects.begin(); it != objects.end(); it++) {
			if ((*it)->_ref.get() != _ownerObject.get()) {
				const auto& worldModelObjectPoly = (*it)->objectBound;
				viewModelObjectPoly.resize(worldModelObjectPoly.size());

				auto kt = viewModelObjectPoly.begin();
				for (auto jt = worldModelObjectPoly.begin(); jt != worldModelObjectPoly.end(); jt++, kt++) {
					*kt = transform(*jt, inverseMatrix);
				}
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

	// update new ray vector
	_scanRaySegment.x = x;
	_scanRaySegment.y = y;
	_lastScanAt = t;
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

	//for (auto it = _modelViewObjects.begin(); it != _modelViewObjects.end(); it++) {
	//	auto& modelViewSnapshot = *it;
	//	auto& points = modelViewSnapshot->objectBound;

	//	if (dynamic_cast<Barrier*>(modelViewSnapshot->_ref.get()) != nullptr) {
	//		color.r = 0.4f;
	//		color.g = 0.4f;
	//		color.b = 0.4f;
	//	}
	//	else {
	//		color.r = 1.0f;
	//		color.g = 0.0f;
	//		color.b = 0.0f;
	//	}
	//	color.a = 1.0f;
	//	ci::gl::ScopedColor scopeColor(color);
	//	drawSolidPolygon(points);
	//}

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