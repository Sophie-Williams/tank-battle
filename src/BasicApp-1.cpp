#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;

class BasicApp : public App {
public:
	void	setup() override;
	void	draw() override;

	CameraOrtho			mCam;
	gl::BatchRef		mRect;
	gl::GlslProgRef		mGlsl;
};

void prepareSettings( BasicApp::Settings* settings )
{
	settings->setMultiTouchEnabled( false );
}
void BasicApp::setup()
{
	mCam.setOrtho(-100, 100, -100, 100, 1, -1);

	mGlsl = gl::GlslProg::create(gl::GlslProg::Format()
		.vertex(CI_GLSL(150,
			uniform mat4	ciModelViewProjection;
	in vec4			ciPosition;
	in vec2			ciTexCoord0;	
	in vec4			ciColor;
	out vec4		Color;
	out vec2		TexCoord0;

	void main(void) {
		gl_Position = ciModelViewProjection * ciPosition;
		TexCoord0 = ciTexCoord0;
		Color = ciColor;
	}
	))
		.fragment(CI_GLSL(150,
	in vec2				TexCoord0;
	in vec4				Color;
	out vec4			oColor;
	uniform float keepColorInRange = 0.3;
	uniform float radius = 0.5;

	vec4 checker(vec2 uv)
	{
		float x = uv.x - radius;
		float y = uv.y - radius;
		float v = x*x + y*y;
		if ( v <= keepColorInRange*keepColorInRange) {
			return Color;
		}

		v = sqrt(v);
		vec4 end = vec4(0, 0, 0, 1);
		vec4 u = end - Color;
		float k = (v - keepColorInRange) / (radius - keepColorInRange);
		if (k > 1.0) {
			return end;
		}
		return Color + u * (k * k);
	}

	void main(void) {
		oColor = checker(TexCoord0);
	}
	)));

	geom::Rect rect(Rectf(-50, -50, 50, 50));
	mRect = gl::Batch::create(rect, mGlsl);

	gl::enableDepthWrite();
	gl::enableDepthRead();
}

void BasicApp::draw()
{
	gl::clear(Color(0.2f, 0.2f, 0.3f));
	gl::ScopedProjectionMatrix scopeMatrices;
	gl::setProjectionMatrix(mCam.getProjectionMatrix());
	gl::ScopedColor scopeColor(0.2f, 0.2f, 0.3f, 1);
	//gl::ScopedGlslProg glslScope();	

	auto h = getWindow()->getHeight();
	ci::Area viewPort(100, 100, 500, 500);
	// mapping scene to view port area
	gl::ScopedViewport scopeViewPort(viewPort.x1, h - viewPort.y1 - viewPort.getHeight(), viewPort.getWidth(), viewPort.getHeight());
	mRect->draw();
}


// This line tells Cinder to actually create and run the application.
CINDER_APP( BasicApp, RendererGl, prepareSettings )
