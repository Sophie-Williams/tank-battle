#pragma once
#include "DrawableObject.h"

class LiveObject : public DrawableObject
{
public:
protected:
	float _health;
	float _shield;
protected:
public:
	LiveObject();
	virtual ~LiveObject();
	
	void setHealth(float health);
	float getHealth() const;

	void setShield(float shield);
	float getShield() const;

	void takeDamage(float damage);
};