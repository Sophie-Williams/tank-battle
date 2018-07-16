#pragma once
#include "Engine/GameComponent.h"

class GameCapture : public GameComponent {
	float _lastUpdate;
public:
	GameCapture();
	~GameCapture();

	void update(float t);
};