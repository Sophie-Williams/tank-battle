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

	struct VertexConnection {
		// position as parameter t of the vertex on its edge
		float t;
		BuildingVertex* vertex;
	};

	struct BuildingEdge
	{
		std::list<VertexConnection> buildingVertices;
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
	auto buildMapData1 = [&buildingVertices, &findDuplicateVertex](const ci::vec2& P, BuildingEdge& edge1, BuildingEdge& edge2, float t1, float t2) {
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
		edge1.buildingVertices.push_back({ t1, pVertex });
		edge2.buildingVertices.push_back({ t2, pVertex });
	};
	auto checkInsideAndBuildMapData = [&buildMapData1](const std::vector<ci::vec2>& poly1, const std::vector<ci::vec2>& poly2, std::vector<BuildingEdge>& edges) {
		int n = (int)poly2.size();
		for (int i = 0; i < n; i++) {
			auto& P = poly2[i];
			if (isPointInside2(poly1, P)) {
				auto& edge1 = i == 0 ? edges[n - 1] : edges[i - 1];
				auto& edge2 = edges[i];
				buildMapData1(P, edge1, edge2, 1.0f, 0.0f);
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
					buildMapData1(P, edge1, edge2, t1, t2);
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
							buildMapData1(v21, edge1, edge2, t, 0.0f);
						}

						// check if v22 is in middle of segment 1
						t = computeT(u1, v22 - v11);
						if (t >= 0.0f && t <= 1.0f) {
							// build map data
							buildMapData1(v22, edge1, edge2, t, 1.0f);
						}
					}

					// compute parameter t of v11, v12 on segment 2
					if (u2.x || u2.y) {
						// check if v11 is in middle of segment 2
						auto t = computeT(u2, v11 - v21);
						if (t >= 0.0f && t <= 1.0f) {
							// build map data
							buildMapData1(v11, edge1, edge2, 0.0f, t);
						}

						// check if v12 is in middle of segment 2
						t = computeT(u2, v12 - v21);
						if (t >= 0.0f && t <= 1.0f) {
							// build map data
							buildMapData1(v12, edge1, edge2, 1.0f, t);
						}
					}
				}
			}
		}
	}

	if (buildingVertices.size() > 2) {
		// build convex poly
		// no need to build convex poly for three points because it is a triangle
		if (buildingVertices.size() == 3) {
			auto it = buildingVertices.begin();
			convexPoly.resize(3);
			convexPoly[0] = it->coord;
			it++;
			convexPoly[1] = it->coord;
			it++;
			convexPoly[2] = it->coord;
		}
		else {
			// before further processing, we need to sort the connected vertices on each edge
			// they must be in an order either decrease or increase of parameter t

			// do sorting
			auto compareConnection = [](const VertexConnection& v1, const VertexConnection& v2) -> bool {
				return v1.t < v2.t;
			};

			auto sortConnections = [&compareConnection](std::vector<BuildingEdge>& edges) {
				for (auto it = edges.begin(); it != edges.end(); it++) {
					auto& connectionVertices = it->buildingVertices;
					if (connectionVertices.size() > 2) {
						connectionVertices.sort(compareConnection);
					}
				}
			};

			sortConnections(edges1);
			sortConnections(edges2);

			// take a vertex
			auto it = buildingVertices.begin();
			// take the first edge that the edge belong to and search the connected edge
			auto pEdge = it->belongEdges.front();

			auto pConntectedVertices = &pEdge->buildingVertices;
			// take the first vertex in its connected vertices as first vertex of the looking polygon
			auto jt = pConntectedVertices->begin();
			auto pVertex = jt->vertex;
			auto pBeginVertexInChain = pVertex;

			std::list<BuildingVertex*> lookingPoly;
			lookingPoly.push_back(pBeginVertexInChain);

			auto pFirstVertex = pBeginVertexInChain;

			bool foundPoly = false;
			while (true) {
				// make connection chain in siblings vertices
				// spin loop in the connected vertices
				for (jt++; true; jt++) {
					if (jt == pConntectedVertices->end()) {
						jt = pConntectedVertices->begin();
					}

					pVertex = jt->vertex;

					// break the loop when it meet the begin vertex again
					if (pVertex == pBeginVertexInChain) {
						pVertex = lookingPoly.back();
						break;
					}
					// check if connection chain is meet the first vertex
					// it means the connection chain is closed and it is the polygon has been looking for
					if (pVertex == pFirstVertex) {
						foundPoly = true;
						break;
					}
					//// check if new vertex is not exist in current list
					//auto kt = lookingPoly.begin();
					//for (; kt != lookingPoly.end(); kt++) {
					//	if (*kt == pVertex) {
					//		break;
					//	}
					//}
					//if (kt == lookingPoly.end()) {
					//	lookingPoly.push_back(pVertex);
					//}
					lookingPoly.push_back(pVertex);
				}
				if (foundPoly) {
					break;
				}
				// find a vertex that connect to current edge by another edge
				// connected edges for current vertex
				auto& connectedEdges = pVertex->belongEdges;
				auto newEdege = pEdge;
				for (auto kt = connectedEdges.begin(); kt != connectedEdges.end(); kt++) {
					newEdege = *kt;
					if (newEdege != pEdge) {
						// check new edge is a duplicated edge
						// now the edge is contain current vertex, if it contain another vertex on existing vertices
						// it means the edge is duplicated with a existing edge
						auto lt = lookingPoly.rbegin();
						lt++;
						// another existing vertex that same edge with current edge
						auto sameEdgeVertex = *lt;
						auto& connectedVertices = newEdege->buildingVertices;
						auto ht = connectedVertices.begin();
						for (; ht != connectedVertices.end(); ht++) {
							if (sameEdgeVertex == ht->vertex) {
								break;
							}
						}
						if (ht != connectedVertices.end()) {
							// don't pass throughout the loop
							// it means the edge is duplicated
							// so, we reset the new edge is old edge to continue search for another edge
							newEdege = pEdge;
							continue;
						}
						break;
					}
				}

				// check if the new edge is found
				if (newEdege == pEdge) {
					// can not found the new edge
					break;
				}
				// change the contex to the new edge
				// current edge become new edge
				pEdge = newEdege;
				// update connected vertices
				pConntectedVertices = &pEdge->buildingVertices;
				// find the iterator contain current vetex
				for (jt = pConntectedVertices->begin(); jt != pConntectedVertices->end(); jt++) {
					if (pVertex == jt->vertex) {
						pBeginVertexInChain = pVertex;
						break;
					}
				}
				// not found its self on the connected edge
				// it should have something wrong in this algorithm
				if (jt == pConntectedVertices->end()) {
					break;
				}
			}
			
			if (foundPoly == false) {
				// if the foundPoly is not found there are something wrong with this algorithm or
				// the input argument is invalid
				// although the collision is detected, but for the warning purpose
				// we return false on this case, to prevent further wrong
				return false;
			}

			// the found polygon should have more than three vertices
			// if it is not, there are something wrong in this algorithm or invalid input argument
			if (lookingPoly.size() < 4) {
				// although the collision is detected, but for the warning purpose
				// we return false on this case, to prevent further wrong
				return false;
			}
			convexPoly.resize(lookingPoly.size());
			int idx = 0;
			for (auto it = lookingPoly.begin(); it != lookingPoly.end(); it++) {
				convexPoly[idx++] = (*it)->coord;
			}
		}
	}

	return buildingVertices.size() > 0;
}

int CollisionDetector::checkCollision2d(const std::vector<ci::vec2>& poly1, const std::vector<ci::vec2>& poly2) {
	return checkIntersect2d(poly1, poly2);
}

// smallest check duration is 1/32 of 
constexpr float smallestDuration = 1.0f / 60 / 32;

float CollisionDetector::findEarliestCollideTime(float beginTime, float colliedTime,
	const std::vector<ci::vec2>& staticBound, std::vector<ci::vec2>& dynamicBoundBuffer, DrawableObject* dynamicObject) {
	float tStart = beginTime;
	float tEnd = colliedTime;
	float t, duration;

	t = beginTime;

	// backup transformation
	auto backupTransform = dynamicObject->getTransformation();

	bool loop = true;
	int checkResult;
	while (loop && (duration = (tEnd - tStart)) > smallestDuration) {
		t = tStart + duration / 2;

		dynamicObject->update(t);
		dynamicObject->getBoundingPoly(dynamicBoundBuffer);
		
		checkResult = checkIntersect2d(staticBound, dynamicBoundBuffer);
		
		if (checkResult == 0) {
			// two objects just touch each other
			// the perfect state to break the loop
			loop = false;
		}
		else if (checkResult == 1) {
			// collided, try to find ealier time
			tEnd = t;
			t = tStart;
		}
		else {
			// not collide, try to find collide time
			tStart = t;
			// save the collied time to t incase the loop breaks
			//t = tEnd;
		}

		// restore transformation
		dynamicObject->update(beginTime);
		dynamicObject->setTransformation(backupTransform);
	}

	return t;
}