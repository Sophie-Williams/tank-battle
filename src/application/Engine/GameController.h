#pragma once
#include "GameObject.h"
#include "DrawableObject.h"

class GameController
{
	GameController();
public:
	virtual~GameController();
	static GameController* getInstance();

	void OnBulletCollisionDetected(GameObjectRef bullet, DrawableObjectRef other, float t);
};