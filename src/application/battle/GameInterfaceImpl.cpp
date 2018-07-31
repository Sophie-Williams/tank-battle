#include "GameInterfaceImpl.h"
#include "BattlePlatform.h"
#include "Engine/GameEngine.h"
#include "Engine/Tank.h"
#include "Engine/Barrier.h"
#include "Engine/Bullet.h"
#include "../common/ILogger.h"

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

void GameInterfaceImpl::doUpdate(float t) {
	std::lock_guard<std::mutex> lk(_capturedObjectRefMapMutex);

	_capturedObjectRefMap.clear();

	auto gameEngine = GameEngine::getInstance();
	auto& scene = gameEngine->getScene();
	auto& objects = scene->getDrawableObjects();

	for (auto it = objects.begin(); it != objects.end(); it++) {
		auto& object = *it;
		_capturedObjectRefMap[object->getId()] = object;
	}
}

float GameInterfaceImpl::getObjectHealth(GameObjectId id) const {
	std::lock_guard<std::mutex> lk(_capturedObjectRefMapMutex);

	float health = -1;

	auto it = _capturedObjectRefMap.find(id);
	if (it != _capturedObjectRefMap.end()) {
		auto liveObject = dynamic_cast<LiveObject*>(it->second.get());
		if (liveObject) {
			health = liveObject->getHealth();
		}
	}

	return health;
}

float GameInterfaceImpl::getTime() const {
	return GameEngine::getInstance()->getCurrentTime();
}

GameObjectType GameInterfaceImpl::getObjectype(GameObjectId id) const {
	std::lock_guard<std::mutex> lk(_capturedObjectRefMapMutex);
	GameObjectType type = GAME_TYPE_UNKNOWN;

	auto it = _capturedObjectRefMap.find(id);
	if (it != _capturedObjectRefMap.end()) {
		auto& object = it->second;
		if (dynamic_cast<Tank*>(object.get())) {
			type = GAME_TYPE_TANK;
		}
		else if (dynamic_cast<Barrier*>(object.get())) {
			type = GAME_TYPE_BARRIER;
		}
		else if (dynamic_cast<Bullet*>(object.get())) {
			type = GAME_TYPE_BULLET;
		}
	}

	return type;
}

DrawableObjectRef GameInterfaceImpl::getObject(GameObjectId id) const {
	std::lock_guard<std::mutex> lk(_capturedObjectRefMapMutex);
	auto it = _capturedObjectRefMap.find(id);
	if (it != _capturedObjectRefMap.end()) {
		return it->second;
	}

	return nullptr;
}

void GameInterfaceImpl::printMessage(const char* header, const char* message) const {
	ILogger::getInstance()->logV(LogLevel::Info, "[%s] %s", header, message);
}