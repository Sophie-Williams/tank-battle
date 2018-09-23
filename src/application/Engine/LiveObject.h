/******************************************************************
* File:        LiveObject.h
* Description: declare LiveObject class. A LiveObject is an object
*              has a limited health capacity.
* Author:      Vincent Pham
*
* Copyright (c) 2018 VincentPT.
** Distributed under the MIT License (http://opensource.org/licenses/MIT)
**
*
**********************************************************************/

#pragma once
#include "DrawableObject.h"
#include <functional>

class LiveObject : public DrawableObject
{
public:
protected:
	float _health;
	float _shield;
	float _healthCap;
public:
	LiveObject();
	virtual ~LiveObject();
	
	void setHealth(float health);
	float getHealth() const;

	void setShield(float shield);
	float getShield() const;

	void takeDamage(float damage);
	void drawHeathBar();
};