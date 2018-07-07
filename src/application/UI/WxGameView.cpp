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

	if (_tankViews[0]) {
		_tankViews[0]->setPos((float)(_viewPort.x2 + padding), (float)padding);
		_tankViews[0]->setSize((float)radaW, (float)radaH);
	}
	if (_tankViews[1]) {
		_tankViews[1]->setPos((float)(_viewPort.x2 + padding), (float)(padding * 2 + radaH));
		_tankViews[1]->setSize((float)radaW, (float)radaH);
	}

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

void WxGameView::setTankView(const std::shared_ptr<Widget>& tankView) {
	if (_tankViews[0]) {
		_tankViews[1] = tankView;
	}
	else {
		_tankViews[0] = tankView;
	}
}

void WxGameView::update() {
	if (_tankViews[0]) {
		_tankViews[0]->update();
	}
	if (_tankViews[1]) {
		_tankViews[1]->update();
	}
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
	if (_tankViews[0]) {
		_tankViews[0]->draw();
	}
	if (_tankViews[1]) {
		_tankViews[1]->draw();
	}

	{
		constexpr float measureDuration = 3.0f;
		auto currentTime = ci::app::getElapsedSeconds();
		auto frameCount = ci::app::getElapsedFrames();

		auto fs = frameCount / currentTime;
		auto fsStr = std::to_string((int)fs);
		fsStr.append(" fps");
		gl::drawString(fsStr, _viewPort.getUL());
	}
}
