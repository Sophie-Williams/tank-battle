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