/******************************************************************
* File:        GameComponent.cpp
* Description: implement GameComponent class. This is a base class
*              of any game object.
* Author:      Vincent Pham
*
* Copyright (c) 2018 VincentPT.
** Distributed under the MIT License (http://opensource.org/licenses/MIT)
**
*
**********************************************************************/

#include "GameComponent.h"

GameComponent::GameComponent() : _owner(nullptr), _isAvailble(true){
}

GameComponent::~GameComponent() {
	destroy();
}

void GameComponent::destroy() {
	_isAvailble = false;
}

bool GameComponent::isAvailable() {
	return _isAvailble;
}