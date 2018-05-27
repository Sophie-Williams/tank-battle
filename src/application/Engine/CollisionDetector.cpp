#include "CollisionDetector.h"
#include "Geometry.h"
#include <memory>

using namespace ci;
using namespace std;

CollisionDetector::CollisionDetector() : _lastUpdate(0) {}

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

std::pair<float, float> CollisionDetector::findCollideTime(float beginTime, float colliedTime,
	const std::vector<ci::vec2>& staticBound, std::vector<ci::vec2>& dynamicBoundBuffer, DrawableObject* dynamicObject) {

	std::pair<float, float> collisionTimeRange;
	auto& tStart = collisionTimeRange.first;
	auto& tEnd = collisionTimeRange.second;

	tStart = beginTime;
	tEnd = colliedTime;
	float t, duration;

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
		}
		else {
			// not collide, try to find collide time
			tStart = t;
		}

		// restore transformation
		dynamicObject->update(beginTime);
		dynamicObject->setTransformation(backupTransform);
	}

	return collisionTimeRange;
}

std::pair<float, float> CollisionDetector::findCollideTime(
	float beginTime, float object1State, float object2State,
	std::vector<ci::vec2>& dynamicBoundBuffer1, DrawableObject* dynamicObject1,
	std::vector<ci::vec2>& dynamicBoundBuffer2, DrawableObject* dynamicObject2) {
	std::pair<float, float> collisionTimeRange;
	auto& tStart = collisionTimeRange.first;
	auto& tEnd = collisionTimeRange.second;

	//auto endTime = std::max(object1State, object2State);
	auto endTime = std::min(object1State, object2State);

	tStart = beginTime;
	tEnd = endTime;
	float t, duration;

	// backup transformations
	auto backupTransform1 = dynamicObject1->getTransformation();
	auto backupTransform2 = dynamicObject2->getTransformation();

	bool loop = true;
	int checkResult;
	while (loop && (duration = (tEnd - tStart)) > smallestDuration) {
		t = tStart + duration / 2;

		if (t > object1State) {
			dynamicObject1->update(object1State);
		}
		else {
			dynamicObject1->update(t);
		}
		dynamicObject1->getBoundingPoly(dynamicBoundBuffer1);

		if (t > object2State) {
			dynamicObject2->update(object2State);
		}
		else {
			dynamicObject2->update(t);
		}
		dynamicObject2->getBoundingPoly(dynamicBoundBuffer2);

		checkResult = checkIntersect2d(dynamicBoundBuffer1, dynamicBoundBuffer2);

		if (checkResult == 0) {
			// two objects just touch each other
			// the perfect state to break the loop
			loop = false;
		}
		else if (checkResult == 1) {
			// collided, try to find ealier time
			tEnd = t;
		}
		else {
			// not collide, try to find collide time
			tStart = t;
		}

		// restore transformations
		dynamicObject1->update(beginTime);
		dynamicObject1->setTransformation(backupTransform1);
		dynamicObject2->update(beginTime);
		dynamicObject2->setTransformation(backupTransform2);
	}

	return collisionTimeRange;
}

inline void updateTranformForCollision(DrawableObject* object, float availableTime, float currentTime) {
	// update to new frame at the exepected time
	object->update(availableTime);
	// keep the tranformation at the exepected time
	auto backupTransform = object->getTransformation();
	// update the object's state to the current time
	object->update(currentTime);
	// restore transforam at expected time
	object->setTransformation(backupTransform);
}

void CollisionDetector::resolveCollisions_old(std::list<DrawableObjectRef>& drawableObjects, float t) {
	struct CollisionCheckInfo {
		// object need to be check
		DrawableObjectRef object;

		mat4 previousMat;

		// the time that the the object's states was updated
		float objectStateTime;

		// collision time range
		// start at very last time that collision does not occur
		// end at very first time that collision occurs
		std::pair<float, float> collisionTimeRange;

		std::shared_ptr<std::vector<ci::vec2>> boundingPoly;
		// objects that have collide with the object
		std::shared_ptr<std::list<DrawableObjectRef>> colliedObjects;
	};
	std::list<CollisionCheckInfo> collisionCheckList;

	// build the collision check list first
	for (auto it = drawableObjects.begin(); it != drawableObjects.end(); it++) {
		auto& drawableObject = *it;
		if (drawableObject->isStaticObject() == false) {
			CollisionCheckInfo collisionCheckInfo;
			collisionCheckInfo.object = drawableObject;
			collisionCheckInfo.previousMat = drawableObject->getPreviousTransformation().first;
			collisionCheckInfo.objectStateTime = t;
			collisionCheckInfo.collisionTimeRange.first = t;
			collisionCheckInfo.collisionTimeRange.second = t;
			collisionCheckInfo.boundingPoly = std::make_shared<std::vector<ci::vec2>>(4);
			drawableObject->getBoundingPoly(*collisionCheckInfo.boundingPoly);

			collisionCheckList.push_back(collisionCheckInfo);
		}
	}

	// check collision with static object first
	std::vector<ci::vec2> boundingPolyStatic(4);
	DrawableObject* currentCollision = nullptr;

	for (auto jt = drawableObjects.begin(); jt != drawableObjects.end(); jt++) {
		auto& drawableObject = *jt;
		if (drawableObject->isAvailable() == false) {
			continue;
		}

		if (drawableObject->isStaticObject()) {
			drawableObject->getBoundingPoly(boundingPolyStatic);

			for (auto it = collisionCheckList.begin(); it != collisionCheckList.end(); it++) {
				currentCollision = it->object.get();

				if (drawableObject->canBeWentThrough() && drawableObject->getCollisionHandler() == nullptr && currentCollision->getCollisionHandler() == nullptr) {
					continue;
				}
				if (currentCollision->canBeWentThrough() && currentCollision->getCollisionHandler() == nullptr && drawableObject->getCollisionHandler() == nullptr) {
					continue;
				}

				auto& dynamicBound = *it->boundingPoly;
				if (checkCollision2d(dynamicBound, boundingPolyStatic) >= 0) {
					if (_lastUpdate > 0) {
						// restore the state of object if it available before the current update occur
						// update back to previous frame
						currentCollision->update(_lastUpdate);
						// ensure the tranformation is same as previous frame
						currentCollision->setTransformation(it->previousMat);

						auto collideTimeRange = findCollideTime(_lastUpdate, it->objectStateTime, boundingPolyStatic, dynamicBound, currentCollision);

						if (drawableObject->canBeWentThrough() || currentCollision->canBeWentThrough()) {
							// update the object's state to the state
							updateTranformForCollision(currentCollision, it->objectStateTime, t);

							collideTimeRange.first = std::min(it->collisionTimeRange.first, it->objectStateTime);
						}
						else {
							// update to new frame at collide time
							updateTranformForCollision(currentCollision, collideTimeRange.second, t);

							it->objectStateTime = collideTimeRange.second;
						}
						// update the buffer bounding polygon for lastest state of the object
						currentCollision->getBoundingPoly(dynamicBound);
						// update time when the dynamic object cannot move anymore because of current collision
						it->collisionTimeRange = collideTimeRange;
					}

					// set lastest collide object
					auto& collidedObjects = it->colliedObjects;
					if (collidedObjects == nullptr) {
						collidedObjects = make_shared<list<DrawableObjectRef>>();
					}
					collidedObjects->push_back(drawableObject);
				}
			}
		}
	}

	auto prevEnd = collisionCheckList.end();
	prevEnd--;
	for (auto it = collisionCheckList.begin(); it != prevEnd; it++) {
		auto& collisionInfo1 = *it;
		auto& drawableObject1 = collisionInfo1.object;
		auto& dynamicBound1 = *it->boundingPoly;
		if (drawableObject1->isAvailable() == false) {
			continue;
		}

		auto jt = it;
		for (jt++; jt != collisionCheckList.end(); jt++) {
			auto& collisionInfo2 = *it;
			auto& drawableObject2 = collisionInfo2.object;
			if (drawableObject2->isAvailable() == false) {
				continue;
			}

			if (drawableObject1->canBeWentThrough() && drawableObject1->getCollisionHandler() == nullptr && drawableObject2->getCollisionHandler() == nullptr) {
				continue;
			}
			if (drawableObject2->canBeWentThrough() && drawableObject2->getCollisionHandler() == nullptr && drawableObject1->getCollisionHandler() == nullptr) {
				continue;
			}
			auto& dynamicBound2 = *jt->boundingPoly;

			if (checkCollision2d(dynamicBound1, dynamicBound2) >= 0) {
				if (_lastUpdate > 0) {
					// restore the state of object if it available before the current update occur
					// update back to previous frame
					currentCollision->update(_lastUpdate);
					// ensure the tranformation is same as previous frame
					currentCollision->setTransformation(it->previousMat);

					auto collideTimeRange = findCollideTime(_lastUpdate, collisionInfo1.objectStateTime, collisionInfo2.objectStateTime,
						dynamicBound1, drawableObject1.get(), dynamicBound2, drawableObject2.get());

					if (drawableObject1->canBeWentThrough() || drawableObject2->canBeWentThrough()) {
						// update the object's state to the state
						updateTranformForCollision(drawableObject1.get(), collisionInfo1.objectStateTime, t);
						updateTranformForCollision(drawableObject2.get(), collisionInfo2.objectStateTime, t);

						collideTimeRange.first = std::min(it->collisionTimeRange.first, it->objectStateTime);
					}
					else {
						// update to new frame at collide time
						updateTranformForCollision(currentCollision, collideTimeRange.second, t);

						it->objectStateTime = collideTimeRange.second;
					}
					// update the buffer bounding polygon for lastest state of the object
					drawableObject1->getBoundingPoly(dynamicBound1);
					drawableObject2->getBoundingPoly(dynamicBound2);
					// update time when the dynamic object cannot move anymore because of current collision
					it->collisionTimeRange = collideTimeRange;
				}

				// set lastest collide object
				auto& collidedObjects = it->colliedObjects;
				if (collidedObjects == nullptr) {
					collidedObjects = make_shared<list<DrawableObjectRef>>();
				}
				collidedObjects->push_back(drawableObject1);
			}
		}
	}

	// recheck and remove out-of-date collision
	// update the transforms to the very last time before collision occur
	for (auto it = collisionCheckList.begin(); it != collisionCheckList.end(); it++) {
		auto& colliedObjects = it->colliedObjects;

		if (colliedObjects) {
			auto& object = it->object;
			auto& dynamicBound = *it->boundingPoly;

			// recheck and remove out-of-date collision
			for (auto kt = colliedObjects->begin(); kt != colliedObjects->end();) {
				(*kt)->getBoundingPoly(boundingPolyStatic);
				if (checkCollision2d(dynamicBound, boundingPolyStatic) < 0) {
					auto ktTemp = kt;
					kt++;
					colliedObjects->erase(ktTemp);
				}
				else {
					kt++;
				}
			}
			// update the transforms to the very last time before collision occur
			// restore the state of object if it available before the current update occur
			// update back to previous frame
			object->update(_lastUpdate);
			// ensure the tranformation is same as previous frame
			object->setTransformation(it->previousMat);

			updateTranformForCollision(object.get(), it->collisionTimeRange.first, t);
		}
	}

	// call collision event handler
	for (auto it = collisionCheckList.begin(); it != collisionCheckList.end(); it++) {
		auto& colliedObjects = it->colliedObjects;
		auto& object = it->object;
		if (colliedObjects) {
			auto& collisionHandler1 = object->getCollisionHandler();
			if (collisionHandler1) {
				for (auto kt = colliedObjects->begin(); kt != colliedObjects->end(); kt++) {
					auto& collisionHandler2 = (*kt)->getCollisionHandler();
					collisionHandler1(*kt, it->collisionTimeRange.first);
					if (collisionHandler2) {
						collisionHandler2(object, it->collisionTimeRange.first);
					}
				}
			}
			else {
				for (auto kt = colliedObjects->begin(); kt != colliedObjects->end(); kt++) {
					auto& collisionHandler2 = (*kt)->getCollisionHandler();
					if (collisionHandler2) {
						collisionHandler2(object, it->collisionTimeRange.first);
					}
				}
			}
		}
	}

	// update the last time update
	// must be executed before exit this function
	_lastUpdate = t;
}

void CollisionDetector::resolveCollisions(std::list<DrawableObjectRef>& drawableObjects, float t) {

	struct CollisionCheckInfo;
	struct CollisionNode {
		bool isMain;
		CollisionCheckInfo* pCollisionInfo;
	};
	struct CollisionCheckInfo {
		// object need to be check
		DrawableObjectRef object;

		// is fixed object
		bool fixedObject;

		// transformation of preivous frame
		mat4 previousMat;

		// the time that the the object's states was updated
		float objectStateTime;

		// collision time range
		// start at very last time that collision does not occur
		// end at very first time that collision occurs
		std::pair<float, float> collisionTimeRange;

		std::shared_ptr<std::vector<ci::vec2>> boundingPoly;
		// objects that have collide with the object
		std::shared_ptr<std::list<CollisionNode>> collisions;
	};
	std::list<CollisionCheckInfo> collisionCheckList;

	// build the collision check list first
	for (auto it = drawableObjects.begin(); it != drawableObjects.end(); it++) {
		auto& drawableObject = *it;
		CollisionCheckInfo collisionCheckInfo;
		collisionCheckInfo.object = drawableObject;
		collisionCheckInfo.previousMat = drawableObject->getPreviousTransformation().first;
		collisionCheckInfo.objectStateTime = t;
		collisionCheckInfo.collisionTimeRange.first = t;
		collisionCheckInfo.collisionTimeRange.second = t;
		collisionCheckInfo.boundingPoly = std::make_shared<std::vector<ci::vec2>>(4);
		collisionCheckInfo.fixedObject = drawableObject->isStaticObject();

		drawableObject->getBoundingPoly(*collisionCheckInfo.boundingPoly);

		collisionCheckList.push_back(collisionCheckInfo);
	}

	auto prevEnd = collisionCheckList.end();
	prevEnd--;
	for (auto it = collisionCheckList.begin(); it != prevEnd; it++) {
		auto& collisionInfo1 = *it;
		auto& object1 = collisionInfo1.object;
		auto& boundingPoly1 = *collisionInfo1.boundingPoly;
		if (object1->isAvailable() == false) {
			continue;
		}

		auto jt = it;
		for (jt++; jt != collisionCheckList.end(); jt++) {
			auto& collisionInfo2 = *jt;
			auto& object2 = collisionInfo2.object;

			// check if object1 or object2 is not avaible at current time
			if (object1->isAvailable() == false || object2->isAvailable() == false) {
				continue;
			}

			// if both object is fixed object, they don't need to be checked collision
			if (object1->isStaticObject() && object2->isStaticObject()) {
				continue;
			}

			// check if object1 or object2 is not a barrel and both object have no collision handler
			if ((object1->canBeWentThrough() || object2->canBeWentThrough()) && object1->getCollisionHandler() == nullptr && object2->getCollisionHandler() == nullptr) {
				continue;
			}

			auto& boundingPoly2 = *collisionInfo2.boundingPoly;

			if (checkCollision2d(boundingPoly1, boundingPoly2) >= 0) {
				if (_lastUpdate > 0) {
					if (object1->isStaticObject() == false && object2->isStaticObject() == false) {
						// restore the state of object if it available before the current update occur
						// update back to previous frame
						object1->update(_lastUpdate);
						object2->update(_lastUpdate);
						// ensure the tranformation is same as previous frame
						object1->setTransformation(collisionInfo1.previousMat);
						object2->setTransformation(collisionInfo2.previousMat);

						auto collideTimeRange = findCollideTime(_lastUpdate, collisionInfo1.objectStateTime, collisionInfo2.objectStateTime,
							boundingPoly1, object1.get(), boundingPoly2, object2.get());

						if (object1->canBeWentThrough(object2.get()) || object2->canBeWentThrough(object1.get())) {
							// update the object's state to the state
							updateTranformForCollision(object1.get(), collisionInfo1.objectStateTime, t);
							updateTranformForCollision(object2.get(), collisionInfo2.objectStateTime, t);

							collideTimeRange.first = std::min(collisionInfo1.collisionTimeRange.first, collisionInfo1.objectStateTime);
						}
						else {
							// update to new frame at collide time
							updateTranformForCollision(object1.get(), collideTimeRange.second, t);
							updateTranformForCollision(object2.get(), collideTimeRange.second, t);

							collisionInfo1.objectStateTime = collideTimeRange.second;
							collisionInfo2.objectStateTime = collideTimeRange.second;
						}
						// update the buffer bounding polygon for lastest state of the object
						object1->getBoundingPoly(boundingPoly1);
						object2->getBoundingPoly(boundingPoly2);
						// update time when the dynamic object cannot move anymore because of current collision
						collisionInfo1.collisionTimeRange = collideTimeRange;
						collisionInfo2.collisionTimeRange = collideTimeRange;
					}
					else {
						auto pCollisionOfDynamic = &collisionInfo1;
						auto pCollisionOfStatic = &collisionInfo2;
						if (object1->isStaticObject()) {
							pCollisionOfDynamic = &collisionInfo2;
							pCollisionOfStatic = &collisionInfo1;
						}

						pCollisionOfDynamic->object->update(_lastUpdate);
						// ensure the tranformation is same as previous frame
						pCollisionOfDynamic->object->setTransformation(pCollisionOfDynamic->previousMat);

						auto collideTimeRange = findCollideTime(_lastUpdate, pCollisionOfDynamic->objectStateTime,
							*pCollisionOfStatic->boundingPoly, *pCollisionOfDynamic->boundingPoly, pCollisionOfDynamic->object.get());

						if (pCollisionOfStatic->object->canBeWentThrough() || pCollisionOfDynamic->object->canBeWentThrough()) {
							// update the object's state to the state
							updateTranformForCollision(pCollisionOfDynamic->object.get(), pCollisionOfDynamic->objectStateTime, t);

							collideTimeRange.first = std::min(pCollisionOfDynamic->collisionTimeRange.first, pCollisionOfDynamic->objectStateTime);
						}
						else {
							// update to new frame at collide time
							updateTranformForCollision(pCollisionOfDynamic->object.get(), collideTimeRange.second, t);

							pCollisionOfDynamic->objectStateTime = collideTimeRange.second;
						}
						// update the buffer bounding polygon for lastest state of the object
						pCollisionOfDynamic->object->getBoundingPoly(*pCollisionOfDynamic->boundingPoly);
						// update time when the dynamic object cannot move anymore because of current collision
						pCollisionOfDynamic->collisionTimeRange = collideTimeRange;
					}
				}

				// set lastest collide object
				auto& collisions1 = collisionInfo1.collisions;
				if (collisions1 == nullptr) {
					collisions1 = make_shared<list<CollisionNode>>();
				}
				// add primary collision
				collisions1->push_back({ true, &collisionInfo2 });

				auto& collisions2 = collisionInfo2.collisions;
				if (collisions2 == nullptr) {
					collisions2 = make_shared<list<CollisionNode>>();
				}
				// add secondary collision
				collisions2->push_back({ false, &collisionInfo1 });
			}
		}
	}

	// recheck and remove out-of-date collision
	for (auto it = collisionCheckList.begin(); it != collisionCheckList.end(); it++) {
		auto& collisions = it->collisions;

		if (collisions) {
			auto& object = it->object;
			auto& dynamicBound = *it->boundingPoly;

			// recheck and remove out-of-date collision
			for (auto kt = collisions->begin(); kt != collisions->end();) {
				auto& node = *kt;
				if (node.isMain) {
					auto& colliedObject = node.pCollisionInfo->object;
					if (checkCollision2d(dynamicBound, *(node.pCollisionInfo->boundingPoly)) < 0) {
						auto ktTemp = kt;
						kt++;
						collisions->erase(ktTemp);
					}
					else {
						kt++;
					}
				}
				else {
					kt++;
				}
			}
		}
	}

	// update the transforms to the very last time before collision occur
	for (auto it = collisionCheckList.begin(); it != collisionCheckList.end(); it++) {
		auto& collisions = it->collisions;

		if (collisions) {
			auto& object = it->object;
			auto& dynamicBound = *it->boundingPoly;

			object->update(_lastUpdate);
			// ensure the tranformation is same as previous frame
			object->setTransformation(it->previousMat);

			updateTranformForCollision(object.get(), it->collisionTimeRange.first, t);
		}
	}

	// call collision event handler
	for (auto it = collisionCheckList.begin(); it != collisionCheckList.end(); it++) {
		auto& collisions = it->collisions;
		auto& object = it->object;
		if (collisions) {
			auto& collisionHandler1 = object->getCollisionHandler();
			if (collisionHandler1) {
				for (auto kt = collisions->begin(); kt != collisions->end(); kt++) {
					auto& node = *kt;
					if (node.isMain) {
						auto& colliedObject = node.pCollisionInfo->object;
						auto& collisionHandler2 = colliedObject->getCollisionHandler();
						collisionHandler1(colliedObject, it->collisionTimeRange.first);
						if (collisionHandler2) {
							collisionHandler2(object, it->collisionTimeRange.first);
						}
					}
				}
			}
			else {
				for (auto kt = collisions->begin(); kt != collisions->end(); kt++) {
					auto& node = *kt;
					if (node.isMain) {
						auto& colliedObject = node.pCollisionInfo->object;
						auto& collisionHandler2 = colliedObject->getCollisionHandler();
						if (collisionHandler2) {
							collisionHandler2(object, it->collisionTimeRange.first);
						}
					}
				}
			}
		}
	}

	// update the last time update
	// must be executed before exit this function
	_lastUpdate = t;
}