#pragma once
#include "CiWidget.h"
#include <memory>
#include "../Engine/Scene.h"
#include "cinder/app/Renderer.h"

class WxRadarView : public CiWidget
{
protected:
	ci::app::WindowRef _parent;
	ci::gl::GlslProgRef	_glslBlurShader;
	ci::gl::GlslProgRef	_glslFboToScreen;
	ci::gl::FboRef _fboScene;
	ci::gl::FboRef _fboBlur1;
	ci::gl::FboRef _fboBlur2;

	void renderScene();
public:
	WxRadarView(ci::app::WindowRef parent);
	virtual ~WxRadarView();

	void update();
	void draw();
	void setSize(float w, float h);
	void setShaders(ci::gl::GlslProgRef	glslBlurShader, ci::gl::GlslProgRef	glslSceneToFbo);
};

