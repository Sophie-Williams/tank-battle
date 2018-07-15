#include "GameEngine.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <ratio>
#include <chrono>
#include <algorithm>
#include <string>

#include "cinder/gl/gl.h"
#include "cinder/app/App.h"
#include "../common/Utility.h"

using namespace ci;
using namespace std;

static chrono::high_resolution_clock::time_point timeAtEngineCreated;

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

GameEngine::GameEngine(const char* configFile) :
	_runFlag(false),
	_pauseDuration(0),
	_pauseTime(-1),
	_stopSignal(false),
	_expectedFrameTime(1.0f/60.0f),
	_frameCounter({0, 0}) {
	_collisionDetector = std::make_shared<CollisionDetector>();
	timeAtEngineCreated = chrono::high_resolution_clock::now();
}

GameEngine::~GameEngine() {
}

inline double getNativeTime() {
	using namespace std::chrono;
	high_resolution_clock::time_point t = high_resolution_clock::now();

	duration<double> time_span = duration_cast<duration<double>>(t - timeAtEngineCreated);
	return time_span.count();
}

float GameEngine::getCurrentTime() const {
	if (isPausing()) {
		return (float)(_pauseTime - _pauseDuration);
	}
	return (float)(getNativeTime() - _pauseDuration);
}

void GameEngine::setScene(const std::shared_ptr<Scene>& scene) {
	_gameScene = scene;
}

const std::shared_ptr<Scene>& GameEngine::getScene() const {
	return _gameScene;
}

extern bool stopAndWait(std::thread& worker, int milisecond);

void GameEngine::loop() {
	float timeLeft = 0;

	// check if a stop signal was sent then exit the loop
	while (_stopSignal.waitSignal((unsigned int)(timeLeft * 1000)) == false) {
		auto t1 = getCurrentTime();

		doUpdate(t1);

		// pause some moments before go to nex turn
		auto t2 = getCurrentTime();
		timeLeft = t2 - t1;

		auto& frame = _frameCounter.next();
		frame.timeCap = _expectedFrameTime;
		frame.timeConsume = timeLeft;

		timeLeft = timeLeft > _expectedFrameTime ? 0 : _expectedFrameTime - timeLeft;
	}
}

void GameEngine::run() {
	_worker = std::thread(std::bind(&GameEngine::loop, this));
}

void GameEngine::stop() {
	if (_worker.joinable()) {
		_stopSignal.signal();

		::stopAndWait(_worker, 500);
	}
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

void GameEngine::doUpdate(float t) {

	std::lock_guard<std::mutex> lk(_gameResourceMutex);

	_uiThreadRunner.executeTasks(t);

	if (_gameScene) {
		// update logic object
		for (auto it = _gameObjects.begin(); it != _gameObjects.end(); it++) {
			(*it)->update(t);
		}
		// update the scene
		{
			LOG_SCOPE_ACCESS(ILogger::getInstance(), "game scene update");
			_gameScene->update(t);
		}
		

		LOG_SCOPE_ACCESS(ILogger::getInstance(), "engine post processing");
		// check colission for dynamic object
		{
			auto& drawableObjects = _gameScene->getDrawableObjects();
			_collisionDetector->resolveCollisions(drawableObjects, t);
		}
		
		// remove game object that not available
		{
			for (auto it = _gameObjects.begin(); it != _gameObjects.end();) {
				if (it->get()->isAvailable()) {
					it++;
				}
				else {
					auto itTemp = it;
					it++;
					_gameObjects.erase(itTemp);
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

void GameEngine::postTask(UpdateTask&& task) {
	_uiThreadRunner.postTask(std::move(task));
}

void GameEngine::sendTask(UpdateTask&& task) {
	Signal<std::string> executedSignal(false);

	// execute task with signal notification
	auto taskWrapper = [&executedSignal, task](float t) {
		try {
			task(t);
			executedSignal.sendSignal("");
		}
		catch (std::exception& e) {
			std::string message("executed task failed: ");
			executedSignal.sendSignal(message + e.what());
		}
		catch (...) {
			executedSignal.sendSignal("executed task failed: unknown error");
		}
	};

	// post the task
	postTask(taskWrapper);

	// wait the task executed
	std::string msg;
	executedSignal.waitSignal(msg);
}

void GameEngine::accessEngineResource(std::function<void()>&& acessFunction) {
	std::lock_guard<std::mutex> lk(_gameResourceMutex);
	acessFunction();
}

float GameEngine::getFramePerSecond() const {
	int cap = _frameCounter.cap();
	float totalFrameTime = 0;
	for (int i = 0; i < cap; i++) {
		auto& frame = _frameCounter[i];
		totalFrameTime += std::max(frame.timeCap, frame.timeConsume);
	}
	if (totalFrameTime == 0) return -1;
	return cap / totalFrameTime;
}
float GameEngine::getCPUUsage() const {
	int cap = _frameCounter.cap();
	float totalFrameCap = 0;
	float totalFrameTime = 0;
	for (int i = 0; i < cap; i++) {
		auto& frame = _frameCounter[i];
		totalFrameCap += frame.timeCap;

		totalFrameTime += std::min(frame.timeCap,frame.timeConsume);
	}
	if (totalFrameCap == 0) return -1;
	return totalFrameTime / totalFrameCap;
}


void GameEngine::addGameObject(GameObjectRef gameObjectRef) {
	_gameObjects.push_back(gameObjectRef);
}

void GameEngine::removeGameObject(GameObjectRef gameObjectRef) {
	auto it = findObjectIter(gameObjectRef.get());
	if (it == _gameObjects.end()) {
		return;
	}

	_gameObjects.erase(it);
}

std::list<GameObjectRef>::const_iterator GameEngine::findObjectIter(const GameObject* pObject) const {
	for (auto it = _gameObjects.begin(); it != _gameObjects.end(); it++) {
		if (pObject == it->get()) {
			return it;
		}
	}

	return _gameObjects.end();
}

GameObjectRef GameEngine::findObjectRef(const GameObject* pObject) const {
	auto it = findObjectIter(pObject);
	if (it == _gameObjects.end()) {
		return nullptr;
	}

	return *it;
}

const std::list<GameObjectRef>& GameEngine::getObjects() const {
	return _gameObjects;
}

std::list<GameObjectRef>& GameEngine::getObjects() {
	return _gameObjects;
}