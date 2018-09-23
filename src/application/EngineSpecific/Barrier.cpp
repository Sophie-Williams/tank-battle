/******************************************************************
* File:        Barrier.cpp
* Description: implement Barrier class. Instances of this class are
*              barriers in game. A normal game object cannot go
*              through them.
* Author:      Vincent Pham
*
* Copyright (c) 2018 VincentPT.
** Distributed under the MIT License (http://opensource.org/licenses/MIT)
**
*
**********************************************************************/

#include "Barrier.h"
using namespace ci;

Barrier::Barrier() {
	allowGoThrough(false);
}

Barrier::~Barrier() {}

void Barrier::updateInternal(float t) {
}

void Barrier::drawInternal() {
	//gl::ScopedColor foreGroundColor(1, 0, 0);
	//gl::drawStrokedRect(_boundRect);
}