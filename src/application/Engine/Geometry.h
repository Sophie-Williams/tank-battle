#pragma once
#include <vector>
#include <algorithm>
#include <cmath>

#define GEOMETRY_EPSILON 0.000001
#define MIN_POINT_DISTANCE 5.0f

template <class T>
bool Intersect2D_Lines(
	const T& P, const T& u,
	const T& Q, const T& v,
	float* t1, float* t2)
{
	double D = (float)( ((double)v.x)*u.y - ((double)u.x)*v.y);

	//lines are parallel
	if (std::abs(D) < GEOMETRY_EPSILON) return false;

	auto A = Q.x - P.x;
	auto B = Q.y - P.y;

	if (t1) {
		*t1 = (float)((v.x * B - v.y * A) / D);
	}
	if (t2) {
		*t2 = (float)((u.x * B - u.y * A) / D);
	}
	return true;
}

template <class T>
inline float lengthSquared(const T& u)
{
	return (float)(u.x*u.x + u.y*u.y);
}

template <class T>
inline double lengthSquaredd(const T& u)
{
	double res = (double)u.x;
	res *= u.x;
	res += ((double)u.y)*u.y;
	return res;
}

template <class T>
double cosAngle(const T& u, const T& v)
{
	auto dotProduct = (double)u.x;
	dotProduct *= v.x;
	dotProduct += ((double)u.y)*v.y;

	auto l1l2 = lengthSquaredd(u);
	l1l2 *= lengthSquaredd(v);
	auto cosValue = dotProduct / sqrt(l1l2);

	if (cosValue > 1.0) {
		cosValue = 1.0;
	}
	else if (cosValue < -1.0) {
		cosValue = -1.0;
	}

	return cosValue;
}

template <class T1, class T2>
T2 cosAngle(const T1& u, const T1& v, const T2& l1, const T2& l2)
{
	// u.x*v.x + u.y*v.)
	auto dotProduct = (T2)u.x;
	dotProduct *= v.x;
	dotProduct += ((T2)u.y)*v.y;
	auto cosValue = dotProduct / (l1*l2);

	if (cosValue > 1.0f) {
		cosValue = 1.0f;
	}
	else if (cosValue < -1.0f) {
		cosValue = -1.0f;
	}

	return cosValue;
}

template <class T>
float distanceFromPoint(const T& P, const T& u, const T& Q) {
	float A, B, C;

	A = (float)-u.y;
	B = (float)u.x;
	C = -A * P.x - B * P.y;

	return std::abs(A * Q.x + B * Q.y + C) / sqrt(A*A + B*B);
}

// compute value of a point against a line equation
// line equation is established by a point and a direction vector
// the normal vector follow right hand rule
template <class T>
auto compute(const T& P, const T& u, const T& Q) {
	auto A = -u.y;
	auto B = u.x;
	auto C = -A * P.x - B * P.y;

	return A * Q.x + B * Q.y + C;
}

template <class T>
struct GeneralLine {
	T A;
	T B;
	T C;

	template <class Pt>
	void build(const Pt& P, const Pt& u) {
		A = -u.y;
		B = u.x;
		C = -A * P.x - B * P.y;
	}

	template <class Pt>
	auto compute(const Pt& Q) const {
		return A * Q.x + B * Q.y + C;
	}
};

template <class T>
inline bool checkPoint(const T& p, int width, int height) {
	return !(p.x < 0 || p.y < 0 || p.x > width || p.y > height);
}

template <class T, class Iterator>
void projectPoints(const T& p, const T& u, Iterator begin, Iterator end) {
	auto v = T(u.y, -u.x);
	float t;
	for (auto iter = begin; iter != end; iter++) {
		auto& x = *iter;
		Intersect2D_Lines(p, u, x, v, &t, nullptr);
		x = p + u * t;
	}
}

template <class T>
float projectPoint(const T& p, const T& u, const T& x) {
	T v(u.y, -u.x);
	float t;
	Intersect2D_Lines(p, u, x, v, &t, nullptr);
	return t;
}

template <class T>
bool IsMidleSegment1(
	const T& P, const T& A, const T& B
)
{
	T u = P - A;
	T v = P - B;
	float dotProduct = (float)(u.x*v.x + u.y*v.y);

	return dotProduct <= 0;
}

template <class T>
bool IsMidleSegment2(
	const T& P, const T& A, const T& B
)
{
	T u = P - A;
	T v = P - B;
	float dotProduct = (float)(u.x*v.x + u.y*v.y);

	return dotProduct < 0;
}

template <class T>
auto computeRank(
	const T& P, const T& A, const T& B
)
{
	T u = P - A;
	T v = P - B;
	return (u.x*v.x + u.y*v.y);
}

// compute ratio of vector v base on vector u
// vector u must be not a zero vector
template <class T>
auto computeT(
	const T&u, const T&v
)
{
	if (u.x) {
		return v.x / u.x;
	}
	return v.y / u.y;
}

/// offset a vector by a distance in direction of its normal vector which follow right hand rule
template <class T>
void offset(T& P, const T& u, float distance) {
	distance = (float)(distance / sqrt(lengthSquaredd(u)));

	//// normalize vector follow right hand rule has length is distance
	//T v(-u.y * distance, u.x * distance);
	//// caclulate new p
	//P = P + v;

	//optimize above statments
	P.x += -u.y * distance;
	P.y += u.x * distance;
}

template <class T>
char isPointInside(const std::vector<T>& poly, const T& Q) {
	float prevVal = 0;
	char sign = 1;
	int n = (int)poly.size();
	for (int j = 0; j < n; j++) {
		auto& P1 = poly[j];
		auto& P2 = poly[(j + 1) % n];
		auto u = P2 - P1;
		float currVal = compute(P1, u, Q);

		// check if the point lies on current edge
		// then the point is in the polygon
		if (currVal == 0) {
			if (IsMidleSegment1(Q, P1, P2)) {
				sign = 0;
			}
			else {
				sign = -1;
			}
			break;
		}

		// if the previous value and current value is opposite of sign
		// it means the point is out side of the polygon
		if (prevVal * currVal < 0) {
			sign = -1;
			break;
		}
		prevVal = currVal;
	}

	return sign;
}

// check if a point inside a polygon
// lies on an edge is not count
template <class T>
bool isPointInside2(const std::vector<T>& poly, const T& Q) {
	float prevVal = 0;
	int n = (int)poly.size();
	int j;
	for (j = 0; j < n; j++) {
		auto& P1 = poly[j];
		auto& P2 = poly[(j + 1) % n];
		auto u = P2 - P1;
		float currVal = compute(P1, u, Q);

		// check if the point lies on current edge
		if (currVal == 0) {
			break;
		}

		// if the previous value and current value is opposite of sign
		// it means the point is out side of the polygon
		if (prevVal * currVal < 0) {\
			break;
		}
		prevVal = currVal;
	}

	return j == n;
}

///////////////////////////////////////////////////////////////////////////////////
/// check if sign of a point over generalines is same
/// if the point lies on a line, the function return line index
/// if sign of the point is different, the function return a nagative number of 
/// index of the line that sign of the point on that line is different than sign of the point on preivous line
/// if sign of the point is same, the function return 0
///////////////////////////////////////////////////////////////////////////////////
template <class T, class Pt>
int checkPointSign(const std::vector<GeneralLine<T>>& cachedPoly, const Pt& Q) {
	float prevVal = 0;
	int n = (int)cachedPoly.size();
	for (int j = 0; j < n; j++) {
		auto& generalLine = cachedPoly[j];
		float currVal = generalLine.compute(Q);

		// check if the point lies on current edge
		// then the point is in the polygon
		if (currVal == 0) {
			return j;
		}

		// if the previous value and current value is opposite of sign
		// it means the point is out side of the polygon
		if (prevVal * currVal < 0) {
			return -j;
			break;
		}
		prevVal = currVal;
	}

	return 0;
}


template <class T>
auto triangleArea(const T& A, const T& B, const T& C) {
	auto u = C - B;
	auto t = projectPoint(B, u, A);
	auto projectedA = B + u * t;

	auto la2 = lengthSquaredd(u);
	auto lh2 = lengthSquaredd(projectedA - A);
	
	return sqrt(la2*lh2) / 2;
}

template <class T>
auto polyArea(const std::vector<T>& poly) {
	int n = (int)poly.size();
	double area = 0;
	auto& A = poly[j];
	for (int j = 1; j < n - 1; j++) {
		auto& B = poly[j];
		auto& C = poly[j + 1];

		area += triangleArea(A, B, C);
	}

	return area;
}

template <class T>
bool hasInside(const std::vector<T>& poly1, const std::vector<T>& poly2) {
	int n = (int)poly2.size();
	for (int i = 0; i < n; i++) {
		auto& P = poly2[i];
		if (isPointInside2(poly1, P)) {
			return true;
		}
	}
	return false;
}

// check if two poly is intersect
// return -1 if two poly is not intersect
// return 0 of they are just touch to each other
// return 1 if they have a common zone
template <class T>
int checkIntersect2d(const std::vector<T>& poly1, const std::vector<T>& poly2) {
	int m = (int)poly1.size();
	int n = (int)poly2.size();

	// count number of inside point
	if (hasInside(poly1, poly2)) {
		return 1;
	}
	if (hasInside(poly2, poly1)) {
		return 1;
	}

	bool isTouched = false;
	// check if any segment of two polygon intersect
	float t1, t2;
	for (int i = 0; i < m; i++) {
		auto& v11 = poly1[i];
		auto& v12 = poly1[(i + 1) % m];
		auto u1 = v12 - v11;


		for (int j = 0; j < n; j++) {
			auto& v21 = poly2[j];
			auto& v22 = poly2[(j + 1) % n];
			auto u2 = v22 - v21;

			// two segment must be intersect at the middle of each other
			if (Intersect2D_Lines(v11, u1, v21, u2, &t1, &t2)) {
				// intersect at middle of each line, don't count for intersection
				// at two construct points of the segment
				if ((t1 > 0.0f && t1 < 1.0f) && (t2 > 0.0f && t2 < 1.0f)) {
					// if two edges are intersect at middle, it means they have common area
					return 1;
				}
				else if ((t1 == 0.0f || t1 == 1.0f) && (t2 >= 0.0f && t2 <= 1.0f)) {
					isTouched = true;
				}
				else if ((t2 == 0.0f || t2 == 1.0f) && (t1 >= 0.0f && t1 <= 1.0f)) {
					isTouched = true;
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
							isTouched = true;
						}

						// check if v22 is in middle of segment 1
						t = computeT(u1, v22 - v11);
						if (t >= 0.0f && t <= 1.0f) {
							isTouched = true;
						}
					}

					// compute parameter t of v11, v12 on segment 2
					if (u2.x || u2.y) {
						// check if v11 is in middle of segment 2
						auto t = computeT(u2, v11 - v21);
						if (t >= 0.0f && t <= 1.0f) {
							isTouched = true;
						}

						// check if v12 is in middle of segment 2
						t = computeT(u2, v12 - v21);
						if (t >= 0.0f && t <= 1.0f) {
							isTouched = true;
						}
					}
				}
			}
		}
	}

	return isTouched ? 0 : -1;
}