#include "PlayerControllerUI.h"
using namespace ci;

PlayerControllerUI::PlayerControllerUI(std::shared_ptr<Tank> player, ci::app::WindowRef inputWindow) : PlayerController(player) {
	inputWindow->getSignalKeyDown().connect(std::bind(&PlayerControllerUI::onKeyDown, this, std::placeholders::_1));
	inputWindow->getSignalKeyUp().connect(std::bind(&PlayerControllerUI::onKeyUp, this, std::placeholders::_1));
}

PlayerControllerUI::~PlayerControllerUI() {}

void PlayerControllerUI::onKeyDown(ci::app::KeyEvent& e) {
	float t = (float)ci::app::App::get()->getElapsedSeconds();
	if (e.getCode() == app::KeyEvent::KEY_DOWN) {
		_keyDownDown = true;
	}
	else if(e.getCode() == app::KeyEvent::KEY_UP) {
		_keyUpDown = true;
	}
	else if (e.getCode() == app::KeyEvent::KEY_LEFT) {
		_keyLeftDown = true;
	}
	else if (e.getCode() == app::KeyEvent::KEY_RIGHT) {
		_keyRightDown = true;
	}
	else if (e.getCode() == app::KeyEvent::KEY_z) {
		_keyZDown = true;
	}
	else if (e.getCode() == app::KeyEvent::KEY_x) {
		_keyXDown = true;
	}
	else if (e.getCode() == app::KeyEvent::KEY_SPACE) {
		_player->fire(t);
	}
	makeAction(t);
}

void PlayerControllerUI::onKeyUp(ci::app::KeyEvent& e) {
	float t = (float)ci::app::App::get()->getElapsedSeconds();

	if (e.getCode() == app::KeyEvent::KEY_DOWN) {
		_keyDownDown = false;
	}
	else if (e.getCode() == app::KeyEvent::KEY_UP) {
		_keyUpDown = false;
	}
	else if (e.getCode() == app::KeyEvent::KEY_LEFT) {
		_keyLeftDown = false;
	}
	else if (e.getCode() == app::KeyEvent::KEY_RIGHT) {
		_keyRightDown = false;
	}
	else if (e.getCode() == app::KeyEvent::KEY_z) {
		_keyZDown = false;
	}
	else if (e.getCode() == app::KeyEvent::KEY_x) {
		_keyXDown = false;
	}
	makeAction(t);
}


void PlayerControllerUI::makeAction(float t) {
	char movingDir = 0;
	char rotateDir = 0;
	char spinBarrelDir = 0;

	if (_keyDownDown) {
		movingDir--;
	}
	if (_keyUpDown) {
		movingDir++;
	}
	if (_keyLeftDown) {
		rotateDir++;
	}
	if (_keyRightDown) {
		rotateDir--;
	}
	if (_keyZDown) {
		spinBarrelDir++;
	}
	if (_keyXDown) {
		spinBarrelDir--;
	}

	_player->move(movingDir, t);
	_player->turn(rotateDir, t);
	_player->spinBarrel(spinBarrelDir, t);
}