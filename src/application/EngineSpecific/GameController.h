/******************************************************************
* File:        GameController.h
* Description: declare GameController class. This class is
*              responsible for controlling game logic.
* Author:      Vincent Pham
*
* Copyright (c) 2018 VincentPT.
** Distributed under the MIT License (http://opensource.org/licenses/MIT)
**
*
**********************************************************************/

#pragma once
#include "engine/GameObject.h"
#include "engine/DrawableObject.h"
#include <map>

class GameController
{
private:
	std::map<GameObjectId, int> _killCounter;
protected:
	GameController();
public:
	virtual~GameController();
	static GameController* getInstance();

	void OnBulletCollisionDetected(GameObjectRef bullet, DrawableObjectRef other, const CollisionInfo& poistion, float t);
	int getKills(GameObjectId id);
};