/******************************************************************
* File:        Bullet.h
* Description: declare Bullet class. Instances of this class are
*              bullets in game. They can hurt a lived object.
* Author:      Vincent Pham
*
* Copyright (c) 2018 VincentPT.
** Distributed under the MIT License (http://opensource.org/licenses/MIT)
**
*
**********************************************************************/

#pragma once
#include "engine/TexturedObject.h"
#include "cinder/gl/gl.h"

class Bullet : public TexturedObject
{
protected:
	float _lastUpdate;
	float _movingSpeed;
	float _damaged;
	// the owner object of the bullet where it was fired
	// it can be null
	GameObjectRef _owner;
protected:
	virtual void drawInternal();
	virtual void updateInternal(float t);
public:
	// constructor of a bullet need to know that time it is created
	Bullet(float t);
	virtual ~Bullet();

	void setSpeed(float speed);
	float getSpeed() const;
	void setSize(const float& w, const float& h);
	void setOwner(GameObjectRef owner);
	GameObjectRef getOwner() const;

	void setDamage(float damage);
	float getDamage() const;

	void destroy(float t);
	bool canBeWentThrough(DrawableObject* other) const;
};