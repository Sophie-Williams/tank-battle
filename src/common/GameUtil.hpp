#pragma once

#include "Geometry.h"

template <typename Iterator>
auto center(Iterator start, Iterator end) {
	auto it = start;
	auto P = *it;
	int n = 1;
	for (it++; it != end; it++)
	{
		P += *it;
		n++;
	}
	P /= (float)n;
	return P;
}

template <typename T>
bool isRayIntersect(const T&P, const T&u, T* polygon, int n) {
	if (n < 2) return false;
	int nSub1 = n - 1;

	float t1, t2;

	for (int i = 0; i < n; i++) {
		auto& Q = polygon[i];
		auto v = polygon[next(i, nSub1)] - Q;

		if (Intersect2D_Lines(P, u, Q, v, &t1, &t2) && t1 >= 0 && 1 >= t2 && t2 >= 0) {
			return true;
		}
	}

	return false;
}