#pragma once
#include <functional>
#include "GameObject.h"

typedef std::function<void()> TimeEventHandler;

class TimerObject : public GameObject
{
	TimeEventHandler _timeCompletion;
	float _startAt;
	float _duration;
public:
	TimerObject(float duration);
	virtual ~TimerObject();

	void startTimer();
	void update(float t);
	void setTimeOutHandler(TimeEventHandler&& handler);
};