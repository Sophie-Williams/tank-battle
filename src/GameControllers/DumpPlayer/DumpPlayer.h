/******************************************************************
* File:        DumpPlayer.h
* Description: declare DumpPlayer class. This is tank controller
*              class.
* Author:      Vincent Pham
*
* Copyright (c) 2018 VincentPT.
** Distributed under the MIT License (http://opensource.org/licenses/MIT)
**
*
**********************************************************************/

#pragma once
#include "GameControllerInterfaces.h"
#include "TankCommandsBuilder.h"
#include "PlayerContext.h"
#include "TankController.h"

#define STEP_BUFFER_COUNT 250

class DumpPlayer : public TankController
{
	float _prevTime;
	GeometryInfo _prevGeometry;
	TankOperations _operation = TANK_NULL_OPERATION;
	TankCommandsBuilder _commandBuider;
	char _prevDesireMove = 0;
	char _prevDesireTurn = 0;
	int _turningStuckCount = 0;
	int _movingStuckCount = 0;
	float _beginTurningAngle = 0;
	int _hurtStepCount = STEP_BUFFER_COUNT;
	int _runAfterGetHurtCount = 0;
	bool _stuckWhileRun = false;
	bool _changeWay = false;
	int _rotationStuckCounters[2];
protected:
	int updateMovingStep(TankPlayerContext* player);
	int runAfterGetHurt(TankPlayerContext* player);
	int targetEnemyCheck(TankPlayerContext* player);
	int targetEnemyCheckByRadar(TankPlayerContext* player);
	int findEnemy(TankPlayerContext* player);
	TankOperations giveOperationsInternal(TankPlayerContext* player);
public:
	DumpPlayer();
	virtual ~DumpPlayer();
	void setup(TankPlayerContext* player);
	TankOperations giveOperations(TankPlayerContext* player);
};
