#pragma once
#include "GameControllerInterfaces.h"
#include "TankCommandsBuilder.h"
#include "PlayerContext.h"

class GAME_CONTROLLER_INTERFACE TankController
{
public:
	TankController();
	virtual ~TankController();
	virtual TankCommands giveCommands(TankPlayerContext* player) = 0;
};
