/******************************************************************
* File:        Tank.h
* Description: declare Tank class. Instances of this class are
*              players in game.
* Author:      Vincent Pham
*
* Copyright (c) 2018 VincentPT.
** Distributed under the MIT License (http://opensource.org/licenses/MIT)
**
*
**********************************************************************/

#pragma once
#include "engine/LiveObject.h"
#include "cinder/gl/gl.h"
#include "TankBody.h"
#include "TankBarrel.h"
#include "TankCommandsBuilder.h"

class Tank : public LiveObject
{
public:
	enum TankState {
		Standing = 0,
		Moving = 1,
		Rotating = 2,
		BarrelRotating = 4,
	};

	enum Action
	{
		NoAction = 0,
		MoveForward = 0x2,
		MoveBackward = 0x3,
		TurnLeft = 0x4,
		TurnRight,
		TurnBarrelLeft,
		TurnBarrelRight,
		Fire,
	};
protected:
	TankBody _body;
	TankBarrel _barrel;
	ci::Colorf _color;

	int _groupId;

	float _rotateSpeed;
	float _movingSpeed;
	float _rotateBarrelSpeed;
	float _fireSpeed; // number of bullet can be fired in one minute

	float _lastRotatingAt;
	float _lastMovingAt;
	float _lastRotatingBarrelAt;
	float _lastFireTime;

	char _movingDir;
	char _rotateDir;
	char _rotateBarrelDir;

	ci::vec4 _cahedGun;
protected:
	virtual void setBound(const ci::Rectf& boundRect);
	virtual void drawInternal();
	virtual void updateInternal(float t);
	void setComponentTextures();
public:
	Tank();
	virtual ~Tank();

	virtual void setSize(const ci::vec2& size);
	void move(char direction, float at);
	void turn(char direction, float at);
	void spinBarrel(char direction, float at);
	void fire(float at);
	void setColor(const ci::Colorf& color);
	int getGroupId() const;
	void setGroupId(int groupId);
	float getMaxTurningSpeed() const;

	/// gun segment on tank's model view
	ci::vec4 getGun() const;
	TankOperations getCurrentOperations() const;
};