#include "Barrier.h"
using namespace ci;

Barrier::Barrier() {
	allowGoThrough(false);
}

Barrier::~Barrier() {}

void Barrier::updateInternal(float t) {
}

void Barrier::drawInternal() {
	//gl::ScopedColor foreGroundColor(1, 0, 0);
	//gl::drawStrokedRect(_boundRect);
}