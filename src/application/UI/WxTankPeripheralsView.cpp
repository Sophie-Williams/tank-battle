/******************************************************************
* File:        WxTankPeripheralsView.cpp
* Description: implement WxTankPeripheralsView class. This class is
*              designed to show tank's peripheral views. Currently
*              it mixes radar view and camera view in the same area.
* Author:      Vincent Pham
*
* Copyright (c) 2018 VincentPT.
** Distributed under the MIT License (http://opensource.org/licenses/MIT)
**
*
**********************************************************************/

#include "WxTankPeripheralsView.h"

#include "cinder/gl/gl.h"
#include "cinder/app/App.h"
#include "../common/Geometry.h"

using namespace ci;
using namespace std;

#define BLUR_SIZE 128

WxTankPeripheralsView::WxTankPeripheralsView(ci::app::WindowRef parent) :
	_parent(parent) {
	_radarView = make_shared<WxRadarView>(_parent);
	auto pApp = app::App::get();
	auto shaderBlur = gl::GlslProg::create(pApp->loadAsset("blur.vert"), pApp->loadAsset("blur.frag"));
	_glslFboToScreen = gl::GlslProg::create(pApp->loadAsset("radar.vert"), pApp->loadAsset("radar.frag"));
	_radarView->setShader(shaderBlur);
}

WxTankPeripheralsView::~WxTankPeripheralsView(){}

void WxTankPeripheralsView::setupPeripherals(const std::shared_ptr<Tank>& tank) {
	auto& components = tank->getComponents();
	for (auto it = components.begin(); it != components.end(); it++) {
		auto tankCamera = dynamic_pointer_cast<TankCamera>(*it);
		if (tankCamera) {
			_tankCamera = tankCamera;
		}
		else {
			auto tankRadar = dynamic_pointer_cast<Radar>(*it);
			if (tankRadar) {
				_radarView->setRadar(tankRadar);
			}
		}
	}

	_tank = tank;
}

void WxTankPeripheralsView::update() {
}

void WxTankPeripheralsView::setSize(float w, float h) {
	CiWidget::setSize(w, h);
	_radarView->setSize(w, h);

	gl::Fbo::Format fmt;
	fmt.setSamples(16);
	_fboScene = gl::Fbo::create((int)w, (int)h, fmt);
}

void WxTankPeripheralsView::setPos(float x, float y) {
	CiWidget::setPos(x, y);
	_radarView->setPos(x, y);
}

void WxTankPeripheralsView::draw() {
	Rectf destRect(getX(), getY(), getX() + getWidth(), getY() + getHeight());

	{
		// draw view's to frame buffer
		{
			// bind frame buffer to current context
			gl::ScopedFramebuffer fbo(_fboScene);
			gl::ScopedViewport    viewport(0, 0, _fboScene->getWidth(), _fboScene->getHeight());
			// draw radar view
			if (_radarView) {
				_radarView->draw();
			}
			// draw camera view
			if (_tankCamera) {
				_tankCamera->draw();
			}
		}

		// draw frame buffer scene to screen using blur fragment shader
		if(_glslFboToScreen)
		{
			gl::ScopedTextureBind tex0(_fboScene->getColorTexture(), (uint8_t)0);
			gl::ScopedGlslProg shader(_glslFboToScreen);
			_glslFboToScreen->uniform("uTex0", 0);
			gl::drawSolidRect(destRect);
		}
	}

	
	// draw object's view coordinate system
	{
		gl::ScopedColor lineColorScope(0.2f, 0.2f, 0.3f);
		gl::drawLine((destRect.getUpperLeft() + destRect.getUpperRight()) / 2.0f, (destRect.getLowerLeft() + destRect.getLowerRight()) / 2.0f);
		gl::drawLine((destRect.getUpperLeft() + destRect.getLowerLeft()) / 2.0f, (destRect.getUpperRight() + destRect.getLowerRight()) / 2.0f);
	}

	// draw radar ray scan
	{
		auto& radarRef = _radarView->getRadar();
		if (radarRef) {
			gl::ScopedColor lineColorScope(1.f, 1.f, 0.f);
			auto radarCenter = destRect.getCenter();
			auto ray = radarRef->getRay();
			auto range = radarRef->getRange();

			auto sx = destRect.getWidth() / (2 * range);
			auto sy = destRect.getHeight() / (2 * range);
			ray.x *= sx;
			ray.y *= -sy;
			ray.x += radarCenter.x;
			ray.y += radarCenter.y;

			gl::drawLine(radarCenter, ray);
		}
	}

	// draw tank's gun
	if (_tankCamera) {
		if (_tank) {
			auto gunSegment = _tank->getGun();
			gl::ScopedColor lineColorScope(1, 1, 1);

			vec2 gunStart(gunSegment.x, gunSegment.y);
			vec2 gunDir(gunSegment.z - gunStart.x, gunSegment.w - gunStart.y);

			auto center = destRect.getCenter();
			gunStart += center;
			gunDir.x *= 10;
			gunDir.y *= -10;

			auto gunEnd = gunStart + gunDir;
			gl::drawLine(gunStart, gunEnd);
		}
	}
}
