#pragma once
#include "GameControllerInterfaces.h"
#include "TankCommandsBuilder.h"
#include "PlayerContext.h"
#include "GameInterface.h"

class GAME_CONTROLLER_INTERFACE TankController
{
	char* _playerName;
public:
	TankController();
	virtual ~TankController();
	virtual void setup(TankPlayerContext* player);
	virtual void cleanup(TankPlayerContext* player);
	virtual TankOperations giveOperations(TankPlayerContext* player) = 0;
	virtual void setName(const char* name);
	virtual const char* getName() const;
};
