/******************************************************************
* File:        Scene.h
* Description: declare Scene class. A Scene object contains any
*              drawable object in a game at a specific scene or map.
* Author:      Vincent Pham
*
* Copyright (c) 2018 VincentPT.
** Distributed under the MIT License (http://opensource.org/licenses/MIT)
**
*
**********************************************************************/

#pragma once
#include <list>
#include <cinder/gl/gl.h>
#include "DrawableObject.h"

class Scene
{
protected:
	std::list<DrawableObjectRef> _drawableObjects;
	ci::ColorA8u _backGround;
	ci::gl::Texture2dRef _backGroundTex;
	ci::Rectf _sceneArea;
	Scene(const ci::Rectf& area);
protected:
	std::list<DrawableObjectRef>::const_iterator findObjectIter(const DrawableObject* pObject) const;
public:
	virtual ~Scene();

	void update(float t);
	void draw();

	void setBackgroundColor(const ci::ColorA8u& _backGround);
	const ci::ColorA8u& getBackgroundColor()const;

	void addDrawbleObject(DrawableObjectRef drawableObject);
	DrawableObjectRef findObjectRef(const DrawableObject* pObject) const;
	const std::list<DrawableObjectRef>& getDrawableObjects() const;
	std::list<DrawableObjectRef>& getDrawableObjects();
	const ci::Rectf& getSceneArea() const;

	// current, the engine is allow create only one scene during game time
	static Scene* createScene(const ci::Rectf& area);
	static Scene* getCurrentScene();
};

