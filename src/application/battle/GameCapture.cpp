#include "GameCapture.h"
#include "Engine/GameEngine.h"
#include "BattlePlatform.h"

GameCapture::GameCapture() : _lastUpdate(0) {}
GameCapture::~GameCapture(){}

void GameCapture::update(float t) {
	auto& scene = GameEngine::getInstance()->getScene();
	auto& physicalObjects = scene->getDrawableObjects();
	BattlePlatform* battlePlatform = BattlePlatform::getInstance();

	if (battlePlatform == nullptr) return;

	constexpr float updateInterval = CAPTURE_REFRESH_RATE;
	if (t - _lastUpdate < updateInterval) {
		return;
	}

	auto& snapShotObjects = battlePlatform->getSnapshotObjects();
	snapShotObjects.clear();

	for (auto it = physicalObjects.begin(); it != physicalObjects.end(); it++) {
		auto& object = *it;
		if (object->canBeWentThrough() == false) {
 
			std::shared_ptr<SnapshotObject> snapshotObject = std::make_shared<SnapshotObject>();
			object->getBoundingPoly(snapshotObject->objectBound);
			snapshotObject->_ref = object;

			snapShotObjects.push_back(snapshotObject);
		}
	}
	_lastUpdate = t;
}