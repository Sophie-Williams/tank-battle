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
	struct BuildingEdge;
	struct BuildingVertex {
		ci::vec2 coord;
		std::list<BuildingEdge*> belongEdges;
	};

	struct BuildingEdge
	{
		std::list<BuildingVertex*> buildingVertices;
	};

	int m = (int)poly1.size();
	int n = (int)poly2.size();
	std::list<BuildingVertex> buildingVertices;

	auto findDuplicateVertex = [&buildingVertices](const ci::vec2& P) -> BuildingVertex* {
		for (auto it = buildingVertices.begin(); it != buildingVertices.end(); it++) {
			auto& coord = it->coord;
			if (coord.x == P.x && coord.y == P.y) {
				return &*it;
			}
		}
		return nullptr;
	};

	std::vector<BuildingEdge> edges1(m);
	std::vector<BuildingEdge> edges2(n);

	// check if any vertex of poly 2 is inside of poly 1 and build map data
	auto buildMapData1 = [&buildingVertices, &findDuplicateVertex](const ci::vec2& P, BuildingEdge& edge1, BuildingEdge& edge2) {
		// find duplicate vertex
		auto pVertex = findDuplicateVertex(P);
		if (pVertex == nullptr) {
			// add new vertex if it is not exist
			BuildingVertex newVertex;
			newVertex.coord = P;
			buildingVertices.push_back(newVertex);

			pVertex = &buildingVertices.back();
		}
		// add connected edges for current vertex
		pVertex->belongEdges.push_back(&edge1);
		pVertex->belongEdges.push_back(&edge2);

		// add current vertex to connected edges
		edge1.buildingVertices.push_back(pVertex);
		edge2.buildingVertices.push_back(pVertex);
	};
	auto checkInsideAndBuildMapData = [&buildMapData1](const std::vector<ci::vec2>& poly1, const std::vector<ci::vec2>& poly2, std::vector<BuildingEdge>& edges) {
		int n = (int)poly2.size();
		for (int i = 0; i < n; i++) {
			auto& P = poly2[i];
			if (isPointInside2(poly1, P)) {
				auto& edge1 = i == 0 ? edges[n - 1] : edges[i - 1];
				auto& edge2 = edges[i];
				buildMapData1(P, edge1, edge2);
			}
		}
	};

	// find inside vertices to each other and build map data for poly1 and poly2
	checkInsideAndBuildMapData(poly1, poly2, edges2);
	checkInsideAndBuildMapData(poly2, poly1, edges1);

	// check if any segment of two polygon intersect
	float t1, t2;
	for (int i = 0; i < m; i++) {
		auto& v11 = poly1[i];
		auto& v12 = poly1[(i + 1) % m];
		auto u1 = v12 - v11;

		//  building edeg1
		auto& edge1 = edges1[i];

		for (int j = 0; j < n; j++) {
			auto& v21 = poly2[j];
			auto& v22 = poly2[(j + 1) % n];
			auto u2 = v22 - v21;

			//  building edeg2
			auto& edge2 = edges2[j];

			// two segment must be intersect at the middle of each other
			// don't count the intersection point at two construct points of the segment
			if (Intersect2D_Lines(v11, u1, v21, u2, &t1, &t2)) {
				// intersect at middle of each line
				if ((t1 >= 0.0f && t1 <= 1.0f) && (t2 >= 0.0f && t2 <= 1.0f)) {

					// intersection point
					auto P = v11 + u1 * t1;

					// build map data
					buildMapData1(P, edge1, edge2);
				}
			}
			else {
				// now two lines are parallel or overlap
				if (compute(v11, u1, v21) == 0) {
					// now two lines are overlap
					// compute parameter t of v21, v22 on segment 1
					if (u1.x || u1.y) {
						// check if v21 is in middle of segment 1
						auto t = computeT(u1, v21 - v11);
						if (t >= 0.0f && t <= 1.0f) {
							// build map data
							buildMapData1(v21, edge1, edge2);
						}

						// check if v22 is in middle of segment 1
						t = computeT(u1, v22 - v11);
						if (t >= 0.0f && t <= 1.0f) {
							// build map data
							buildMapData1(v22, edge1, edge2);
						}
					}

					// compute parameter t of v11, v12 on segment 2
					if (u2.x || u2.y) {
						// check if v11 is in middle of segment 2
						auto t = computeT(u2, v11 - v21);
						if (t >= 0.0f && t <= 1.0f) {
							// build map data
							buildMapData1(v11, edge1, edge2);
						}

						// check if v12 is in middle of segment 2
						t = computeT(u2, v12 - v21);
						if (t >= 0.0f && t <= 1.0f) {
							// build map data
							buildMapData1(v12, edge1, edge2);
						}
					}
				}
			}
		}
	}

	//if (intersectPoints.size() > 2) {
	//	// build convex poly
	//	// no need to build convex poly for three points because it is a triangle
	//	if (intersectPoints.size() == 3) {
	//		auto it = intersectPoints.begin();
	//		convexPoly.resize(3);
	//		convexPoly[0] = *it++;
	//		convexPoly[1] = *it++;
	//		convexPoly[2] = *it++;
	//	}
	//	else {

	//	}
	//}

	return buildingVertices.size() > 0;
}