#pragma once
#include "GameControllerInterfaces.h"
#include "TankCommandsBuilder.h"
#include "PlayerContext.h"
#include "TankController.h"

class SimplePlayer : public TankController
{
public:
	SimplePlayer();
	virtual ~SimplePlayer();
	void setup(TankPlayerContext* player);
	TankOperations giveOperations(TankPlayerContext* player);
};
