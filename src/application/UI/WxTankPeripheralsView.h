#pragma once
#include "CiWidget.h"
#include <memory>
#include "../Engine/Scene.h"
#include "cinder/app/Renderer.h"
#include "WxRadarView.h"
#include "battle/TankCamera.h"
#include "../EngineSpecific/Tank.h"

class WxTankPeripheralsView : public CiWidget
{
protected:
	ci::app::WindowRef _parent;
	std::shared_ptr<WxRadarView> _radarView;
	std::shared_ptr<TankCamera> _tankCamera;
	std::shared_ptr<Tank> _tank;
	ci::gl::FboRef _fboScene;
	ci::gl::GlslProgRef	_glslFboToScreen;
public:
	WxTankPeripheralsView(ci::app::WindowRef parent);
	virtual ~WxTankPeripheralsView();

	void setupPeripherals(const std::shared_ptr<Tank>& tank);
	void update();
	void draw();
	void setSize(float w, float h);
	void setPos(float x, float y);
};

