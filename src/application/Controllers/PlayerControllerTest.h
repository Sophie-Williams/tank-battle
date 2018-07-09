#pragma once
#include "Engine/GameComponent.h"

class PlayerControllerTest : public GameComponent
{
	float _lastMoveAt;
	float _lastFireAt;
	float _fireLimitDuration;
	float _moveLimitDuration;
	bool _enableRun;

	void randomStrategy(float t);
public:
	PlayerControllerTest();
	virtual ~PlayerControllerTest();
	virtual void update(float t);
	void startControl();
	void stopControl();
};