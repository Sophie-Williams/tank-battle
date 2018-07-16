#include "TankCamera.h"
#include "BattlePlatform.h"
#include "Engine/Barrier.h"
#include "../common/Geometry.h"
#include "Engine/GameEngine.h"

TankCamera::TankCamera(const std::shared_ptr<ObjectViewContainer>& objectViewContainer, float viewAngle) : 
	_objectViewContainer(objectViewContainer),
	_viewAngle(viewAngle),
	_lastScanAt(0) {
	setRange(100);
}

TankCamera::~TankCamera() {

}

void TankCamera::setRange(float range) {
	_range = range;

	auto angleStart = -_viewAngle / 2;

	constexpr float maxSweepAngle = glm::pi<float>() / 18 ;
	int n = (int)(_viewAngle / maxSweepAngle) + 1;
	auto sweepAngle = _viewAngle / n;

	_viewArea.resize(n + 1 + 1);

	// view base ray
	float startX = 0;
	float startY = _range;
	// roate ray around origin
	float angle = angleStart;
	int i;
	for (i = 0; i <= n; i++, angle+= sweepAngle) {
		auto& p = _viewArea[i];
		p.x = -_range * sin(angle);
		p.y = _range * cos(angle);
	}
	// origin
	auto& p = _viewArea[i];
	p.x = 0;
	p.y = 0;

	//auto halfAngle = _viewAngle / 2;
	//
	//_viewArea = {
	//	{ 0, 0 },
	//	{ -_range * sin(halfAngle), _range * cos(halfAngle) },
	//	{ -_range * sin(-halfAngle), _range * cos(-halfAngle) },
	//};
}

float TankCamera::getRange() const {
	return _range;
}

extern void drawSolidPolygon(const std::vector<ci::vec2>& poly);

void TankCamera::update(float t) {
	if (_owner->isAvailable() == false) {
		return;
	}

	if (_lastScanAt < 0) {
		_lastScanAt = t;
	}

	constexpr float updateInterval = CAPTURE_REFRESH_RATE;
	if (t - _lastScanAt < updateInterval) {
		return;
	}
	_lastScanAt = t;

	auto& objects = _objectViewContainer->getModelViewObjects();
	_seenObjects.clear();
	for (auto it = objects.begin(); it != objects.end(); it++) {
		const auto& viewModelObjectPoly = (*it)->objectBound;

		//if (dynamic_cast<Barrier*>((*it)->_ref.get())) continue;

		auto scannedObject = std::make_shared<SnapshotObject>();
		scannedObject->objectBound.resize(0);
		scannedObject->_ref = (*it)->_ref;
		if (geometry::polyIntersect(_viewArea, viewModelObjectPoly, scannedObject->objectBound) && scannedObject->objectBound.size() > 2) {
			_seenObjects.push_back(scannedObject);
		}
	}
}

void TankCamera::draw() {
	ci::CameraOrtho cam;
	auto range = getRange();
	cam.setOrtho(-range, range, -range, range, 1, -1);
	ci::gl::ScopedProjectionMatrix scopeMatrices;
	ci::gl::setProjectionMatrix(cam.getProjectionMatrix());
	ci::ColorAf color(0.2f, 0.2f, 0.3f, 1.0f);


	ci::gl::ScopedColor scopeColor(color);
	drawSolidPolygon(_viewArea);

	// update the detected object list during time event
	for (auto it = _seenObjects.begin(); it != _seenObjects.end(); it++) {
		auto& seenObject = *it;

		auto& ownerRef = seenObject->_ref;

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

		ci::gl::ScopedColor scopeColor(color);
		drawSolidPolygon(seenObject->objectBound);
	}
}

const std::shared_ptr<ObjectViewContainer>& TankCamera::getView() const {
	return _objectViewContainer;
}

const SnapshotRefObjects& TankCamera::getSeenObjects() const {
	return _seenObjects;
}