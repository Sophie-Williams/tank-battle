#pragma once
#include "GameObject.h"
#include "cinder/gl/gl.h"

class TexturedObject : public GameObject
{
protected:
	ci::gl::Texture2dRef _tex;
protected:
	virtual void drawInternal();
public:
	TexturedObject();
	virtual ~TexturedObject();

	void setTexture(const std::string& imageFile);
};