#pragma once
#include "GameControllerInterfaces.h"
#include "TankCommandsBuilder.h"
#include "PlayerContext.h"
#include "TankController.h"

class SimplePlayer : public TankController
{
	char _moveDir = 1;
	int _moveCount = 100;
public:
	SimplePlayer();
	virtual ~SimplePlayer();
	void setup(TankPlayerContext* player);
	TankOperations giveOperations(TankPlayerContext* player);
};
