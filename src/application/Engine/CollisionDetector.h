/******************************************************************
* File:        CollisionDetector.h
* Description: declare CollisionDetector class. This class detects
*              collisions in the game.
* Author:      Vincent Pham
*
* Copyright (c) 2018 VincentPT.
** Distributed under the MIT License (http://opensource.org/licenses/MIT)
**
*
**********************************************************************/

#pragma once
#include <list>
#include "DrawableObject.h"
#include <vector>
#include "../common/Geometry.h"

class CollisionDetector
{
	float _lastUpdate;
protected:
public:
	CollisionDetector();
	virtual ~CollisionDetector();

	// check collision for an object with all monitored objects at specific time
	bool checkCollision(DrawableObject* object1, DrawableObject* object2, float at);

	bool checkCollision2d(const std::vector<ci::vec2>& poly1, const std::vector<ci::vec2>& poly2, std::vector<ci::vec2>& convexPoly);

	int checkCollision2d(const std::vector<ci::vec2>& poly1, const std::vector<ci::vec2>& poly2, PolyIntersectionInfo<ci::vec2>& intersectAt);

	std::pair<float,float> findCollideTime(float beginTime, float colliedTime,
		const std::vector<ci::vec2>& staticBound, std::vector<ci::vec2>& dynamicBoundBuffer, DrawableObject* dynamicObject);
	std::pair<float, float> findCollideTime(
		float beginTime, float objectState1, float objectState2,
		std::vector<ci::vec2>& dynamicBoundBuffer1, DrawableObject* dynamicObject1,
		std::vector<ci::vec2>& dynamicBoundBuffer2, DrawableObject* dynamicObject2);

	void resolveCollisions_old(std::list<DrawableObjectRef>& objects, float t);
	void resolveCollisions(std::list<DrawableObjectRef>& objects, float t);

	static void transform(std::vector<ci::vec2>& points, const glm::mat4& m);
	static void rayClipPolygon(const ci::vec2& P, const ci::vec2& u, const std::vector<ci::vec2>& poly, std::vector<ci::vec2>& polyOut);
	static void polyIntersect(const std::vector<ci::vec2>& poly1, const std::vector<ci::vec2>& poly2, std::vector<ci::vec2>& polyOut);
};

