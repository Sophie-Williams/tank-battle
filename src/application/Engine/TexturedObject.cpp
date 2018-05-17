#include "TexturedObject.h"
#include "cinder/app/App.h"
using namespace ci;

TexturedObject::TexturedObject() {
}

TexturedObject::~TexturedObject() {}


void TexturedObject::drawInternal() {
	if (_tex) {
		gl::draw(_tex, _boundRect);
	}
}

void TexturedObject::setTexture(const std::string& imageFile) {
	auto img = loadImage(ci::app::loadAsset((imageFile)));
	setTexture(gl::Texture2d::create(img));
}

void TexturedObject::setTexture(const gl::Texture2dRef& texture) {
	_tex = texture;
}