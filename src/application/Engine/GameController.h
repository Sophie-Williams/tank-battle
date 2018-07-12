#pragma once
#include "GameObject.h"
#include "DrawableObject.h"
#include <map>

class GameController
{
private:
	std::map<GameObjectId, int> _killCounter;
protected:
	GameController();
public:
	virtual~GameController();
	static GameController* getInstance();

	void OnBulletCollisionDetected(GameObjectRef bullet, DrawableObjectRef other, const CollisionInfo& poistion, float t);
	int getKills(GameObjectId id);
};