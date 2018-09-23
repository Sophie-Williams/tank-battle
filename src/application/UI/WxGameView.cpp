/******************************************************************
* File:        WxGameView.cpp
* Description: implement WxGameView class. This class is responsible
*              for showing a game sence real time.
* Author:      Vincent Pham
*
* Copyright (c) 2018 VincentPT.
** Distributed under the MIT License (http://opensource.org/licenses/MIT)
**
*
**********************************************************************/

#include "WxGameView.h"
#include "Engine/GameEngine.h"

#include "cinder/gl/gl.h"
#include "cinder/app/App.h"

using namespace ci;
using namespace std;

WxGameView::WxGameView(ci::app::WindowRef parent) : _expectedRatio(1.0f), _parent(parent), _lastTime(-1){
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
	_displayRange = _viewPort;

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

		_displayRange.x2 = _tankViews[0]->getX() + _tankViews[0]->getWidth();
	}
	if (_tankViews[1]) {
		_tankViews[1]->setPos((float)(_viewPort.x2 + padding), (float)(padding * 2 + radaH));
		_tankViews[1]->setSize((float)radaW, (float)radaH);

		_displayRange.x2 = _tankViews[1]->getX() + _tankViews[1]->getWidth();
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
	GameEngine::getInstance()->accessEngineResource([this]() {
		if (_tankViews[0]) {
			_tankViews[0]->update();
		}
		if (_tankViews[1]) {
			_tankViews[1]->update();
		}
	});
}

void WxGameView::draw() {
	auto gameEngine = GameEngine::getInstance();
	gameEngine->accessEngineResource([this]() {
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
	});	

	// draw debug information
	{
		auto currentTime = ci::app::getElapsedSeconds();
		float fps = -1;
		if (_lastTime >= 0) {
			_frameCounter.next() = currentTime - _lastTime;

			int cap = _frameCounter.cap();
			float totalFrameTime = 0;
			for (int i = 0; i < cap; i++) {
				totalFrameTime += _frameCounter[i];
			}
			fps = cap / totalFrameTime;
		}
		std::string str;
		auto pos = ci::ivec2(_displayRange.x2, _displayRange.y1);

		str = "display fps: ";
		if (fps >= 0) {
			str += std::to_string((int)std::ceil(fps));
		}
		else {
			str += " N/A";
		}
		gl::drawString(str, pos);
		auto engineFps = gameEngine->getFramePerSecond(); 
		auto engineCPUUsage = gameEngine->getCPUUsage();

		pos.y += 15;
		str = "engine fps: ";
		if (engineFps >= 0) {
			str += std::to_string((int)(std::ceil(engineFps)));
		}
		else {
			str += "N/A";
		}
		gl::drawString(str, pos);

		pos.y += 15;
		str = "engine cpu usage: ";
		if (engineCPUUsage >= 0) {
			str += std::to_string((int)(engineCPUUsage * 100));
			str += " %";
		}
		else {
			str += "N/A";
		}
		gl::drawString(str, pos);
		
		int logicObject = (int)gameEngine->getObjects().size();
		int physicalObject = (int)_gameScene->getDrawableObjects().size();

		pos.y += 15;
		str = "logic objects: ";
		str += std::to_string(logicObject);
		gl::drawString(str, pos);

		pos.y += 15;
		str = "phyical objects: ";
		str += std::to_string(physicalObject);
		gl::drawString(str, pos);

		_lastTime = currentTime;
	}
}
