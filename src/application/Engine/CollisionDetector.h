#pragma once
#include <list>
#include "DrawableObject.h"

class CollisionDetector
{
public:
	CollisionDetector();
	virtual ~CollisionDetector();

	// check collision for an object with all monitored objects at specific time
	bool checkCollision(DrawableObject* object1, DrawableObject* object2, float at);
};

