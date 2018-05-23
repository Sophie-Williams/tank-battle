#pragma once
#include "TexturedObject.h"
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
};