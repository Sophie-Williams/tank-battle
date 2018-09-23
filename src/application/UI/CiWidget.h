/******************************************************************
* File:        CiWidget.h
* Description: declare CiWidget class. This a base class of UI
*              controlls that use Cinder to render.
* Author:      Vincent Pham
*
* Copyright (c) 2018 VincentPT.
** Distributed under the MIT License (http://opensource.org/licenses/MIT)
**
*
**********************************************************************/

#pragma once
#include "Widget.h"
#include "cinder/gl/gl.h"

class CiWidget : public Widget
{
protected:
	glm::vec2 _window_pos;
	glm::vec2 _window_size;
public:
	CiWidget();
	virtual ~CiWidget();

	virtual void update();
	virtual void draw();
	virtual void setSize(float w, float h);
	virtual void setPos(float x, float y);
	float getWidth();
	float getHeight();
	float getX();
	float getY();
};

