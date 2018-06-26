#include "GameInterfaceImpl.h"
#include "BattlePlatform.h"
#include "Engine/GameEngine.h"
#include "Engine/Tank.h"
#include "Engine/Barrier.h"
#include "Engine/Bullet.h"

static GameInterfaceImpl gameInterface;

GameInterfaceImpl::GameInterfaceImpl() {
}

GameInterfaceImpl::~GameInterfaceImpl() {
}

float GameInterfaceImpl::getObjectSpeed(GameObjectType type) const {
	if (type == GAME_TYPE_TANK) {
		return 10; // 10 metter per second
	}
	return -1;
}

float GameInterfaceImpl::getObjectHealth(GameObjectId id) const {
	auto gameEngine = GameEngine::getInstance();
	float health = -1;

	gameEngine->sendTask([gameEngine, id, &health](float t) {
		auto& scene = gameEngine->getScene();
		auto& objects = scene->getDrawableObjects();

		for (auto it = objects.begin(); it != objects.end(); it++) {
			auto& object = *it;
			if (id == object->getId()) {
				auto liveObject = dynamic_cast<LiveObject*>(object.get());
				if (liveObject) {
					health = liveObject->getHealth();
					break;
				}
			}
		}
	});

	return health;
}

GameObjectType GameInterfaceImpl::getObjectype(GameObjectId id) const {
	auto gameEngine = GameEngine::getInstance();
	GameObjectType type = GAME_TYPE_UNKNOWN;

	gameEngine->sendTask([gameEngine, id, &type](float t) {
		auto& scene = gameEngine->getScene();
		auto& objects = scene->getObjects();
		for (auto it = objects.begin(); it != objects.end(); it++) {
			auto& object = *it;
			if (id == object->getId()) {
				if (dynamic_cast<Tank*>(object.get())) {
					type = GAME_TYPE_TANK;
					break;
				}
				if (dynamic_cast<Barrier*>(object.get())) {
					type = GAME_TYPE_BARRIER;
					break;
				}
				if (dynamic_cast<Bullet*>(object.get())) {
					type = GAME_TYPE_BULLET;
					break;
				}
			}
		}
	});

	return type;
}