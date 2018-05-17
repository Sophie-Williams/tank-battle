#include "CollisionDetector.h"
#include "Geometry.h"

using namespace ci;

CollisionDetector::CollisionDetector() {}

CollisionDetector::~CollisionDetector() {
}

void transform(std::vector<ci::vec2>& points, const glm::mat4& m) {
	glm::mat3 aa;
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

	// check if poly 1 contains any vertex of poly 2 and reverse
	for (int i = 0; i < (int)poly2.size(); i++) {
		auto& Q = poly2[i];
		if (isPointInside(poly1, Q)) {
			return true;
		}
	}
	for (int i = 0; i < (int)poly1.size(); i++) {
		auto& Q = poly1[i];
		if (isPointInside(poly2, Q)) {
			return true;
		}
	}

	return false;
}