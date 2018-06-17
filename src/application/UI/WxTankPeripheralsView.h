#pragma once
#include "CiWidget.h"
#include <memory>
#include "../Engine/Scene.h"
#include "cinder/app/Renderer.h"
#include "WxRadarView.h"
#include "battle/TankCamera.h"

class WxTankPeripheralsView : public CiWidget
{
protected:
	ci::app::WindowRef _parent;
	std::shared_ptr<WxRadarView> _radarView;
	std::shared_ptr<TankCamera> _tankCamera;
	ci::gl::FboRef _fboScene;
	ci::gl::GlslProgRef	_glslFboToScreen;
public:
	WxTankPeripheralsView(ci::app::WindowRef parent);
	virtual ~WxTankPeripheralsView();

	void setupPeripherals(const std::shared_ptr<TankCamera>& tankCamera, const std::shared_ptr<Radar>& tankRadar);
	void update();
	void draw();
	void setSize(float w, float h);
	void setPos(float x, float y);
};

