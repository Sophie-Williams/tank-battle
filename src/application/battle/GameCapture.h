#pragma once
#include "Engine/GameObject.h"

class GameCapture : public GameObject {
	float _lastUpdate;
public:
	GameCapture();
	~GameCapture();

	void update(float t);
};