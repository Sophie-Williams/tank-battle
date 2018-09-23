/******************************************************************
* File:        TimerObject.cpp
* Description: implement TimerObject class. A TimerObject object
*              is a game component object that hold a timer and
*              notify timer event to a handler.
* Author:      Vincent Pham
*
* Copyright (c) 2018 VincentPT.
** Distributed under the MIT License (http://opensource.org/licenses/MIT)
**
*
**********************************************************************/

#include "Timer.h"
#include "GameEngine.h"

TimerObject::TimerObject(float duration, bool notifyRealTime) : _startAt(-1), _duration(duration), _notifyRealTime(notifyRealTime) , _t(-1) {

}

TimerObject::~TimerObject() {

}

void TimerObject::startTimer() {
	_startAt = GameEngine::getInstance()->getCurrentTime();
}

void TimerObject::setTimeEventHandler(TimeEventHandler&& handler) {
	_timeEvent = handler;
}

void TimerObject::update(float t) {
	_t = t;
	if (_startAt >= 0) {
		bool timeOut = t - _startAt >= _duration;
		if (_notifyRealTime || timeOut) {
			if (_timeEvent) {
				_timeEvent(timeOut, t);
			}
		}
		if (timeOut) {
			destroy();
		}
	}
}

float TimerObject::getLifeTime() const {
	if (_startAt < 0) {
		return -1;
	}
	return _t - _startAt;
}
