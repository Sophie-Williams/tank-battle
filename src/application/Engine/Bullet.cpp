#include "Bullet.h"
using namespace ci;

Bullet::Bullet(float t) : _lastUpdate(t), _movingSpeed(100), _damaged(20) {
	setTexture("E:\\Projects\\tank-battle\\src\\application\\assets\\bulletBlue1_outline.png");
	setSize(8, 14);
}

Bullet::~Bullet() {}

void Bullet::setSpeed(float speed) {
	_movingSpeed = speed;
}

float Bullet::getSpeed() const {
	return _movingSpeed;
}

void Bullet::updateInternal(float t) {
	// moving direction is always y-axis in local object coordinate
	static const ci::vec3 movingDir(0, 1, 0);

	auto delta = t - _lastUpdate;
	_lastUpdate = t;

	// compute moving vector since last update time
	auto v = movingDir * _movingSpeed * delta;
	move(v);
}

void Bullet::setSize(const float& w, const float& h) {
	Rectf boundRect(-w / 2, -h / 2, w / 2, h / 2);
	setBound(boundRect);
}

void Bullet::setOwner(GameObjectRef owner) {
	_owner = owner;
}

GameObjectRef Bullet::getOwner() const {
	return _owner;
}

void Bullet::setDamage(float damage) {
	_damaged = damage;
}

float Bullet::getDamage() const {
	return _damaged;
}