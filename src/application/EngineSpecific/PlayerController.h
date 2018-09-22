#pragma once
#include "Tank.h"

class PlayerController
{
protected:
	std::shared_ptr<Tank> _player;
public:
	PlayerController(std::shared_ptr<Tank> player);
	virtual ~PlayerController();

	virtual void makeAction(float t) = 0;
};