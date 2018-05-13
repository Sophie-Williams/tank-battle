#pragma once
#include <list>
#include <cinder/gl/gl.h>
#include "GameObject.h"

class Scene
{
protected:
	std::list<GameObjectRef> _gameObjects;
	ci::ColorA8u _backGround;
	ci::gl::Texture2dRef _backGroundTex;
	ci::Rectf _sceneArea;
	Scene(const ci::Rectf& area);
public:
	virtual ~Scene();

	void update(float t);
	void draw();

	void setBackgroundColor(const ci::ColorA8u& _backGround);
	const ci::ColorA8u& getBackgroundColor()const;

	void addGameObject(GameObjectRef gameObjectRef);

	// current, the engine is allow create only one scene during game time
	static Scene* createScene(const ci::Rectf& area);
	static Scene* getCurrentScene();
};

