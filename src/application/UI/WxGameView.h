#pragma once
#include "CiWidget.h"
#include <memory>
#include "../Engine/Scene.h"
#include "cinder/app/Renderer.h"

class WxGameView : public CiWidget
{
protected:
	ci::Area _viewPort;
	ci::Area _radarViewPort;
	float _expectedRatio;
	std::shared_ptr<Scene> _gameScene;
	ci::app::WindowRef _parent;

protected:
	bool updateViewPort();
public:
	WxGameView(ci::app::WindowRef parent);
	virtual ~WxGameView();

	void update();
	void draw();

	void setSize(float w, float h);
	void setPos(float x, float y);

	void setScene(std::shared_ptr<Scene> gameScene);
	// scene view ratio width per height
	// default is 1:1
	void setSceneViewRatio(float ratio);
	float getSenceViewRatio() const;
};

