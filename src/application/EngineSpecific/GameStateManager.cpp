#include "GameStateManager.h"
#include "../Engine/GameEngine.h"
#include "../Engine/Tank.h"

GameStateManager::GameStateManager() : _beginTankCount(-1) {
	initState(false);
}

GameStateManager::~GameStateManager() {
}

int countTank(Tank** ppTank = nullptr) {
	int tankCount = 0;

	Tank* pTank = nullptr;

	auto& physicalObjects = GameEngine::getInstance()->getScene()->getDrawableObjects();
	for (auto it = physicalObjects.begin(); it != physicalObjects.end(); it++) {
		if ( (pTank = dynamic_cast<Tank*>(it->get()))) {
			tankCount++;
			if (ppTank) {
				*ppTank = pTank;
			}
		}
	}

	return tankCount;
}

//void GameStateManager::onTankCollisionDetected(DrawableObjectRef other, const CollisionInfo& poistion, float t) {
//}

void GameStateManager::initState(bool beginGameState) {
	_lastStandDetectedAt = -1;
	_gameOver = false;
	_winner = -1;
	_beginTankCount = -1;

	if (beginGameState) {
		_beginTankCount = 0;
		Tank* pTank = nullptr;

		auto& physicalObjects = GameEngine::getInstance()->getScene()->getDrawableObjects();
		for (auto it = physicalObjects.begin(); it != physicalObjects.end(); it++) {
			if ((pTank = dynamic_cast<Tank*>(it->get()))) {
				_beginTankCount++;
			}
		}
	}
}

void GameStateManager::update(float t) {
	if (_gameOver) return;

	Tank* lastTank;
	int tankCount = countTank(&lastTank);
	if (_beginTankCount == tankCount) return;

	if (tankCount == 0) {
		// game over but no tank is alive
		if (_beginTankCount != tankCount) {
			_gameOver = true;
		}
	}
	else if (tankCount == 1) {
		if (_lastStandDetectedAt == -1) {
			_lastStandDetectedAt = t;
		}
		// check if last stand is alive last 2 seconds
		else if ((t - _lastStandDetectedAt) >= 2.0f) {
			// game over and one last stand
			_gameOver = true;
			_winner = lastTank->getId();
		}
	}
}

GameObjectId GameStateManager::getWinner() const {
	return _winner;
}

bool GameStateManager::isGameOver() const {
	return _gameOver;
}