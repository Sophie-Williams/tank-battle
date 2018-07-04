#include "GameInterface.h"

static GameInterface* s_gameInstance = nullptr;

GameInterface::GameInterface() {
	s_gameInstance = this;
}

GameInterface::~GameInterface() {
	s_gameInstance = nullptr;
}

GameInterface* GameInterface::getInstance() {
	return s_gameInstance;
}

////////////////////////////////////////////////////////////////////////////////////////////////

GAME_CONTROLLER_INTERFACE RawPoint operator-(const RawPoint& P) {
	RawPoint X = { -P.x, -P.y };
	return X;
}

RawPoint operator+(const RawPoint& P, const RawPoint& Q) {
	RawPoint X = { P.x + Q.x, P.y + Q.y };
	return X;
}

const RawPoint& operator+=(RawPoint& P, const RawPoint& Q) {
	P.x += Q.x;
	P.y += Q.y;

	return P;
}

RawPoint operator-(const RawPoint& P, const RawPoint& Q) {
	RawPoint X = { P.x - Q.x, P.y - Q.y };
	return X;
}

const RawPoint& operator-=(RawPoint& P, const RawPoint& Q) {
	P.x -= Q.x;
	P.y -= Q.y;

	return P;
}

RawPoint operator*(const RawPoint& P, float k) {
	RawPoint X = { P.x * k, P.y * k };
	return X;
}

const RawPoint& operator*=(RawPoint& P, float k) {
	P.x *= k;
	P.y *= k;

	return P;
}

RawPoint operator/(const RawPoint& P, float k) {
	RawPoint X = { P.x / k, P.y / k };
	return X;
}

const RawPoint& operator/=(RawPoint& P, float k) {
	P.x /= k;
	P.y /= k;

	return P;
}

float operator*(const RawPoint& P, const RawPoint& Q) {
	return P.x * Q.y - P.y * Q.x;
}