#pragma once
#include <memory>

#include "cinder/gl/gl.h"

class GameObject
{
protected:
	ci::Rectf _boundRect;
	// transformation matrix
	glm::mat4 _tMat;
	ci::vec2 _pivot;

	virtual void drawInternal();
public:
	GameObject();
	virtual ~GameObject();

	virtual void setBound(const ci::Rectf& boundRect);
	virtual const ci::Rectf& getBound() const;
	virtual void setPivot(const ci::vec2& pivot);
	virtual void update(float t) = 0;
	virtual void draw();

	virtual void move(const ci::vec2& offset);
	virtual void rotate(const float& angle);

};

typedef std::shared_ptr<GameObject> GameObjectRef;

