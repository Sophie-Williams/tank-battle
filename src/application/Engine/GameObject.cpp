#include "GameObject.h"

using namespace ci;

GameObject::GameObject() {
}

GameObject::~GameObject() {
}

void GameObject::setBound(const ci::Rectf& boundRect) {
	_boundRect = boundRect;

	setPivot(vec3(_boundRect.getCenter(), 0));
}

const ci::Rectf& GameObject::getBound() const {
	return _boundRect;
}

void GameObject::setPivot(const ci::vec3& pivot) {
	_pivot = pivot;
}

const ci::vec3& GameObject::getPivot() const {
	return _pivot;
}

void GameObject::move(const ci::vec3& offset) {
	_tMat = glm::translate(_tMat, offset);
}

void GameObject::rotate(const float& angle) {
	// translate to object's origin
	_tMat = glm::translate(_tMat, -_pivot);
	// rotate object by 0z
	_tMat = glm::rotate(_tMat, angle, ci::vec3(0, 0, 1));
	// translate object back to current position
	_tMat = glm::translate(_tMat, _pivot);
}

void GameObject::setTransformation(const glm::mat4& tMat) {
	_tMat = tMat;
}

const glm::mat4& GameObject::getTransformation() const {
	return _tMat;
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