#pragma once
#include <list>

#include "GameComponent.h"

class GameObject
{
protected:
	bool _available;
	std::list<GameComponentRef> _components;
protected:
public:
	GameObject();
	virtual ~GameObject();

	virtual void update(float t);
	
	virtual bool isAvailable() const;
	virtual void setAvailable(bool flag);
	virtual void destroy(float t);
	virtual void addComponent(const GameComponentRef& component);
};

typedef std::shared_ptr<GameObject> GameObjectRef;