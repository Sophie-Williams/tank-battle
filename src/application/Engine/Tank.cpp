#include "Tank.h"

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
	_rotateBarrelSpeed = glm::half_pi<float>();
}

Tank::~Tank() {}

void Tank::setBound(const ci::Rectf& boundRect) {
	GameObject::setBound(boundRect);
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
	setPivot(bodyBound.getCenter());
	_barrel.setPivot(barrelPivot);
}

void Tank::update(float t) {
	_body.update(t);
	_barrel.update(t);

	if (_rotateDir) {
		auto delta = t - _lastRotatingAt;
		if (delta > 0) {
			rotate(_rotateDir * _rotateSpeed * delta);
			_lastRotatingAt = t;
		}
	}
	if (_rotateBarrelDir) {
		auto delta = t - _lastRotatingBarrelAt;
		if (delta > 0) {
			_barrel.rotate(_rotateBarrelDir * _rotateBarrelSpeed * delta);
			_lastRotatingBarrelAt = t;
		}
	}
	if (_movingDir) {
		auto delta = t - _lastMovingAt;
		if (delta > 0) {
			ci::vec3 movingDir(0,1,0);
			movingDir *= _movingDir*_movingSpeed*delta;
			GameObject::move(movingDir);
			_lastMovingAt = t;
		}
	}
}

void Tank::drawInternal() {
	_body.draw();
	_barrel.draw();
}

void Tank::setComponentTexture(const std::string& bodyImage, const std::string& barrelImage) {
	_body.setTexture(bodyImage);
	_barrel.setTexture(barrelImage);
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
	_lastFireTime = at;
}