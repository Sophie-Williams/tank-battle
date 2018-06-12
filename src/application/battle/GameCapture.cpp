#include "GameCapture.h"
#include "Engine/GameEngine.h"
#include "BattlePlatform.h"

GameCapture::GameCapture(){}
GameCapture::~GameCapture(){}

void GameCapture::update(float t) {
	auto& scene = GameEngine::getInstance()->getScene();
	auto& physicalObjects = scene->getDrawableObjects();
	BattlePlatform* battlePlatform = BattlePlatform::getInstance();

	if (battlePlatform == nullptr) return;

	auto& snapShotObjects = battlePlatform->getSnapshotObjects();
	snapShotObjects.clear();

	for (auto it = physicalObjects.begin(); it != physicalObjects.end(); it++) {
		auto& object = *it;
		if (object->canBeWentThrough() == false) {
			auto snapshotObject = std::make_shared<SnapshotObject>();
			object->getBoundingPoly(snapshotObject->objectBound);
			snapshotObject->_ref = object;

			snapShotObjects.push_back(snapshotObject);
		}
	}
}