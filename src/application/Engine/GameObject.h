#pragma once
#include <memory>

#include "cinder/gl/gl.h"

class Scene;

class GameObject
{
protected:
	bool _available;
	bool _allowGoThrough;
	ci::Rectf _boundRect;
	// transformation matrix
	glm::mat4 _tMat;
	// transformation matrix of previous frame
	std::pair<glm::mat4, float> _previousMatFrame;
	ci::vec3 _pivot;

protected:

	virtual void drawInternal();
	virtual void updateInternal(float t) = 0;
public:
	GameObject();
	virtual ~GameObject();

	virtual void setBound(const ci::Rectf& boundRect);
	virtual const ci::Rectf& getBound() const;
	virtual void setPivot(const ci::vec3& pivot);
	virtual const ci::vec3& getPivot() const;

	virtual void update(float t);
	virtual void draw();

	virtual void move(const ci::vec3& offset);
	virtual void rotate(const float& angle);

	virtual void setTransformation(const glm::mat4& tMat);
	virtual const glm::mat4& getTransformation() const;

	virtual const std::pair<glm::mat4, float>& getPreviousTransformation() const;

	virtual bool isAvailable() const;
	virtual void setAvailable(bool flag);
	virtual void destroy();
	
	virtual bool canBeWentThrough() const;
	virtual void allowGoThrough(bool allowGoThrough);
};

typedef std::shared_ptr<GameObject> GameObjectRef;

