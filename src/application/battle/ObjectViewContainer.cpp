#include "ObjectViewContainer.h"
#include "BattlePlatform.h"
#include "Engine/Barrier.h"
#include "../common/Geometry.h"
#include "Engine/GameEngine.h"

ObjectViewContainer::ObjectViewContainer(const DrawableObjectRef& object) : 
	_ownerObject(object),
	_enableSnapshot(false),
	_lastUpdate(0) {
}

ObjectViewContainer::~ObjectViewContainer() {

}

void ObjectViewContainer::update(float t) {
	auto ownerRef = _ownerObject.lock();
	// check if owner referene is expried
	if (!ownerRef) {
		return;
	}

	if (_owner->isAvailable() == false || _enableSnapshot == false) {
		return;
	}
	constexpr float updateInterval = CAPTURE_REFRESH_RATE;
	if (t - _lastUpdate < updateInterval) {
		return;
	}

	ci::mat4 inverseMatrix;

	DrawableObject* owner = dynamic_cast<DrawableObject*>(_owner);

	if (owner) {
		inverseMatrix = glm::inverse(owner->getTransformation());
	}

	_modelSnapshotObjects.clear();

	auto battlePlatform = BattlePlatform::getInstance();
	if (battlePlatform) {
		auto& objects = battlePlatform->getSnapshotObjects();

		for (auto it = objects.begin(); it != objects.end(); it++) {
			auto& snapshotRef = (*it);
			if (snapshotRef->_ref.get() != _owner) {
				const auto& worldModelObjectPoly = snapshotRef->objectBound;

				auto snapshotObject = std::make_shared<SnapshotObject>();
				snapshotObject->_ref = snapshotRef->_ref;

				auto& viewModelObjectPoly = snapshotObject->objectBound;
				auto kt = viewModelObjectPoly.begin();
				for (auto jt = worldModelObjectPoly.begin(); jt != worldModelObjectPoly.end(); jt++, kt++) {
					*kt = DrawableObject::transform(*jt, inverseMatrix);
				}
				_modelSnapshotObjects.push_back(snapshotObject);
			}
		}
	}

	_lastUpdate = t;
}

const std::list<std::shared_ptr<SnapshotObject>>& ObjectViewContainer::getModelViewObjects() const {
	return _modelSnapshotObjects;
}

void ObjectViewContainer::enableSnapshot(bool enable) {
	_enableSnapshot = enable;
	if (_enableSnapshot == false) {
		_modelSnapshotObjects.clear();
	}
}