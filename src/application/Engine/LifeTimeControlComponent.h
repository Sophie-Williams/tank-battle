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