#include "WxRadarView.h"

#include "cinder/gl/gl.h"
#include "cinder/app/App.h"

using namespace ci;
using namespace std;

#define BLUR_SIZE 128

WxRadarView::WxRadarView(ci::app::WindowRef parent) : _parent(parent) {

	// setup our blur Fbo's, smaller ones will generate a bigger blur
	_fboBlur = gl::Fbo::create(BLUR_SIZE, BLUR_SIZE);
}

WxRadarView::~WxRadarView(){}

void WxRadarView::update() {
}

void WxRadarView::setSize(float w, float h) {
	CiWidget::setSize(w, h);

	gl::Fbo::Format fmt;
	fmt.setSamples(16);

	// setup our scene Fbo
	//_fboScene = gl::Fbo::create((int)w, (int)h, fmt);
	_fboScene = gl::Fbo::create(w, h, fmt);
}

void WxRadarView::renderScene() {
	gl::ScopedColor scopeColor(0.05f, 0, 0, 1);
	// render objects
	gl::drawSolidRect(Rectf(10, 10, 50, 50));
}

void WxRadarView::draw() {
	if (!_fboScene) return;
	{
		//// render scene into mFboScene using illumination texture
		{
			// bind to draw to the frame buffer instead of screen
			gl::ScopedFramebuffer fbo(_fboScene);
			// set the viewport
			gl::ScopedViewport    viewport(0, 0, _fboScene->getWidth(), _fboScene->getHeight());
			// clear the frame buffer
			gl::clear(Color(0.2f, 0.2f, 0.3f));

			if (_radar) {
				_radar->draw();
			}
		}

		constexpr float attenuation = 1.5f;
		// bind the blur shader
		{
			gl::ScopedGlslProg shader(_glslBlurShader);
			_glslBlurShader->uniform("tex0", 0); // use texture unit 0

			// tell the shader to blur horizontally and the size of 1 pixel
			_glslBlurShader->uniform("sample_offset", vec2(1.0f / _fboBlur->getWidth(), 0.0f));
			_glslBlurShader->uniform("attenuation", attenuation);

			// copy a horizontally blurred version of our scene into the first blur Fbo
			{
				gl::ScopedFramebuffer fbo(_fboBlur);
				gl::ScopedViewport    viewport(0, 0, _fboBlur->getWidth(), _fboBlur->getHeight());

				gl::ScopedTextureBind tex0(_fboScene->getColorTexture(), (uint8_t)0);

				gl::ScopedMatrices scopeMatrices;
				gl::setMatricesWindow(_fboBlur->getWidth(), _fboBlur->getHeight());
				gl::clear(Color::black());

				gl::drawSolidRect(_fboBlur->getBounds());
			}

			// tell the shader to blur vertically and the size of 1 pixel
			_glslBlurShader->uniform("sample_offset", vec2(0.0f, 1.0f / _fboBlur->getHeight()));
			_glslBlurShader->uniform("attenuation", attenuation);

			// copy a vertically blurred version of our blurred scene into the second blur Fbo
			{
				gl::ScopedTextureBind tex0(_fboBlur->getColorTexture(), (uint8_t)0);
				gl::ScopedMatrices scopeMatrices;
				gl::setMatricesWindow(_fboBlur->getWidth(), _fboBlur->getHeight());
				gl::drawSolidRect(_fboBlur->getBounds());
			}
		}
	}
}

void WxRadarView::setShader(gl::GlslProgRef glslBlurShader) {
	_glslBlurShader = glslBlurShader;
}

void WxRadarView::setRadar(const std::shared_ptr<Radar> &radar) {
	_radar = radar;
}

const std::shared_ptr<Radar> WxRadarView::getRadar() const {
	return _radar;
}
