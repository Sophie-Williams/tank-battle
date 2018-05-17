#include "LifeTimeControlComponent.h"
#include "GameObject.h"

LifeTimeControlComponent::LifeTimeControlComponent(float lifeTime) : _lifeTime(lifeTime) {
}

LifeTimeControlComponent::~LifeTimeControlComponent() {
}

void LifeTimeControlComponent::update(float t) {
	if (_lifeTime <= (t - _startTime)) {
		_owner->destroy();
	}
}

void LifeTimeControlComponent::startLifeTimeCountDown(float at) {
	_startTime = at;
}