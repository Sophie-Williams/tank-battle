#include "LiveObject.h"

LiveObject::LiveObject() {
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