/******************************************************************
* File:        GameObject.h
* Description: declare GameObject class. This is a base class
*              of any game object.
* Author:      Vincent Pham
*
* Copyright (c) 2018 VincentPT.
** Distributed under the MIT License (http://opensource.org/licenses/MIT)
**
*
**********************************************************************/

#pragma once
#include <list>

#include "GameControllerInterfaces.h"
#include "GameComponent.h"

class GameObject
{
protected:
	GameObjectId _objectId;
	bool _available;
	std::list<GameComponentRef> _components;
protected:
public:
	GameObject();
	virtual ~GameObject();

	virtual void update(float t);
	
	virtual bool isAvailable() const;
	virtual void setAvailable(bool flag);
	virtual void destroy(float t);
	virtual void addComponent(const GameComponentRef& component);
	virtual const std::list<GameComponentRef>& getComponents() const;
	
	GameObjectId getId() const;
};

typedef std::shared_ptr<GameObject> GameObjectRef;