#pragma once
#include "Engine/PlayerController.h"
#include "cinder/gl/gl.h"
#include "cinder/app/App.h"

class PlayerControllerUI : public PlayerController
{
	bool _keyLeftDown = false;
	bool _keyRightDown = false;
	bool _keyUpDown = false;
	bool _keyDownDown = false;
	bool _keyZDown = false;
	bool _keyXDown = false;
private:
	void onKeyUp(ci::app::KeyEvent& e);
	void onKeyDown(ci::app::KeyEvent& e);
public:
	PlayerControllerUI(std::shared_ptr<Tank> player, ci::app::WindowRef inputWindow);
	virtual ~PlayerControllerUI();
	virtual void makeAction(float t);
};