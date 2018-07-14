#pragma once
#include <functional>
#include "GameComponent.h"

typedef std::function<void(bool timeOut, float t)> TimeEventHandler;

class TimerObject : public GameComponent
{
	TimeEventHandler _timeEvent;
	float _startAt;
	float _t;
	float _duration;
	bool _notifyRealTime;
public:
	TimerObject(float duration, bool notifyRealTime = false);
	virtual ~TimerObject();

	void startTimer();
	void update(float t);
	void setTimeEventHandler(TimeEventHandler&& handler);
	float getLifeTime() const;
};