#pragma once
#include <list>
#include <cinder/gl/gl.h>
#include "GameObject.h"
#include "DrawableObject.h"

class Scene
{
protected:
	std::list<GameObjectRef> _gameObjects;
	std::list<DrawableObjectRef> _drawableObjects;
	ci::ColorA8u _backGround;
	ci::gl::Texture2dRef _backGroundTex;
	ci::Rectf _sceneArea;
	Scene(const ci::Rectf& area);

protected:
	std::list<GameObjectRef>::const_iterator findObjectIter(const GameObject* pObject) const;
public:
	virtual ~Scene();

	void update(float t);
	void draw();

	void setBackgroundColor(const ci::ColorA8u& _backGround);
	const ci::ColorA8u& getBackgroundColor()const;

	void addDrawbleObject(DrawableObjectRef drawableObject);
	void addGameObject(GameObjectRef gameObjectRef);
	void removeGameObject(GameObjectRef gameObjectRef);
	GameObjectRef findObjectRef(const GameObject* pObject) const;
	const std::list<GameObjectRef>& getObjects() const;
	std::list<GameObjectRef>& getObjects();
	const std::list<DrawableObjectRef>& getDrawableObjects() const;
	std::list<DrawableObjectRef>& getDrawableObjects();
	const ci::Rectf& getSceneArea() const;

	// current, the engine is allow create only one scene during game time
	static Scene* createScene(const ci::Rectf& area);
	static Scene* getCurrentScene();
};

