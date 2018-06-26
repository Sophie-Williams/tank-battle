#pragma once
#include "GameControllerInterfaces.h"
#include "TankCommandsBuilder.h"
#include "PlayerContext.h"

class GAME_CONTROLLER_INTERFACE TankController
{
public:
	TankController();
	virtual ~TankController();
	virtual TankOperations giveOperations(TankPlayerContext* player) = 0;
};
