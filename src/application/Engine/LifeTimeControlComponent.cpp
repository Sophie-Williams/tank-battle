/******************************************************************
* File:        LifeTimeControlComponent.cpp
* Description: implement LifeTimeControlComponent class. A
*              LifeTimeControlComponent object control life time
*              of its owner object when the game run.
* Author:      Vincent Pham
*
* Copyright (c) 2018 VincentPT.
** Distributed under the MIT License (http://opensource.org/licenses/MIT)
**
*
**********************************************************************/

#include "LifeTimeControlComponent.h"
#include "GameObject.h"

LifeTimeControlComponent::LifeTimeControlComponent(float lifeTime) : _lifeTime(lifeTime) {
}

LifeTimeControlComponent::~LifeTimeControlComponent() {
}

void LifeTimeControlComponent::update(float t) {
	if (_lifeTime <= (t - _startTime)) {
		_owner->destroy(t);
	}
}

void LifeTimeControlComponent::startLifeTimeCountDown(float at) {
	_startTime = at;
}