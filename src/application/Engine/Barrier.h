#pragma once
#include "GameObject.h"

class Barrier : public GameObject
{
public:
	Barrier();
	virtual ~Barrier();

protected:
	virtual void updateInternal(float t);
	virtual void drawInternal();
};