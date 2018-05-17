#include "Animation.h"
using namespace ci;

Animation::Animation() : _iterator(-1), _frameCount(-1), _loop(1), _startAt(-1), _frameDuration(1.0f/30.0f) {
}

Animation::~Animation() {}

void Animation::updateInternal(float t) {
	if (_tex) {
		if (_frameSize.x == 0 && _frameSize.y == 0) {
			_frameSize.x = _tex->getWidth();
			_frameSize.y = _tex->getHeight();
		}
		if (_frameCount == -1) {
			autoCalculateFrameCount();
		}
		auto delta = t - _startAt;
		int frameIter = (int)(delta / _frameDuration);
		_loop -= frameIter / _frameCount;
		_iterator = frameIter % _frameCount;
	}
}

void Animation::drawInternal() {
	if (_loop <= 0 || _startAt == -1) {
		return;
	}
	if (_tex) {
		if (_frameSize.x != 0 && _frameSize.y != 0) {
			int cols = _tex->getWidth() / _frameSize.x;
			int y = _iterator / cols;
			int x = _iterator % cols;

			y *= _frameSize.y;
			x *= _frameSize.x;

			ci::Area frameArea(x, y, x + _frameSize.x, y + _frameSize.y);

			gl::draw(_tex, frameArea, getBound());
		}
	}
}

void Animation::reset() {
	_startAt = -1;
	_iterator = -1;
	_loop = 1;
}

void Animation::start(float t) {
	_startAt = t;
	_iterator = 0;
}

void Animation::stop() {
}

void Animation::setLoop(int loop) {
	_loop = loop;
}

void Animation::autoCalculateFrameCount() {
	int cols = _tex->getWidth() / _frameSize.x;
	int rows = _tex->getHeight() / _frameSize.y;

	_frameCount = cols * rows;
}

// set frameCount to -1 to auto compute frame count base on image size and frame size
void Animation::setFrameCount(int frameCount) {
	if (frameCount == -1 && _tex) {
		autoCalculateFrameCount();
	}
	else {
		_frameCount = frameCount;
	}
}

void Animation::setDisplayFrameDuration(float duration) {
	_frameDuration = duration;
}