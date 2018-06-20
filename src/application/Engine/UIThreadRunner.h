#pragma once
#include "../common/SyncMessageQueue.hpp"
#include <functional>

typedef std::function<void(float)> UpdateTask;

class UIThreadRunner
{
protected:
	SemiSyncMessageQueue<UpdateTask> _tasks;
public:
	UIThreadRunner();
	virtual ~UIThreadRunner();

	void sendTask(UpdateTask&& task);
	void executeTasks(float t);
};