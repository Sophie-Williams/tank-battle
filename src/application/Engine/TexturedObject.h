#pragma once
#include "DrawableObject.h"
#include "cinder/gl/gl.h"

class TexturedObject : public DrawableObject
{
protected:
	ci::gl::Texture2dRef _tex;
protected:
	virtual void drawInternal();
public:
	TexturedObject();
	virtual ~TexturedObject();

	void setTexture(const std::string& imageFile);
	void setTexture(const ci::gl::Texture2dRef& texture);
};