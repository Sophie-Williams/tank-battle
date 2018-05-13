#pragma once
#include "TexturedObject.h"
#include "cinder/gl/gl.h"

class TankBody : public TexturedObject
{
public:
	TankBody();
	virtual ~TankBody();

	virtual void update(float t);
};