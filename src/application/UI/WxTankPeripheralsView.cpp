#include "WxTankPeripheralsView.h"

#include "cinder/gl/gl.h"
#include "cinder/app/App.h"
#include "Engine/Geometry.h"

using namespace ci;
using namespace std;

#define BLUR_SIZE 128

WxTankPeripheralsView::WxTankPeripheralsView(ci::app::WindowRef parent) :
	_parent(parent) {
}

WxTankPeripheralsView::~WxTankPeripheralsView(){}

void WxTankPeripheralsView::setupPeripherals(const std::shared_ptr<TankCamera>& tankCamera, const std::shared_ptr<Radar>& tankRadar) {
	_tankCamera = tankCamera;

	// setup radar view
	_radarView = make_shared<WxRadarView>(_parent);
	auto pApp = app::App::get();
	auto shaderBlur = gl::GlslProg::create(pApp->loadAsset("blur.vert"), pApp->loadAsset("blur.frag"));
	auto shaderRadar = gl::GlslProg::create(pApp->loadAsset("radar.vert"), pApp->loadAsset("radar.frag"));
	_radarView->setShaders(shaderBlur, shaderRadar);
	_radarView->setRadar(tankRadar);
}

void WxTankPeripheralsView::update() {
}

void WxTankPeripheralsView::setSize(float w, float h) {
	CiWidget::setSize(w, h);
	_radarView->setSize(w, h);
}

void WxTankPeripheralsView::setPos(float x, float y) {
	CiWidget::setPos(x, y);
	_radarView->setPos(x, y);
}

void WxTankPeripheralsView::draw() {
	if (_radarView) {
		_radarView->draw();
	}
	if (_tankCamera) {
		auto h = _parent->getHeight();
		// mapping scene to view port area
		{
			gl::ScopedViewport scopeViewPort(getX(), h - getY() - getHeight(), getWidth(), getHeight());
			_tankCamera->draw();
		}

		auto pTank = dynamic_cast<Tank*>( _tankCamera->getOwner().get());
		if (pTank) {
			auto gunSegment = pTank->getGun();
			gl::ScopedColor lineColorScope(1, 1, 1);
			constexpr float demonstrateGunLength = 20;
			
			vec2 gunStart(gunSegment.x, gunSegment.y);
			vec2 gunDir(gunSegment.z - gunStart.x, gunSegment.w - gunStart.y);
			
			Rectf destRect(getX(), getY(), getX() + getWidth(), getY() + getHeight());
			auto center = destRect.getCenter();
			gunStart += center;
			gunDir.x *= 10;
			gunDir.y *= -10;

			auto gunEnd = gunStart + gunDir;
			gl::drawLine(gunStart, gunEnd);
		}
	}
}