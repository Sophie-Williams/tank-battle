#pragma once
#include <functional>
#include "GameComponent.h"

typedef std::function<void()> TimeEventHandler;

class TimerObject : public GameComponent
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