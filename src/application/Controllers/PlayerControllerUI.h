#pragma once
#include "Engine/GameComponent.h"
#include "cinder/gl/gl.h"
#include "cinder/app/App.h"

class PlayerControllerUI : public GameComponent
{
	bool _keyLeftDown = false;
	bool _keyRightDown = false;
	bool _keyUpDown = false;
	bool _keyDownDown = false;
	bool _keyZDown = false;
	bool _keyXDown = false;
	ci::signals::Connection _keyDown;
	ci::signals::Connection _keyUp;
private:
	void onKeyUp(ci::app::KeyEvent& e);
	void onKeyDown(ci::app::KeyEvent& e);
public:
	PlayerControllerUI(ci::app::WindowRef inputWindow);
	virtual ~PlayerControllerUI();
	virtual void makeAction(float t);
	virtual void update(float t);
};