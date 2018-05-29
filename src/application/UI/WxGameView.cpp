#include "WxGameView.h"

#include "cinder/gl/gl.h"
#include "cinder/app/App.h"

using namespace ci;
using namespace std;

WxGameView::WxGameView(ci::app::WindowRef parent) : _expectedRatio(1.0f), _parent(parent) {
}
WxGameView::~WxGameView(){}

bool WxGameView::updateViewPort() {
	auto& viewRatio = _expectedRatio;

	if (getWidth() == 0 || getHeight() == 0) {
		return false;
	}

	auto windowRatio = getWidth() * 1.0f / getHeight();

	int viewWidth, viewHeight;

	if (viewRatio < windowRatio) {
		viewHeight = (int) getHeight();
		viewWidth = (int)(viewHeight * viewRatio);
	}
	else {
		viewWidth = (int)getWidth();
		viewHeight = (int)(viewWidth / viewRatio);
	}
	//ci::ivec2 basePoint(getX() + getWidth() / 2, getY() + getHeight() / 2);
	ci::ivec2 basePoint(viewWidth / 2, getY() + getHeight() / 2);
	_viewPort.set(basePoint.x - viewWidth/2, basePoint.y - viewHeight/2, basePoint.x + viewWidth / 2, basePoint.y + viewHeight / 2);

	// the left space of the view is the space for displaying radar
	constexpr int padding = 10;
	int radaW = (int)getWidth() - _viewPort.getWidth() - padding * 2;
	int radaH = (int)getHeight() - padding * 2;
	// but the radar area is not exceed 300 for width and height
	int maxRadaWidth = 300;

	radaH = radaW = std::min({ radaH, radaW, maxRadaWidth });

	_radarViewPort.set(_viewPort.x2 + padding, padding, _viewPort.x2 + padding + radaW, padding + radaH);

	return true;
}

void WxGameView::setSize(float w, float h) {
	CiWidget::setSize(w, h);
	updateViewPort();
}

void WxGameView::setPos(float x, float y) {
	CiWidget::setPos(x, y);
	updateViewPort();
}

void WxGameView::setSceneViewRatio(float ratio) {
	_expectedRatio = ratio;
	updateViewPort();
}

float WxGameView::getSenceViewRatio() const {
	return _expectedRatio;
}

void WxGameView::setScene(std::shared_ptr<Scene> gameScene) {
	_gameScene = gameScene;
}

void WxGameView::update() {
}

void WxGameView::draw() {
	{
		auto h = _parent->getHeight();
		// mapping scene to view port area
		gl::ScopedViewport scopeViewPort(_viewPort.x1, h - _viewPort.y1 - _viewPort.getHeight(), _viewPort.getWidth(), _viewPort.getHeight());

		if (_gameScene) {
			_gameScene->draw();
		}
	}
	if (_radarViewPort.x1 < _radarViewPort.x2 - 20 && _radarViewPort.y1 < _radarViewPort.y2 - 20) {
		auto h = _parent->getHeight();
		gl::ScopedViewport scopeViewPort(_radarViewPort.x1, h - _radarViewPort.y1 - _radarViewPort.getHeight(), _radarViewPort.getWidth(), _radarViewPort.getHeight());

		CameraOrtho orthoCam;
		orthoCam.setOrtho(-100, 100, -100, 100, 1, -1);
		gl::ScopedProjectionMatrix scopeMatrices;
		gl::setProjectionMatrix(orthoCam.getProjectionMatrix());

		gl::ScopedColor radarBackgroundColorScope (0.4f, 0.4f, 0.4f, 1);
		gl::drawSolidCircle(vec2(0, 0), 100);
	}
	{
		static float lastStartCountTime = -1;
		static int frameCount = 0;
		// measure at least for last 3 seconds
		constexpr float measureDuration = 3.0f;
		auto currentTime = ci::app::getElapsedSeconds();

		if (lastStartCountTime == -1) {
			lastStartCountTime = currentTime;
			frameCount = 0;
		}

		frameCount++;

		auto fs = frameCount / (currentTime - lastStartCountTime);
		auto fsStr = std::to_string((int)fs);
		fsStr.append(" fps");
		gl::drawString(fsStr, _viewPort.getUL());

		if (currentTime - lastStartCountTime > measureDuration) {
			lastStartCountTime = currentTime;
			frameCount = 1;
		}
	}
}
