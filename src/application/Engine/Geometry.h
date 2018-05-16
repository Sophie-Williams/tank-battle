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

	if (t1) {
		*t1 = (float)((v.x*(Q.y - P.y) - v.y*(Q.x - P.x)) / D);
	}
	if (t2) {
		*t2 = (float)((u.x*(Q.y - P.y) - u.y*(Q.x - P.x)) / D);
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
	auto v = T(u.y, -u.x);
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
bool isPointInside(const std::vector<T>& poly, const T& Q) {
	float prevVal = 0;
	char sign = 1;
	for (int j = 0; j < (int)poly.size(); j++) {
		auto& P1 = poly[j];
		auto& P2 = poly[(j + 1) % poly.size()];
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

	return (sign >= 0);
}