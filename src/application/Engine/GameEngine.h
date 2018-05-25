#pragma once

#include "../common/SyncMessageQueue.hpp"
#include "../common/ILogger.h"

#include "Scene.h"
#include "CollisionDetector.h"


#include <future>
#include <string>
#include <mutex>

#include <functional>

class GameEngine
{
private:
	bool _runFlag;
private:
	std::shared_ptr<Scene> _gameScene;
	std::shared_ptr<CollisionDetector> _collisionDetector;

protected:
	GameEngine(const char* configFile);
public:
	static GameEngine* createInstance();
	static GameEngine* getInstance();
	virtual ~GameEngine();

	void setScene(std::shared_ptr<Scene> scene);
	float getCurrentTime() const;

	void run();
	void stop();
	void doUpdate();
};

