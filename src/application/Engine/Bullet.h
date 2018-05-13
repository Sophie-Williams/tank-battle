#pragma once
#include "TexturedObject.h"
#include "cinder/gl/gl.h"

class Bullet : public TexturedObject
{
protected:
	float _lastUpdate;
	float _movingSpeed;
public:

	// constructor of a bullet need to know that time it is created
	Bullet(float t);
	virtual ~Bullet();

	void setSpeed(float speed);
	float getSpeed() const;
	virtual void update(float t);
	virtual void setSize(const float& w, const float& h);
};