#pragma once
#include <Windows.h>

#include "../common/SyncMessageQueue.hpp"
#include "../common/ILogger.h"


#include <future>
#include <string>
#include <mutex>

#include <functional>

class GameEngine
{
private:
	bool _runFlag;
private:
public:
	GameEngine(const char* configFile);
	virtual ~GameEngine();

	void run();
	void stop();
	void doUpdate();
	void doDraw();
};

