/******************************************************************
* File:        Bullet.cpp
* Description: implement Bullet class. Instances of this class are
*              bullets in game. They can hurt a lived object.
* Author:      Vincent Pham
*
* Copyright (c) 2018 VincentPT.
** Distributed under the MIT License (http://opensource.org/licenses/MIT)
**
*
**********************************************************************/

#include "Bullet.h"
#include "engine/Animation.h"
#include "engine/Scene.h"
#include "engine/LifeTimeControlComponent.h"
#include "engine/GameEngine.h"
#include "engine/GameResource.h"

using namespace ci;

Bullet::Bullet(float t) : _lastUpdate(t), _movingSpeed(100), _damaged(20) {
	setTexture(GameResource::getInstance()->getTexture(TEX_ID_BULLET));
	setSize(8, 14);
	allowGoThrough(true);
	setObjectStaticFlag(false);
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
	translate(v);
}

void Bullet::drawInternal() {
	gl::ScopedColor bulletColor(0.0f, 1.0f, 0, 1);
	TexturedObject::drawInternal();
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

void Bullet::destroy(float t) {
	if (isAvailable() == false) return;

	TexturedObject::destroy(t);
	auto explosion = std::make_shared<Animation>();
	Scene::getCurrentScene()->addDrawbleObject(explosion);;
	
	// setup animation
	explosion->setTexture(GameResource::getInstance()->getTexture(TEX_ID_EXPLOSION));
	explosion->setFrameSize(ivec2(128, 128));
	// 30 frames per second
	explosion->setDisplayFrameDuration(1.0f/60);
	explosion->autoCalculateFrameCount();

	//
	auto& bulletBound = getBound();
	auto center = bulletBound.getCenter();

	glm::mat4::col_type p4(center, 0, 1);
	p4 = _tMat * p4;

	float w = bulletBound.getWidth() * 24;
	explosion->setBound(ci::Rectf(p4.x - w/2, p4.y - w * 3 / 4, p4.x + w / 2, p4.y + w / 4));

	// ensure that the explosion animation destroy automatically after it run to end
	auto animDuration = explosion->getDisplayFrameDuration() * explosion->getFrameCount();
	auto animLifeTimeControl = std::make_shared<LifeTimeControlComponent>(animDuration + 0.5f);
	animLifeTimeControl->startLifeTimeCountDown(t);
	explosion->addComponent(animLifeTimeControl);

	explosion->start(t);
}

bool Bullet::canBeWentThrough(DrawableObject* other) const {
	if (DrawableObject::canBeWentThrough(other)) {
		return true;
	}

	if (dynamic_cast<Bullet*>(other) != nullptr) {
		return true;
	}

	return (_owner.get() == other);
}