#pragma once
#include <memory>

class GameObject
{
protected:
	bool _available;
protected:
public:
	GameObject();
	virtual ~GameObject();

	virtual void update(float t) = 0;

	virtual bool isAvailable() const;
	virtual void setAvailable(bool flag);
	virtual void destroy();
};

typedef std::shared_ptr<GameObject> GameObjectRef;

