#pragma once

#include "../common/SyncMessageQueue.hpp"
#include "../common/ILogger.h"

#include "Scene.h"
#include "CollisionDetector.h"


#include <future>
#include <string>
#include <mutex>

#include <functional>

typedef std::function<void(DrawableObjectRef)> CollisionDetectedHandler;

class GameEngine
{
private:
	bool _runFlag;
private:
	std::shared_ptr<Scene> _gameScene;
	std::shared_ptr<CollisionDetector> _collisionDetector;
	std::list <std::pair<DrawableObjectRef, CollisionDetectedHandler>> _monitoredObjects;

protected:
	GameEngine(const char* configFile);
public:
	static GameEngine* createInstance();
	static GameEngine* getInstance();
	virtual ~GameEngine();

	void setScene(std::shared_ptr<Scene> scene);
	float getCurrentTime() const;
	void registerCollisionDetection(DrawableObjectRef object, CollisionDetectedHandler&& hander);
	bool unregisterCollisionDetection(DrawableObjectRef object);

	void run();
	void stop();
	void doUpdate();
};

