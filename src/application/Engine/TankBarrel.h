#pragma once
#include "TexturedObject.h"

class TankBarrel : public TexturedObject
{
public:
	TankBarrel();
	virtual ~TankBarrel();
protected:
	virtual void updateInternal(float t);
};