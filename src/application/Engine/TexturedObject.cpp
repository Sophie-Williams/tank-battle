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
	_tex = gl::Texture2d::create(img);
}