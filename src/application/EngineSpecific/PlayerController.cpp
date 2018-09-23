/******************************************************************
* File:        PlayerController.cpp
* Description: implement PlayerController class. This is abstract class
*              responsible for controlling a tank in game.
* Author:      Vincent Pham
*
* Copyright (c) 2018 VincentPT.
** Distributed under the MIT License (http://opensource.org/licenses/MIT)
**
*
**********************************************************************/

#include "PlayerController.h"
PlayerController::PlayerController(std::shared_ptr<Tank> player) : _player(player) {
}

PlayerController::~PlayerController() {}