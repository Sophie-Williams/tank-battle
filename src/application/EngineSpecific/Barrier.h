/******************************************************************
* File:        Barrier.h
* Description: declare Barrier class. Instances of this class are 
*              barriers in game. A normal game object cannot go
*              through them.
* Author:      Vincent Pham
*
* Copyright (c) 2018 VincentPT.
** Distributed under the MIT License (http://opensource.org/licenses/MIT)
**
*
**********************************************************************/

#pragma once
#include "engine/DrawableObject.h"

class Barrier : public DrawableObject
{
public:
	Barrier();
	virtual ~Barrier();

protected:
	virtual void updateInternal(float t);
	virtual void drawInternal();
};