#include "ObjectViewContainer.h"
#include "BattlePlatform.h"
#include "Engine/Barrier.h"
#include "Engine/Geometry.h"
#include "Engine/GameEngine.h"

ObjectViewContainer::ObjectViewContainer(const DrawableObjectRef& object) : _ownerObject(object), _lastUpdate(0) {
}

ObjectViewContainer::~ObjectViewContainer() {

}

void ObjectViewContainer::update(float t) {
	if (_ownerObject->isAvailable() == false) {
		return;
	}
	constexpr float updateInterval = CAPTURE_REFRESH_RATE;
	if (t - _lastUpdate < updateInterval) {
		return;
	}

	ci::mat4 inverseMatrix;
	if (_ownerObject) {
		inverseMatrix = glm::inverse(_ownerObject->getTransformation());
	}

	_modelSnapshotObjects.clear();

	auto battlePlatform = BattlePlatform::getInstance();
	if (battlePlatform) {
		auto& objects = battlePlatform->getSnapshotObjects();

		for (auto it = objects.begin(); it != objects.end(); it++) {
			auto& snapshotRef = (*it);
			if (snapshotRef->_ref.get() != _ownerObject.get()) {
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

const DrawableObjectRef& ObjectViewContainer::getOwner() const {
	return _ownerObject;
}