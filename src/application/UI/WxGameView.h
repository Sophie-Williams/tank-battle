#pragma once
#include "CiWidget.h"
#include <memory>
#include "../Engine/Scene.h"
#include "cinder/app/Renderer.h"

class WxGameView : public CiWidget
{
protected:
	ci::Area _viewPort;
	float _expectedRatio;
	std::shared_ptr<Scene> _gameScene;
	ci::app::WindowRef _parent;
	std::shared_ptr<Widget> _tankView;
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
	void setTankView(const std::shared_ptr<Widget>& tankView);
	// scene view ratio width per height
	// default is 1:1
	void setSceneViewRatio(float ratio);
	float getSenceViewRatio() const;
};

