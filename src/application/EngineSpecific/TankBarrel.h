/******************************************************************
* File:        TankBarrel.h
* Description: declare TankBarrel class. A TankBarrel object
*              equivalient to a tank component in real life which
*              used to put a gun on.
* Author:      Vincent Pham
*
* Copyright (c) 2018 VincentPT.
** Distributed under the MIT License (http://opensource.org/licenses/MIT)
**
*
**********************************************************************/

#pragma once
#include "engine/TexturedObject.h"
#include "engine/Animation.h"

class TankBarrel : public TexturedObject
{
protected:
	Animation _fireAnim;
public:
	TankBarrel();
	virtual ~TankBarrel();
	void fire(float t);
	void setBound(const ci::Rectf& boundRect);
protected:
	void updateInternal(float t);
	void drawInternal();
};