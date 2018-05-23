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

inline void updateTranformForCollision(DrawableObject* object, float availableTime, float currentTime) {
	// update to new frame at the exepected time
	object->update(availableTime);
	// keep the tranformation at the exepected time
	auto backupTransform = object->getTransformation();
	// update the object's state to the current time
	object->update(currentTime);
	// restore transforam at expected time
	object->setTransformation(backupTransform);
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

				mat4 previousMat;

				// the time that the the object's states was updated
				float objectStateTime;

				// collision time range
				// start at very last time that collision does not occur
				// end at very first time that collision occurs
				std::pair<float, float> collisionTimeRange;

				std::shared_ptr<std::vector<ci::vec2>> boundingPoly;
				// objects that have collide with the object
				std::shared_ptr<std::list<DrawableObjectRef>> colliedObjects;
			};
			std::list<CollisionCheckInfo> collisionCheckList;

			// build the collision check list first
			auto& drawableObjects = _gameScene->getDrawableObjects();
			for (auto it = drawableObjects.begin(); it != drawableObjects.end(); it++) {
				auto drawableObject = it->get();
				if (drawableObject->isStaticObject() == false) {
					CollisionCheckInfo collisionCheckInfo;
					collisionCheckInfo.object = drawableObject;
					collisionCheckInfo.previousMat = drawableObject->getPreviousTransformation().first;
					collisionCheckInfo.objectStateTime = t;
					collisionCheckInfo.collisionTimeRange.first = t;
					collisionCheckInfo.collisionTimeRange.second = t;
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
				if (drawableObject->isAvailable() == false) {
					continue;
				}
				if (drawableObject->canBeWentThrough() && drawableObject->getCollisionHandler() == nullptr) {
					continue;
				}

				if (drawableObject->isStaticObject()) {
					drawableObject->getBoundingPoly(boundingPolyStatic);

					for (auto it = collisionCheckList.begin(); it != collisionCheckList.end(); it++) {
						currentCollision = it->object;
						convexPoly.resize(0);
						auto& dynamicBound = *it->boundingPoly;
						if (_collisionDetector->checkCollision2d(dynamicBound, boundingPolyStatic) >= 0) {
							if (_lastUpdate > 0) {
								// restore the state of object if it available before the current update occur
								// update back to previous frame
								currentCollision->update(_lastUpdate);
								// ensure the tranformation is same as previous frame
								currentCollision->setTransformation(it->previousMat);

								auto collideTimeRange = _collisionDetector->findEarliestCollideTime(_lastUpdate, it->objectStateTime, boundingPolyStatic, dynamicBound, currentCollision);

								if (drawableObject->canBeWentThrough() || currentCollision->canBeWentThrough()) {
									// update the object's state to the state
									updateTranformForCollision(currentCollision, it->objectStateTime, t);

									collideTimeRange.first = std::min(it->collisionTimeRange.first, it->objectStateTime);
								}
								else {
									// update to new frame at collide time
									updateTranformForCollision(currentCollision, collideTimeRange.second, t);

									it->objectStateTime = collideTimeRange.second;
								}
								// update the buffer bounding polygon for lastest state of the object
								currentCollision->getBoundingPoly(dynamicBound);
								// update time when the dynamic object cannot move anymore because of current collision
								it->collisionTimeRange = collideTimeRange;
							}

							// set lastest collide object
							auto& collidedObjects = it->colliedObjects;
							if (collidedObjects == nullptr) {
								collidedObjects = make_shared<std::list<DrawableObjectRef>>();
							}
							collidedObjects->push_back(drawableObject);
						}
					}
				}
			}
			// recheck and remove out-of-date collision
			// update the transforms to the very last time before collision occur
			for (auto it = collisionCheckList.begin(); it != collisionCheckList.end(); it++) {
				auto& colliedObjects = it->colliedObjects;
				
				if (colliedObjects) {
					auto& object = it->object;
					auto& dynamicBound = *it->boundingPoly;

					// recheck and remove out-of-date collision
					for (auto kt = colliedObjects->begin(); kt != colliedObjects->end();) {
						(*kt)->getBoundingPoly(boundingPolyStatic);
						if (_collisionDetector->checkCollision2d(dynamicBound, boundingPolyStatic) < 0) {
							auto ktTemp = kt;
							kt++;
							colliedObjects->erase(ktTemp);
						}
						else {
							kt++;
						}
					}
					// update the transforms to the very last time before collision occur
					// restore the state of object if it available before the current update occur
					// update back to previous frame
					object->update(_lastUpdate);
					// ensure the tranformation is same as previous frame
					object->setTransformation(it->previousMat);

					updateTranformForCollision(object, it->collisionTimeRange.first, t);
				}
			}

			// call collision event handler
			for (auto it = collisionCheckList.begin(); it != collisionCheckList.end(); it++) {
				auto& colliedObjects = it->colliedObjects;
				auto& object = it->object;
				if (colliedObjects) {
					auto& collisionHandler = object->getCollisionHandler();
					if (collisionHandler) {
						for (auto kt = colliedObjects->begin(); kt != colliedObjects->end(); kt++) {
							collisionHandler(*kt, it->collisionTimeRange.first);
						}
					}
				}
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