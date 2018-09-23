/******************************************************************
* File:        WxRadarView.h
* Description: declare WxRadarView class. This class is responsible
*              for showing view of a tank's radar.
* Author:      Vincent Pham
*
* Copyright (c) 2018 VincentPT.
** Distributed under the MIT License (http://opensource.org/licenses/MIT)
**
*
**********************************************************************/

#pragma once
#include "CiWidget.h"
#include <memory>
#include "../Engine/Scene.h"
#include "cinder/app/Renderer.h"
#include "battle/Radar.h"

class WxRadarView : public CiWidget
{
protected:
	ci::app::WindowRef _parent;
	ci::gl::GlslProgRef	_glslBlurShader;
	ci::gl::FboRef _fboScene;
	ci::gl::FboRef _fboBlur;
	std::shared_ptr<Radar> _radar;

	void renderScene();
public:
	WxRadarView(ci::app::WindowRef parent);
	virtual ~WxRadarView();

	void update();
	void draw();
	void setSize(float w, float h);
	void setShader(ci::gl::GlslProgRef	glslBlurShader);
	void setRadar(const std::shared_ptr<Radar> &radar);
	const std::shared_ptr<Radar> getRadar() const;
};

