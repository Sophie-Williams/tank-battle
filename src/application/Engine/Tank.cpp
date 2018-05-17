#include "Tank.h"
#include "Bullet.h"
#include "Scene.h"
#include "GameEngine.h"
#include "GameController.h"
#include "LifeTimeControlComponent.h"

using namespace ci;

Tank::Tank() :
	_movingDir(0),
	_rotateDir(0),
	_rotateBarrelDir(0),
	_lastMovingAt(-1),
	_lastRotatingAt(-1),
	_lastRotatingBarrelAt(-1),
	_lastFireTime(-1)
{
	_movingSpeed = 10; // 10 metter per second
	_rotateSpeed = glm::pi<float>()/ 6; // 60 degree per second
	_fireSpeed = 60; // 60 bullets can be fired in one minute
	_rotateBarrelSpeed = glm::half_pi<float>();

	allowGoThrough(false);
	setComponentTextures();
}

Tank::~Tank() {}

void Tank::setBound(const ci::Rectf& boundRect) {
	DrawableObject::setBound(boundRect);
}

void Tank::setSize(const ci::vec2& size) {
	// the expected sizes base on its images
	static const ci::vec2 baseSize(76, 88);
	static const ci::vec2 baseBodySize(76, 72);
	static const ci::vec2 baseBarrelSize(16, 52);
	static const ci::vec2 baseBarrelPivotLocal(baseBarrelSize.x/2, 8);

	// caculate diffent ratio of base size and real size
	float scaleX = size.x / baseSize.x;
	float scaleY = size.y / baseSize.y;

	// construct tank's components coordinates base on real size
	ci::vec2 bodySize(baseBodySize.x*scaleX, baseBodySize.y*scaleY);
	ci::vec2 barrelSize(baseBarrelSize.x*scaleX, baseBarrelSize.y*scaleY);
	ci::vec2 barrelPivotLocal(baseBarrelPivotLocal.x*scaleX, baseBarrelPivotLocal.y*scaleY);

	ci::Rectf tankBound(-size.x/2, -size.y/2, size.x/2, size.y/2);
	ci::vec2 barrelPivot((tankBound.x1 + tankBound.x2) / 2, tankBound.y2 - barrelSize.y + barrelPivotLocal.y);

	// tank boundary
	setBound(tankBound);

	// body boundary
	ci::Rectf bodyBound(tankBound.x1, tankBound.y1, tankBound.x2, tankBound.y1 + bodySize.y);
	_body.setBound(bodyBound);

	// barrel boundary
	ci::Rectf barrelBound(barrelPivot.x - barrelSize.x/2, tankBound.y2 - barrelSize.y, barrelPivot.x + barrelSize.x / 2, tankBound.y2);
	_barrel.setBound(barrelBound);

	// set pivots
	setPivot(ci::vec3(bodyBound.getCenter(), 0));
	_barrel.setPivot(ci::vec3(barrelPivot, 0));
}

void Tank::updateInternal(float t) {
	// update tank components
	_body.update(t);
	_barrel.update(t);

	// update rotation
	if (_rotateDir) {
		auto delta = t - _lastRotatingAt;
		if (delta > 0) {
			rotate(_rotateDir * _rotateSpeed * delta);
			_lastRotatingAt = t;
		}
	}

	// update barrel rotation
	if (_rotateBarrelDir) {
		auto delta = t - _lastRotatingBarrelAt;
		if (delta > 0) {
			_barrel.rotate(_rotateBarrelDir * _rotateBarrelSpeed * delta);
			_lastRotatingBarrelAt = t;
		}
	}

	// update tank's position
	if (_movingDir) {
		auto delta = t - _lastMovingAt;
		if (delta > 0) {
			ci::vec3 movingDir(0, 1, 0);
			movingDir *= _movingDir*_movingSpeed*delta;
			DrawableObject::move(movingDir);
			_lastMovingAt = t;
		}
	}
}

void Tank::drawInternal() {
	_body.draw();
	_barrel.draw();
}

void Tank::setComponentTextures() {
	_body.setTexture("tankBody.png");
	_barrel.setTexture("tankBarrel.png");
}

void Tank::move(char direction, float at) {
	if (_movingDir == direction) return;

	_movingDir = direction;
	if (direction) {
		_lastMovingAt = at;
	}
}

void Tank::turn(char direction, float at) {
	if (_rotateDir == direction) return;

	_rotateDir = direction;
	if (direction) {
		_lastRotatingAt = at;
	}
}

void Tank::spinBarrel(char direction, float at) {
	if (_rotateBarrelDir == direction) return;

	_rotateBarrelDir = direction;
	if (direction) {
		_lastRotatingBarrelAt = at;
	}
}

void Tank::fire(float at) {
	if (_lastFireTime < 0 || (at - _lastFireTime) >= _fireSpeed/60) {
		auto currentScene = Scene::getCurrentScene();
		if (currentScene) {
			auto bullet = std::make_shared<Bullet>(at);
			currentScene->addDrawbleObject(bullet);

			// ensure that the bullet destroy automatically after 5 seconds
			auto bulletLifeTimeControl = std::make_shared<LifeTimeControlComponent>(1.0f);
			bulletLifeTimeControl->startLifeTimeCountDown(at);
			bullet->addComponent(bulletLifeTimeControl);

			auto onCollisionDetected = std::bind(&GameController::OnBulletCollisionDetected,
				GameController::getInstance(), bullet, std::placeholders::_1);
			GameEngine::getInstance()->registerCollisionDetection(bullet, onCollisionDetected);

			bullet->setOwner(currentScene->findObjectRef(this));

			auto& tankTransform = getTransformation();
			auto& barrelTransform = _barrel.getTransformation();

			// put bullet transformation at tank's barrel
			auto bulletTransform = tankTransform * barrelTransform;
			bullet->setTransformation(bulletTransform);

			auto& barelBound = _barrel.getBound(); 
			auto& pivot = _barrel.getPivot();
			// bullet out position is center of bottom edge of the bound rectangle
			auto bulletOutPosition = (barelBound.getLowerLeft() + barelBound.getLowerRight())/2.0f;

			// after put bullet transformation at tank's barrel
			// the bullet is put at pivot of barrel
			// now we move it to bullet out position by offset the transformation matrix by translation vector of
			// pivot and bullet out position
			auto v = ci::vec3(bulletOutPosition, 0) - pivot;
			bullet->move(v);

			// bullet speed is 5 times faster than tank's moving speed
			bullet->setSpeed(_movingSpeed * 5);

			// set bullet size, should be smaller than tank's gun, current is 0.75 compare to tank's gun
			// the width of tank's gun assume that is half of barrel's width
			auto& defaultBulletBound = bullet->getBound();
			auto bulletW = 0.75f * barelBound.getWidth()/2;
			auto bulletH = bulletW * defaultBulletBound.getHeight() / defaultBulletBound.getWidth();
			bullet->setSize(bulletW, bulletH);

			_barrel.fire(at);
			_lastFireTime = at;
		}
	}
}