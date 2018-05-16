#pragma once
#include <list>
#include "GameObject.h"

class CollisionDetector
{
public:
	CollisionDetector();
	virtual ~CollisionDetector();

	// check collision for an object with all monitored objects at specific time
	bool checkCollision(GameObjectRef object1, GameObjectRef object2, float at);
};

