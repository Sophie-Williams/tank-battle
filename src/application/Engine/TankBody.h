#pragma once
#include "TexturedObject.h"

class TankBody : public TexturedObject
{
public:
	TankBody();
	virtual ~TankBody();

protected:
	virtual void updateInternal(float t);
};