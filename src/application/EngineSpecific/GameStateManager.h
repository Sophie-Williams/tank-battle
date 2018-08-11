#pragma once
#include "../Engine/GameComponent.h"
#include "../Engine/LiveObject.h"
#include "GameControllerInterfaces.h"
#include <list>

class GameStateManager : public GameComponent
{
	bool _gameOver;
	float _lastStandDetectedAt;
	float _firstTimeNoKillingObjectDetectedAt;
	float _maxMatchDuration;
	float _startMatchTime;
	GameObjectId _winner;
	// list of killing object at a specific of time
	std::list<DrawableObjectRef> _snapshotKillingObjects;
	std::list<std::shared_ptr<LiveObject>> _mustAliveObjects;
protected:
	//void onTankCollisionDetected(DrawableObjectRef other, const CollisionInfo& poistion, float t);
public:
	GameStateManager();
	virtual ~GameStateManager();

	virtual void update(float t);

	void initState();
	GameObjectId getWinner() const;
	bool isGameOver() const;
	void addMonitorObject(const std::shared_ptr<LiveObject>&);
};