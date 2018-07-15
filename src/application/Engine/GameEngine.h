#pragma once

#include "../common/SyncMessageQueue.hpp"
#include "../common/ILogger.h"

#include "Scene.h"
#include "CollisionDetector.h"
#include "UIThreadRunner.h"

#include <thread>
#include <mutex>
#include <string>
#include <functional>
#include "../common/SpinArray.hpp"
#include "GameObject.h"

class GameEngine
{
	struct FrameInfo {
		float timeCap;
		float timeConsume;
	};
private:
	bool _runFlag;
	float _pauseTime;
	float _pauseDuration;
	float _expectedFrameTime;
	SpinArray<FrameInfo, 100> _frameCounter;
private:
	std::shared_ptr<Scene> _gameScene;
	std::list<GameObjectRef> _gameObjects;
	std::shared_ptr<CollisionDetector> _collisionDetector;
	UIThreadRunner _uiThreadRunner;

	SignalAny _stopSignal;
	std::thread _worker;
	std::mutex _gameResourceMutex;
protected:
	GameEngine(const char* configFile);
	void loop();
	void doUpdate(float t);
	std::list<GameObjectRef>::const_iterator findObjectIter(const GameObject* pObject) const;
public:
	static GameEngine* createInstance();
	static GameEngine* getInstance();
	virtual ~GameEngine();

	void addGameObject(GameObjectRef gameLogicObject);
	void removeGameObject(GameObjectRef gameLogicObject);
	GameObjectRef findObjectRef(const GameObject* pObject) const;
	const std::list<GameObjectRef>& getObjects() const;
	std::list<GameObjectRef>& getObjects();

	void setScene(const std::shared_ptr<Scene>& scene);
	const std::shared_ptr<Scene>& getScene() const;
	float getCurrentTime() const;

	void run();
	void stop();
	void pause();
	void resume();
	bool isPausing() const;

	void postTask(UpdateTask&& task);

	// this function must be called from a thread different than UI thread
	void sendTask(UpdateTask&& task);

	void accessEngineResource(std::function<void()>&& acessFunction);

	float getFramePerSecond() const;
	float getCPUUsage() const;
};

