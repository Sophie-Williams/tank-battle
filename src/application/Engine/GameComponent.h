#pragma once
#include <memory>
class GameObject;

class GameComponent
{
protected:
	GameObject* _owner;
protected:
public:
	GameComponent();
	virtual ~GameComponent();

	virtual void update(float t) = 0;

	friend class GameObject;
};

typedef std::shared_ptr<GameComponent> GameComponentRef;