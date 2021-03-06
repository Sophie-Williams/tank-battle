/******************************************************************
* File:        TankBarrel.cpp
* Description: implement TankBarrel class. A TankBarrel object
*              equivalient to a tank component in real life which
*              used to put a gun on.
* Author:      Vincent Pham
*
* Copyright (c) 2018 VincentPT.
** Distributed under the MIT License (http://opensource.org/licenses/MIT)
**
*
**********************************************************************/

#include "TankBarrel.h"
#include "engine/GameResource.h"

using namespace ci;

TankBarrel::TankBarrel() {
}

TankBarrel::~TankBarrel() {}

void TankBarrel::updateInternal(float t) {
	_fireAnim.update(t);
}

void TankBarrel::drawInternal() {
	TexturedObject::drawInternal();
	_fireAnim.draw();
}

void TankBarrel::fire(float t) {
	// start fire animation
	_fireAnim.reset();
	_fireAnim.start(t);
}

void TankBarrel::setBound(const ci::Rectf& boundRect) {
	TexturedObject::setBound(boundRect);

	static const ci::vec2 baseShotSize(20, 25);
	ci::vec2 shotSize(boundRect.getWidth() * 20 / 16, 0);
	shotSize.y = shotSize.x * baseShotSize.y / baseShotSize.x;

	_fireAnim.setTexture(GameResource::getInstance()->getTexture(TEX_ID_TANKSHOT));
	_fireAnim.setDisplayFrameDuration(0.15f);

	// fire animation boundary
	ci::Rectf fireAnimBound((boundRect.x1 + boundRect.x2 - shotSize.x) / 2, boundRect.y2, (boundRect.x1 + boundRect.x2 + shotSize.x) / 2, boundRect.y2 + shotSize.y);
	_fireAnim.setBound(fireAnimBound);
}