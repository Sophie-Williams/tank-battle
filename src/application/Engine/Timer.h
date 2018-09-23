/******************************************************************
* File:        TimerObject.h
* Description: declare TimerObject class. A TimerObject object
*              is a game component object that hold a timer and
*              notify timer event to a handler.
* Author:      Vincent Pham
*
* Copyright (c) 2018 VincentPT.
** Distributed under the MIT License (http://opensource.org/licenses/MIT)
**
*
**********************************************************************/

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