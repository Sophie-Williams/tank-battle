/******************************************************************
* File:        TankBody.h
* Description: declare TankBody class. A TankBody object
*              equivalient to a tank body in real life.
* Author:      Vincent Pham
*
* Copyright (c) 2018 VincentPT.
** Distributed under the MIT License (http://opensource.org/licenses/MIT)
**
*
**********************************************************************/

#pragma once
#include "engine/TexturedObject.h"

class TankBody : public TexturedObject
{
public:
	TankBody();
	virtual ~TankBody();

protected:
	virtual void updateInternal(float t);
};