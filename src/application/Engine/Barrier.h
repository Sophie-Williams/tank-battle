#pragma once
#include "DrawableObject.h"

class Barrier : public DrawableObject
{
public:
	Barrier();
	virtual ~Barrier();

protected:
	virtual void updateInternal(float t);
	virtual void drawInternal();
};