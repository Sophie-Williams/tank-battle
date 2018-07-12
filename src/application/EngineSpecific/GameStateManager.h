#pragma once
#include "../Engine/GameComponent.h"
#include "GameControllerInterfaces.h"

class GameStateManager : public GameComponent
{
	bool _gameOver;
	int _beginTankCount;
	float _lastStandDetectedAt;
	GameObjectId _winner;
protected:
	//void onTankCollisionDetected(DrawableObjectRef other, const CollisionInfo& poistion, float t);
public:
	GameStateManager();
	virtual ~GameStateManager();

	virtual void update(float t);

	void initState(bool beginGameState = true);
	GameObjectId getWinner() const;
	bool isGameOver() const;
};