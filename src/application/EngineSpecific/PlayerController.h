/******************************************************************
* File:        PlayerController.h
* Description: declare PlayerController class. This is abstract class
*              responsible for controlling a tank in game.
* Author:      Vincent Pham
*
* Copyright (c) 2018 VincentPT.
** Distributed under the MIT License (http://opensource.org/licenses/MIT)
**
*
**********************************************************************/

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