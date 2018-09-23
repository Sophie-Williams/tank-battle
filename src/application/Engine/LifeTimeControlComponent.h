/******************************************************************
* File:        LifeTimeControlComponent.h
* Description: declare LifeTimeControlComponent class. A
*              LifeTimeControlComponent object control life time
*              of its owner object when the game run.
* Author:      Vincent Pham
*
* Copyright (c) 2018 VincentPT.
** Distributed under the MIT License (http://opensource.org/licenses/MIT)
**
*
**********************************************************************/

#pragma once
#include "GameComponent.h"

class LifeTimeControlComponent : public GameComponent
{
protected:
	float _lifeTime;
	float _startTime;
public:
	LifeTimeControlComponent(float lifeTime);
	virtual ~LifeTimeControlComponent();

	virtual void update(float t);
	void startLifeTimeCountDown(float at);
};