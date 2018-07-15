#include "LiveObject.h"

LiveObject::LiveObject() {
	_healthCap = 1;
	setHealth(1);
	setShield(0);
}

LiveObject::~LiveObject() {
}

void LiveObject::setHealth(float health) {
	_health = health;
	if (_health <= 0) {
		setAvailable(false);
		_health = 0;
	}

	if (_healthCap < health) {
		_healthCap = health;
	}
}

float LiveObject::getHealth() const {
	return _health;
}

void LiveObject::setShield(float shield) {
	_shield = shield;
}

float LiveObject::getShield() const {
	return _shield;
}

void LiveObject::takeDamage(float damage) {
	if(_shield <= 0) {
		setHealth(getHealth() - damage);
	}
	else if(_shield <= 1){
		setHealth(getHealth() - (damage - _shield));
	}
	else {
		setHealth(getHealth() - (damage / _shield));
	}
}

void LiveObject::drawHeathBar() {
	std::vector<ci::vec2> boundingPoly(4);
	// polygon of the object
	this->getBoundingPoly(boundingPoly);

	auto it = boundingPoly.begin();
	float xMin, xMax;
	float yMax;

	xMax = xMin = it->x;
	yMax = it->y;

	for (it++; it != boundingPoly.end(); it++) {
		if (xMax < it->x) {
			xMax = it->x;
		}
		if (xMin > it->x) {
			xMin = it->x;
		}
		if (yMax < it->y) {
			yMax = it->y;
		}
	}

	constexpr float heathBarWidth = 5;
	constexpr float heathBarHeight = 0.5;
	float x = (xMin + xMax - heathBarWidth) / 2;
	float y = yMax + 1.0f;
	ci::Rectf rect(x, y, x + heathBarWidth, y + heathBarHeight);
	ci::ColorA color(1.0f, 1.0f, 1.0f, 1.0f);
	{
		ci::gl::ScopedColor heathColorScope(color);
		ci::gl::drawStrokedRect(rect);
	}
	color.b = color.g = _health / _healthCap;
	rect.x2 = rect.x1 + heathBarWidth * color.b;
	{
		ci::gl::ScopedColor heathColorScope(color);
		ci::gl::drawSolidRect(rect);
	}
}