/******************************************************************
* File:        GameInterface.h
* Description: declare GameInterface class. This is an abstract class
*              provide information of game to plugged-in libraries.
* Author:      Vincent Pham
*
* Copyright (c) 2018 VincentPT.
** Distributed under the MIT License (http://opensource.org/licenses/MIT)
**
*
**********************************************************************/

#pragma once;
#include "GameControllerInterfaces.h"

class GAME_CONTROLLER_INTERFACE GameInterface {
public:
	GameInterface();
	virtual ~GameInterface();
	static GameInterface* getInstance();

	virtual float getObjectSpeed(GameObjectType) const = 0;
	virtual float getObjectHealth(GameObjectId id) const = 0;
	virtual float getTime() const = 0;

	virtual GameObjectType getObjectype(GameObjectId id) const = 0;
	virtual void printMessage(const char* header, const char* message) const = 0;
};