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

GameEngine::GameEngine(const char* configFile) : _runFlag(false), _lastUpdate(0) {
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
	float t = getCurrentTime();
	if (_gameScene) {
		// update the scene
		_gameScene->update(t);

		// check colission for dynamic object
		{
			struct CollisionCheckInfo {
				// object need to be check
				DrawableObject* object;
				DrawableObjectRef colliedObject;

				// lastest time before collision occur
				float availableTime;

				std::shared_ptr<std::vector<ci::vec2>> boundingPoly;

				// objects that have collide with the object
				//std::shared_ptr<std::list<CollisionCheckInfo*>> colliedObjects;
			};
			std::list<CollisionCheckInfo> collisionCheckList;

			// build the collision check list first
			auto& drawableObjects = _gameScene->getDrawableObjects();
			for (auto it = drawableObjects.begin(); it != drawableObjects.end(); it++) {
				auto drawableObject = it->get();
				if (drawableObject->isStaticObject() == false) {
					CollisionCheckInfo collisionCheckInfo;
					collisionCheckInfo.object = drawableObject;
					collisionCheckInfo.availableTime = t;
					collisionCheckInfo.boundingPoly = std::make_shared<std::vector<ci::vec2>>(4);
					drawableObject->getBoundingPoly(*collisionCheckInfo.boundingPoly);

					collisionCheckList.push_back(collisionCheckInfo);
				}
			}

			// check collision with static object first
			std::vector<ci::vec2> boundingPolyStatic(4);
			std::vector<ci::vec2> convexPoly;
			DrawableObject* currentCollision = nullptr;

			auto compareToCurrentCollision = [&currentCollision](const std::pair<DrawableObjectRef, CollisionDetectedHandler> & obj) {
				return currentCollision == obj.first.get();
			};

			for (auto jt = drawableObjects.begin(); jt != drawableObjects.end(); jt++) {
				auto& drawableObject = *jt;
				if (drawableObject->isStaticObject() && drawableObject->canBeWentThrough() == false && drawableObject->isAvailable()) {
					drawableObject->getBoundingPoly(boundingPolyStatic);

					for (auto it = collisionCheckList.begin(); it != collisionCheckList.end(); it++) {
						currentCollision = it->object;
						convexPoly.resize(0);
						if (_collisionDetector->checkCollision2d(*it->boundingPoly, boundingPolyStatic, convexPoly)) {
							// set lastest collide object
							it->colliedObject = drawableObject;
						}
					}
				}
			}
			// call collision event handler
			for (auto it = collisionCheckList.begin(); it != collisionCheckList.end(); it++) {
				if (it->colliedObject) {
					currentCollision = it->object;
					auto kt = std::find_if(_monitoredObjects.begin(), _monitoredObjects.end(), compareToCurrentCollision);
					if (kt != _monitoredObjects.end()) {
						(kt->second)(it->colliedObject);

						if (kt->first->isAvailable()) {
							_monitoredObjects.erase(kt);
						}
					}
				}
			}
		}

		// check collision after the scene is updated
		//auto& objects = _gameScene->getDrawableObjects();
		//for (auto it = objects.begin(); it != objects.end();) {
		//	auto& obj2 = *it;
		//	if (obj2->isAvailable()) {
		//		if (obj2->canBeWentThrough() == false) {
		//			for (auto jt = _monitoredObjects.begin(); jt != _monitoredObjects.end();) {
		//				auto& obj1 = jt->first;

		//				if (obj1.get() != obj2.get() && _collisionDetector->checkCollision(obj1.get(), obj2.get(), t)) {
		//					(jt->second)(obj2);
		//				}

		//				if (obj1->isAvailable()) {
		//					jt++;
		//				}
		//				else {
		//					auto itTemp = jt;
		//					jt++;
		//					_monitoredObjects.erase(itTemp);
		//				}
		//			}
		//		}
		//	}
		//	if (it->get()->isAvailable()) {
		//		it++;
		//	}
		//	else {
		//		auto itTemp = it;
		//		it++;
		//		objects.erase(itTemp);
		//	}
		//}
		
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

	// update the last time update
	// must be executed before exit this function
	_lastUpdate = t;
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