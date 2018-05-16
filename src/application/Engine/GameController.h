#pragma once
#include "GameObject.h"

class GameController
{
	GameController();
public:
	virtual~GameController();
	static GameController* getInstance();

	void OnBulletCollisionDetected(GameObjectRef bullet, GameObjectRef other);
};