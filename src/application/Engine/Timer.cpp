#include "Timer.h"
#include "GameEngine.h"

TimerObject::TimerObject(float duration) : _startAt(-1), _duration(duration) {

}

TimerObject::~TimerObject() {

}

void TimerObject::startTimer() {
	_startAt = GameEngine::getInstance()->getCurrentTime();
}

void TimerObject::setTimeOutHandler(TimeEventHandler&& handler) {
	_timeCompletion = handler;
}

void TimerObject::update(float t) {
	if (_startAt >= 0 && t - _startAt >= _duration) {
		if (_timeCompletion) {
			_timeCompletion();
		}

		destroy(t);
	}
}
