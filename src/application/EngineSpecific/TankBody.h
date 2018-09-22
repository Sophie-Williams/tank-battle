#pragma once
#include "engine/TexturedObject.h"

class TankBody : public TexturedObject
{
public:
	TankBody();
	virtual ~TankBody();

protected:
	virtual void updateInternal(float t);
};