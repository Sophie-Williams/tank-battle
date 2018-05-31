#include "WxRadarView.h"

#include "cinder/gl/gl.h"
#include "cinder/app/App.h"

using namespace ci;
using namespace std;

#define BLUR_SIZE 128

WxRadarView::WxRadarView(ci::app::WindowRef parent) : _parent(parent) {

	// setup our blur Fbo's, smaller ones will generate a bigger blur
	_fboBlur1 = gl::Fbo::create(BLUR_SIZE, BLUR_SIZE);
	_fboBlur2 = gl::Fbo::create(BLUR_SIZE, BLUR_SIZE);
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
	_fboScene = gl::Fbo::create(512, 512, fmt);
}

void WxRadarView::renderScene() {
	gl::ScopedColor scopeColor(0.05f, 0, 0, 1);
	// render objects
	gl::drawSolidRect(Rectf(10, 10, 50, 50));
}

void WxRadarView::draw() {
	if (!_fboScene) return;
	gl::pushMatrices();
	//// render scene into mFboScene using illumination texture
	{
		// bind to draw to the frame buffer instead of screen
		gl::ScopedFramebuffer fbo(_fboScene);
		// set the viewport
		gl::ScopedViewport    viewport(0, 0, _fboScene->getWidth(), _fboScene->getHeight());
		// clear the frame buffer
		gl::clear(Color(0.2f, 0.2f, 0.3f));

		// set ortho camera at center of radar area
		CameraOrtho cam;
		cam.setOrtho(-100, 100, -100, 100, 1, -1);
		gl::ScopedProjectionMatrix scopeMatrices;
		gl::setProjectionMatrix(cam.getProjectionMatrix());

		// render the scene
		renderScene();
	}

	// bind the blur shader
	{
		gl::ScopedGlslProg shader(_glslBlurShader);
		_glslBlurShader->uniform("tex0", 0); // use texture unit 0

		// tell the shader to blur horizontally and the size of 1 pixel
		_glslBlurShader->uniform("sample_offset", vec2(1.0f / _fboBlur1->getWidth(), 0.0f));
		_glslBlurShader->uniform("attenuation", 5.0f);

		// copy a horizontally blurred version of our scene into the first blur Fbo
		{
			gl::ScopedFramebuffer fbo(_fboBlur1);
			gl::ScopedViewport    viewport(0, 0, _fboBlur1->getWidth(), _fboBlur1->getHeight());

			gl::ScopedTextureBind tex0(_fboScene->getColorTexture(), (uint8_t)0);

			gl::setMatricesWindow(BLUR_SIZE, BLUR_SIZE);
			gl::clear(Color::black());

			gl::drawSolidRect(_fboBlur1->getBounds());
		}

		// tell the shader to blur vertically and the size of 1 pixel
		_glslBlurShader->uniform("sample_offset", vec2(0.0f, 1.0f / _fboBlur2->getHeight()));
		_glslBlurShader->uniform("attenuation", 5.0f);

		// copy a vertically blurred version of our blurred scene into the second blur Fbo
		{
			gl::ScopedFramebuffer fbo(_fboBlur2);
			gl::ScopedViewport    viewport(0, 0, _fboBlur2->getWidth(), _fboBlur2->getHeight());

			gl::ScopedTextureBind tex0(_fboBlur1->getColorTexture(), (uint8_t)0);

			gl::setMatricesWindow(BLUR_SIZE, BLUR_SIZE);
			gl::clear(Color::black());

			gl::drawSolidRect(_fboBlur2->getBounds());
		}
	}

	gl::popMatrices();

	Rectf destRect(getX(), getY(), getX() + getWidth(), getY() + getHeight());
	{
		// draw blur scene to screen
		gl::ScopedTextureBind tex0(_fboBlur2->getColorTexture(), (uint8_t)0);
		gl::ScopedGlslProg shader(_glslFboToScreen);
		_glslFboToScreen->uniform("uTex0", 0);
		gl::drawSolidRect(destRect);
	}
	{
		// draw rada coordinate system
		gl::ScopedColor lineColorScope(0.2f, 0.2f, 0.3f);
		gl::drawLine((destRect.getUpperLeft() + destRect.getUpperRight()) / 2.0f, (destRect.getLowerLeft() + destRect.getLowerRight()) / 2.0f );
		gl::drawLine((destRect.getUpperLeft() + destRect.getLowerLeft()) / 2.0f, (destRect.getUpperRight() + destRect.getLowerRight()) / 2.0f);
	}
}

void WxRadarView::setShaders(gl::GlslProgRef glslBlurShader, gl::GlslProgRef glslFboToScreen) {
	_glslBlurShader = glslBlurShader;
	_glslFboToScreen = glslFboToScreen;
}
