#include "CollisionDetector.h"
#include "Geometry.h"

using namespace ci;

CollisionDetector::CollisionDetector() {}

CollisionDetector::~CollisionDetector() {
}

void CollisionDetector::transform(std::vector<ci::vec2>& points, const glm::mat4& m) {
	for (auto it = points.begin(); it != points.end(); it++) {
		glm::mat4::col_type p4(*it, 0, 1);
		p4 = m * p4;

		it->x = p4.x;
		it->y = p4.y;
	}
}

bool CollisionDetector::checkCollision(DrawableObject* object1, DrawableObject* object2, float at) {
	if (object1 == object2) {
		return false;
	}

	auto& rect1 = object1->getBound();
	auto& m1 = object1->getTransformation();
	std::vector<ci::vec2> poly1 = {
		rect1.getUpperLeft(), rect1.getUpperRight(), rect1.getLowerRight(), rect1.getLowerLeft()
	};

	auto& rect2 = object2->getBound();
	auto& m2 = object2->getTransformation();
	std::vector<ci::vec2> poly2 = {
		rect2.getUpperLeft(), rect2.getUpperRight(), rect2.getLowerRight(), rect2.getLowerLeft()
	};

	// transform local bounding poly of object 1 to world space
	transform(poly1, m1);
	// transform local bounding poly of object 2 to world space
	transform(poly2, m2);

	int m = (int)poly1.size();
	int n = (int)poly2.size();

	// check if poly 1 contains any vertex of poly 2 and reverse
	for (int i = 0; i < n; i++) {
		auto& Q = poly2[i];
		if (isPointInside(poly1, Q) >= 0) {
			return true;
		}
	}
	for (int i = 0; i < m; i++) {
		auto& Q = poly1[i];
		if (isPointInside(poly2, Q) >= 0) {
			return true;
		}
	}

	// check if any segment of two polygon intersect
	float t1, t2;
	for (int i = 0; i < m; i++) {
		auto& P1 = poly1[i];
		auto u1 = poly1[(i + 1) % m] - P1;
		for (int j = 0; j < n; j++) {
			auto& P2 = poly2[j];
			auto u2 = poly2[(j + 1) % n] - P2;
			
			// two segment must be intersect at the middle of each other
			if (Intersect2D_Lines(P1, u1, P2, u2, &t1, &t2) && t1 >= 0 && t1 <= 1 && t2 >= 0 && t2 <= 1) {
				return true;
			}
		}
	}

	return false;
}

bool CollisionDetector::checkCollision2d(const std::vector<ci::vec2>& poly1, const std::vector<ci::vec2>& poly2, std::vector<ci::vec2>& convexPoly) {
	std::list<ci::vec2> intersectPoints;

	int lieOnEdgeCount = 0;
	int insidePolygonCount = 0;
	int intersectCount = 0;

	int m = (int)poly1.size();
	int n = (int)poly2.size();

	char c;

	// check if poly 1 contains any vertex of poly 2 and reverse
	for (int i = 0; i < n; i++) {
		auto& Q = poly2[i];
		c = isPointInside(poly1, Q);
		if (c == 0) {
			lieOnEdgeCount++;
			intersectPoints.push_back(Q);
		}
		else if (c > 0) {
			insidePolygonCount++;
			intersectPoints.push_back(Q);
		}
	}
	for (int i = 0; i < m; i++) {
		auto& Q = poly1[i];
		c = isPointInside(poly2, Q);
		if (c == 0) {
			lieOnEdgeCount++;
			intersectPoints.push_back(Q);
		}
		else if (c > 0) {
			insidePolygonCount++;
			intersectPoints.push_back(Q);
		}
	}

	// check if any segment of two polygon intersect
	float t1, t2;
	for (int i = 0; i < m; i++) {
		auto& P1 = poly1[i];
		auto u1 = poly1[(i + 1) % m] - P1;
		for (int j = 0; j < n; j++) {
			auto& P2 = poly2[j];
			auto u2 = poly2[(j + 1) % n] - P2;

			// two segment must be intersect at the middle of each other
			// don't count the intersection point at two construct points of the segment
			if (Intersect2D_Lines(P1, u1, P2, u2, &t1, &t2) && t1 > 0 && t1 < 1 && t2 > 0 && t2 < 1) {
				intersectCount++;
				intersectPoints.push_back( P1 + u1 * t1);
			}
		}
	}

	if (intersectPoints.size() > 2) {
		// build convex poly
		// no need to build convex poly for three points because it is a triangle
		if (intersectPoints.size() == 3) {
			auto it = intersectPoints.begin();
			convexPoly.resize(3);
			convexPoly[0] = *it++;
			convexPoly[1] = *it++;
			convexPoly[2] = *it++;
		}
		else {

		}
	}

	return intersectPoints.size() > 0;
}