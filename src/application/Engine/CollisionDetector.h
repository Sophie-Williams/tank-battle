#pragma once
#include <list>
#include "DrawableObject.h"
#include <vector>

class CollisionDetector
{
public:
	CollisionDetector();
	virtual ~CollisionDetector();

	// check collision for an object with all monitored objects at specific time
	bool checkCollision(DrawableObject* object1, DrawableObject* object2, float at);

	bool checkCollision2d(const std::vector<ci::vec2>& poly1, const std::vector<ci::vec2>& poly2, std::vector<ci::vec2>& convexPoly);
	static void transform(std::vector<ci::vec2>& points, const glm::mat4& m);
};

