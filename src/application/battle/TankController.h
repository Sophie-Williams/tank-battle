#pragma once
#include "TankCommandsBuilder.h"
#include "PlayerContext.h"

class TankController
{
public:
	TankController();
	virtual ~TankController();
	virtual TankCommands giveCommands(PlayerContext* player) = 0;
};
