#pragma once
#include "GameControllerInterfaces.h"
#include "TankCommandsBuilder.h"
#include "PlayerContext.h"
#include "GameInterface.h"

class GAME_CONTROLLER_INTERFACE TankController
{
public:
	TankController();
	virtual ~TankController();
	virtual void setup(TankPlayerContext* player);
	virtual TankOperations giveOperations(TankPlayerContext* player) = 0;
};
