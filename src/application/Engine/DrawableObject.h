#pragma once
#include "GameObject.h"
#include "cinder/gl/gl.h"
#include <vector>

class DrawableObject;
typedef std::shared_ptr<DrawableObject> DrawableObjectRef;

typedef std::function<void(DrawableObjectRef, float)> CollisionDetectedHandler;

class DrawableObject : public GameObject
{
protected:
	bool _available;
	bool _allowGoThrough;
	bool _staticObject;
	ci::Rectf _boundRect;
	// transformation matrix
	glm::mat4 _tMat;
	// transformation matrix of previous frame
	std::pair<glm::mat4, float> _previousMatFrame;
	ci::vec3 _pivot;

	CollisionDetectedHandler _collisionHandler;

protected:
	virtual void drawInternal();
	virtual void updateInternal(float t) = 0;
public:
	DrawableObject();
	virtual ~DrawableObject();

	virtual void setBound(const ci::Rectf& boundRect);
	virtual const ci::Rectf& getBound() const;
	// bounding poly array must be initialized with at least 4 elements
	void getBoundingPoly(std::vector<ci::vec2>& boundingPoly) const;

	virtual void setPivot(const ci::vec3& pivot);
	virtual const ci::vec3& getPivot() const;

	virtual void update(float t);
	virtual void draw();

	virtual void translate(const ci::vec3& offset);
	virtual void rotate(const float& angle);

	virtual void setTransformation(const glm::mat4& tMat);
	virtual const glm::mat4& getTransformation() const;

	virtual const std::pair<glm::mat4, float>& getPreviousTransformation() const;

	virtual bool canBeWentThrough() const;
	virtual void allowGoThrough(bool allowGoThrough);
	virtual bool canBeWentThrough(DrawableObject* other) const;

	void setObjectStaticFlag(bool staticFlag);
	bool isStaticObject() const;

	const CollisionDetectedHandler& getCollisionHandler() const;
	void setCollisionHandler(CollisionDetectedHandler&&);
};