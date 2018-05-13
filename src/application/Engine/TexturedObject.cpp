#include "TexturedObject.h"
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
	auto img = loadImage(imageFile);
	_tex = gl::Texture2d::create(img);
}