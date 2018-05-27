#include "DrawableObject.h"

using namespace ci;

DrawableObject::DrawableObject() : _allowGoThrough(true), _staticObject(true) {

	// at begin transformation of previous frame is not available
	// so the time is set to -1
	_previousMatFrame.second = -1;
}

DrawableObject::~DrawableObject() {
}

void DrawableObject::setBound(const ci::Rectf& boundRect) {
	_boundRect = boundRect;

	setPivot(vec3(_boundRect.getCenter(), 0));
}

const ci::Rectf& DrawableObject::getBound() const {
	return _boundRect;
}

ci::vec2 transform(const ci::vec2& point, const glm::mat4& m) {
	glm::mat4::col_type p4(point, 0, 1);
	p4 = m * p4;

	return ci::vec2(p4.x, p4.y);
}

void DrawableObject::getBoundingPoly(std::vector<ci::vec2>& boundingPoly) const {
	auto& rect = getBound();
	auto& m = getTransformation();
	boundingPoly[0] = transform(rect.getUpperLeft(), m);
	boundingPoly[1] = transform(rect.getUpperRight(), m);
	boundingPoly[2] = transform(rect.getLowerRight(), m);
	boundingPoly[3] = transform(rect.getLowerLeft(), m);
}

void DrawableObject::setPivot(const ci::vec3& pivot) {
	_pivot = pivot;
}

const ci::vec3& DrawableObject::getPivot() const {
	return _pivot;
}

void DrawableObject::translate(const ci::vec3& offset) {
	_tMat = glm::translate(_tMat, offset);
}

void DrawableObject::rotate(const float& angle) {
	// translate to object's origin
	_tMat = glm::translate(_tMat, -_pivot);
	// rotate object by 0z
	_tMat = glm::rotate(_tMat, angle, ci::vec3(0, 0, 1));
	// translate object back to current position
	_tMat = glm::translate(_tMat, _pivot);
}

void DrawableObject::setTransformation(const glm::mat4& tMat) {
	_tMat = tMat;
}

const glm::mat4& DrawableObject::getTransformation() const {
	return _tMat;
}

const std::pair<glm::mat4, float>& DrawableObject::getPreviousTransformation() const {
	return _previousMatFrame;
}

bool DrawableObject::canBeWentThrough() const {
	return _allowGoThrough;
}

void DrawableObject::allowGoThrough(bool allowGoThrough) {
	_allowGoThrough = allowGoThrough;
}

void DrawableObject::setObjectStaticFlag(bool staticFlag) {
	_staticObject = staticFlag;
}

bool DrawableObject::isStaticObject() const {
	return _staticObject;
}

bool DrawableObject::canBeWentThrough(DrawableObject* other) const {
	return (canBeWentThrough() || other->canBeWentThrough());
}

const CollisionDetectedHandler& DrawableObject::getCollisionHandler() const {
	return _collisionHandler;
}

void DrawableObject::setCollisionHandler(CollisionDetectedHandler&& handler) {
	_collisionHandler = handler;
}

void DrawableObject::drawInternal() {
#ifdef SHOW_OBJECT_BOUNDARY
	gl::ScopedColor foreGroundColor(1, 0, 0);
	gl::drawStrokedRect(_boundRect);
#endif
}

void DrawableObject::update(float t) {
	GameObject::update(t);

	_previousMatFrame.first = _tMat;
	_previousMatFrame.second = t;
	updateInternal(t);
}

void DrawableObject::draw() {
	auto currentModelMatrix = gl::getModelMatrix();
	auto newMatrix = currentModelMatrix * _tMat;
	gl::setModelMatrix(newMatrix);

	drawInternal();

	gl::setModelMatrix(currentModelMatrix);
}

