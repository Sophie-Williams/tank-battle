#pragma once
#include <memory>
class GameObject;

class GameComponent
{
protected:
	GameObject* _owner;
	bool _isAvailble;
protected:
	void destroy();
	bool isAvailable();
public:
	GameComponent();
	virtual ~GameComponent();

	virtual void update(float t) = 0;

	friend class GameObject;
};

typedef std::shared_ptr<GameComponent> GameComponentRef;