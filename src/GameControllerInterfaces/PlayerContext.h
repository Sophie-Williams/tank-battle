/******************************************************************
* File:        PlayerContext.h
* Description: declare TankPlayerContext class. This is an abstract
*              class providing information of a tanks object to
*              plugged-in libraries.
* Author:      Vincent Pham
*
* Copyright (c) 2018 VincentPT.
** Distributed under the MIT License (http://opensource.org/licenses/MIT)
**
*
**********************************************************************/

#pragma once
#include "GameControllerInterfaces.h"

typedef RawArray<RawObject> SnapshotObjectPoints;
typedef RawArray<RawTimeObject> SnapshotTimeObjectPoints;
typedef RawArray<ColissionRawInfo> SnapshotColissions;

class GAME_CONTROLLER_INTERFACE TankPlayerContext {
public:
	TankPlayerContext();
	virtual ~TankPlayerContext();

	virtual const SnapshotTimeObjectPoints* getRadarSnapshot() const = 0;
	virtual const SnapshotObjectPoints* getCameraSnapshot() const = 0;
	virtual const SnapshotColissions* getCollisionsAtThisTurn() const = 0;
	virtual RawRay getMyGun() const = 0;
	virtual float getMyHealth() const = 0;
	virtual GeometryInfo getMyGeometry() const = 0;
	virtual float getMovingSpeed() const = 0;
	virtual float getTurningSpeed() const = 0;

	virtual TankOperations getCurrentOperations() const = 0;

	virtual bool isAlly(GameObjectId id) const = 0;
	virtual bool isEnemy(GameObjectId id) const = 0;
};