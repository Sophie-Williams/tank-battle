#pragma once
#include "Engine/GameComponent.h"

class PlayerControllerTest : public GameComponent
{
	float _lastMoveAt;
	float _lastFireAt;
	float _lifeBeginAt;
	float _fireLimitDuration;
	float _moveLimitDuration;

	void randomStrategy(float t);
	void moveDownStrategy(float t);
public:
	PlayerControllerTest();
	virtual ~PlayerControllerTest();
	virtual void update(float t);
};