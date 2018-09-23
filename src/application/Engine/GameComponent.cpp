/******************************************************************
* File:        GameComponent.h
* Description: Implement GameComponent class. A gameComponent object
*              is contained in a game object. It may define behavior
*              of that object.
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