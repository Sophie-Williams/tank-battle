/******************************************************************
* File:        SimplePlayer.h
* Description: declare SimplePlayer class. This is a tank controller
*              class.
* Author:      Vincent Pham
*
* Copyright (c) 2018 VincentPT.
** Distributed under the MIT License (http://opensource.org/licenses/MIT)
**
*
**********************************************************************/

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
