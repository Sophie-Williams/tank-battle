#include "PlayerControllerTest.h"
#include "Engine/Tank.h"
#include "cinder/Rand.h"
using namespace ci;
std::mersenne_twister_engine<unsigned int, 32, 624, 397, 31, 2567483615, 11, 4294967295, 7, 2636928640, 15, 4022730752, 18, 1812433253> cinder::Rand::sBase;
std::uniform_real_distribution<float> cinder::Rand::sFloatGen;

PlayerControllerTest::PlayerControllerTest() : _lastMoveAt(-1), _lastFireAt(-1), _lifeBeginAt(-1) {
	_fireLimitDuration = 2.7f;
	_moveLimitDuration = 2;
}

PlayerControllerTest::~PlayerControllerTest() {}

void PlayerControllerTest::randomStrategy(float t) {
	auto pTank = dynamic_cast<Tank*>(_owner);
	if (pTank == nullptr) return;

	static char singleActions[] = { 1, -1, 1, -1, 1, -1 };
	//static char singleActions[] = { 1, -1 };
	static Rand radomizer;
	static bool isSeeded = false;
	if (!isSeeded) {
		radomizer.seed((uint32_t)(t * 100000));
		isSeeded = true;
	}

	if (_lifeBeginAt < 0) {
		_lifeBeginAt = t;

		_fireLimitDuration = radomizer.randFloat(1.5f, 3.0f);
		_moveLimitDuration = radomizer.randFloat(1.5f, 4.0f);
		return;
	}

	auto delta = t - _lifeBeginAt;
	if (delta < 3.0f) {
		return;
	}

	if (_lastMoveAt < 0 || (t - _lastMoveAt) >= _moveLimitDuration) {
		_lastMoveAt = t;
		int i = radomizer.nextInt(0, sizeof(singleActions));
		if (i < 2) {
			pTank->turn(0, t);
			pTank->spinBarrel(0, t);
			pTank->move(singleActions[i], t);
		}
		else if (i < 4) {
			pTank->turn(singleActions[i], t);
			pTank->spinBarrel(0, t);
			pTank->move(0, t);
		}
		else {
			pTank->spinBarrel(singleActions[i], t);
			pTank->turn(0, t);
			pTank->move(0, t);
		}
		// random moving time limit for the next time
		_moveLimitDuration = radomizer.randFloat(1.5f, 4.0f);
	}

	if (_lastFireAt < 0 || (t - _lastFireAt) >= _fireLimitDuration) {
		_lastFireAt = t;
		pTank->fire(t);
		// random firing time limit for the next time
		_fireLimitDuration = radomizer.randFloat(1.5f, 3.0f);
	}
}

void PlayerControllerTest::moveDownStrategy(float t) {
	auto pTank = dynamic_cast<Tank*>(_owner);
	if (pTank == nullptr) return;
	if (_lifeBeginAt < 0) {
		_lifeBeginAt = t;
		pTank->move(-1, t);
	}
}

void PlayerControllerTest::update(float t) {
	//randomStrategy(t);
	moveDownStrategy(t);
}