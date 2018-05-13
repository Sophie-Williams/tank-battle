#pragma once
#include <memory>

#include "cinder/gl/gl.h"

class Scene;

class GameObject
{
protected:
	ci::Rectf _boundRect;
	// transformation matrix
	glm::mat4 _tMat;
	ci::vec3 _pivot;

protected:

	virtual void drawInternal();
public:
	GameObject();
	virtual ~GameObject();

	virtual void setBound(const ci::Rectf& boundRect);
	virtual const ci::Rectf& getBound() const;
	virtual void setPivot(const ci::vec3& pivot);
	virtual const ci::vec3& getPivot() const;

	virtual void update(float t) = 0;
	virtual void draw();

	virtual void move(const ci::vec3& offset);
	virtual void rotate(const float& angle);

	virtual void setTransformation(const glm::mat4& tMat);
	virtual const glm::mat4& getTransformation() const;
};

typedef std::shared_ptr<GameObject> GameObjectRef;

