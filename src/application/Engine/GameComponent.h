/******************************************************************
* File:        GameComponent.h
* Description: declare GameComponent class. This is a base class
*              of any game object.
* Author:      Vincent Pham
*
* Copyright (c) 2018 VincentPT.
** Distributed under the MIT License (http://opensource.org/licenses/MIT)
**
*
**********************************************************************/

#pragma once
#include <memory>
class GameObject;

class GameComponent
{
protected:
	GameObject* _owner;
	bool _isAvailble;
protected:
	void destroy();
	bool isAvailable();
public:
	GameComponent();
	virtual ~GameComponent();

	virtual void update(float t) = 0;

	friend class GameObject;
};

typedef std::shared_ptr<GameComponent> GameComponentRef;