#pragma once
#include "TexturedObject.h"
#include "cinder/gl/gl.h"

class TankBarrel : public TexturedObject
{
public:
	TankBarrel();
	virtual ~TankBarrel();

	virtual void update(float t);
};