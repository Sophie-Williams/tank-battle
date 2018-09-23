/******************************************************************
* File:        TexturedObject.h
* Description: declare TexturedObject class. A TexturedObject object
*              is a drawable object using a texture to show its
*              appearance.
* Author:      Vincent Pham
*
* Copyright (c) 2018 VincentPT.
** Distributed under the MIT License (http://opensource.org/licenses/MIT)
**
*
**********************************************************************/

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