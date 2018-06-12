#include "GameEngine.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <algorithm>
#include <string>

#include "cinder/gl/gl.h"
#include "cinder/app/App.h"

using namespace ci;
using namespace std::chrono;
using namespace std;

GameEngine* GameEngine::createInstance() {
	return getInstance();
}

GameEngine* GameEngine::getInstance() {
	static GameEngine* s_Instance = nullptr;
	if (s_Instance == nullptr) {
		s_Instance = new GameEngine(nullptr);
	}

	return s_Instance;
}

GameEngine::GameEngine(const char* configFile) : _runFlag(false), _pauseDuration(0), _pauseTime(-1) {
	_collisionDetector = std::make_shared<CollisionDetector>();
}

GameEngine::~GameEngine() {
}

inline float getNativeTime() {
	return (float)ci::app::App::get()->getElapsedSeconds();
}

float GameEngine::getCurrentTime() const {
	if (isPausing()) {
		return _pauseTime - _pauseDuration;
	}
	return getNativeTime() - _pauseDuration;
}

void GameEngine::setScene(const std::shared_ptr<Scene>& scene) {
	_gameScene = scene;
}

const std::shared_ptr<Scene>& GameEngine::getScene() const {
	return _gameScene;
}

void GameEngine::run() {
}

void GameEngine::stop() {
}

void GameEngine::pause() {
	_pauseTime = getNativeTime();
}

void GameEngine::resume() {
	_pauseDuration += getNativeTime() - _pauseTime;
	_pauseTime = -1;
}

bool GameEngine::isPausing() const {
	return (_pauseTime >= 0);
}

void GameEngine::doUpdate() {
	float t = getCurrentTime();
	if (_gameScene) {
		// update the scene
		_gameScene->update(t);

		// check colission for dynamic object
		{
			auto& drawableObjects = _gameScene->getDrawableObjects();
			_collisionDetector->resolveCollisions(drawableObjects, t);
		}
		
		// remove game object that not available
		{
			auto& objects = _gameScene->getObjects();
			for (auto it = objects.begin(); it != objects.end();) {
				if (it->get()->isAvailable()) {
					it++;
				}
				else {
					auto itTemp = it;
					it++;
					objects.erase(itTemp);
				}
			}
		}
		{
			auto& objects = _gameScene->getDrawableObjects();
			for (auto it = objects.begin(); it != objects.end();) {
				auto& obj2 = *it;
				if (it->get()->isAvailable()) {
					it++;
				}
				else {
					auto itTemp = it;
					it++;
					objects.erase(itTemp);
				}
			}
		}
	}
}
//
//void GameEngine::registerCollisionDetection(DrawableObjectRef object, CollisionDetectedHandler&& hander) {
//	std::pair<DrawableObjectRef, CollisionDetectedHandler> detectionElement;
//	detectionElement.first = object;
//	detectionElement.second = hander;
//
//	_monitoredObjects.push_back(detectionElement);
//}
//
//bool GameEngine::unregisterCollisionDetection(DrawableObjectRef object) {
//	auto it = std::find_if(_monitoredObjects.begin(), _monitoredObjects.end(),
//		[&object](const std::pair<DrawableObjectRef, CollisionDetectedHandler>& monitoredObject) {
//		if (monitoredObject.first.get() == object.get()) {
//			return true;
//		}
//
//		return false;
//	});
//
//	if (it == _monitoredObjects.end()) {
//		return false;
//	}
//
//	_monitoredObjects.erase(it);
//
//	return true;
//}