#pragma once
#include "Engine/GameObject.h"

class GameCapture : public GameObject {
public:
	GameCapture();
	~GameCapture();

	void update(float t);
};