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

GameEngine::GameEngine(const char* configFile) : _runFlag(false) {
	_collisionDetector = std::make_shared<CollisionDetector>();
}

GameEngine::~GameEngine() {
}

float GameEngine::getCurrentTime() const {
	return (float)ci::app::App::get()->getElapsedSeconds();
}

void GameEngine::setScene(std::shared_ptr<Scene> scene) {
	_gameScene = scene;
}

void GameEngine::run() {
}

void GameEngine::stop() {
}

void GameEngine::doUpdate() {
	if (_gameScene) {
		float t = getCurrentTime();

		// update the scene
		_gameScene->update(t);

		// check collision after the scene is updated
		auto& objects = _gameScene->getDrawableObjects();
		for (auto it = objects.begin(); it != objects.end();) {
			auto& obj2 = *it;
			if (obj2->isAvailable()) {
				if (obj2->canBeWentThrough() == false) {
					for (auto jt = _monitoredObjects.begin(); jt != _monitoredObjects.end();) {
						auto& obj1 = jt->first;

						if (obj1.get() != obj2.get() && _collisionDetector->checkCollision(obj1.get(), obj2.get(), t)) {
							(jt->second)(obj2);
						}

						if (obj1->isAvailable()) {
							jt++;
						}
						else {
							auto itTemp = jt;
							jt++;
							_monitoredObjects.erase(itTemp);
						}
					}
				}
			}
			if (it->get()->isAvailable()) {
				it++;
			}
			else {
				auto itTemp = it;
				it++;
				objects.erase(itTemp);
			}
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

	}
}

void GameEngine::registerCollisionDetection(DrawableObjectRef object, CollisionDetectedHandler&& hander) {
	std::pair<DrawableObjectRef, CollisionDetectedHandler> detectionElement;
	detectionElement.first = object;
	detectionElement.second = hander;

	_monitoredObjects.push_back(detectionElement);
}

bool GameEngine::unregisterCollisionDetection(DrawableObjectRef object) {
	auto it = std::find_if(_monitoredObjects.begin(), _monitoredObjects.end(),
		[&object](const std::pair<DrawableObjectRef, CollisionDetectedHandler>& monitoredObject) {
		if (monitoredObject.first.get() == object.get()) {
			return true;
		}

		return false;
	});

	if (it == _monitoredObjects.end()) {
		return false;
	}

	_monitoredObjects.erase(it);

	return true;
}