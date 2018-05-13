#include "GameObject.h"

using namespace ci;

GameObject::GameObject() {
}

GameObject::~GameObject() {
}

void GameObject::setBound(const ci::Rectf& boundRect) {
	_boundRect = boundRect;

	setPivot(_boundRect.getCenter());
}

const ci::Rectf& GameObject::getBound() const {
	return _boundRect;
}

void GameObject::setPivot(const ci::vec2& pivot) {
	_pivot = pivot;
}

void GameObject::move(const ci::vec2& offset) {
	_tMat = glm::translate(_tMat, ci::vec3(offset, 0));
}

void GameObject::rotate(const float& angle) {
	// translate to object's origin
	_tMat = glm::translate(_tMat, ci::vec3(-_pivot, 0));
	// rotate object by 0z
	_tMat = glm::rotate(_tMat, angle, ci::vec3(0, 0, 1));
	// translate object back to current position
	_tMat = glm::translate(_tMat, ci::vec3(_pivot, 0));
}

void GameObject::drawInternal() {
#ifdef SHOW_OBJECT_BOUNDARY
	gl::ScopedColor foreGroundColor(1, 0, 0);
	gl::drawStrokedRect(_boundRect);
#endif
}

void GameObject::draw() {
	auto currentModelMatrix = gl::getModelMatrix();
	auto newMatrix = currentModelMatrix * _tMat;
	gl::setModelMatrix(newMatrix);

	drawInternal();

	gl::setModelMatrix(currentModelMatrix);
}