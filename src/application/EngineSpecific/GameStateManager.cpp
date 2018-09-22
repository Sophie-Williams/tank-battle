#include "GameStateManager.h"
#include "../Engine/GameEngine.h"
#include "../EngineSpecific/Tank.h"
#include "../EngineSpecific/Bullet.h"

GameStateManager::GameStateManager() {
	initState();
}

GameStateManager::~GameStateManager() {
}

int countLiveObject(LiveObject** ppLiveObject = nullptr) {
	int liveObjectCount = 0;

	LiveObject* pLiveObject = nullptr;

	auto& physicalObjects = GameEngine::getInstance()->getScene()->getDrawableObjects();
	for (auto it = physicalObjects.begin(); it != physicalObjects.end(); it++) {
		if ( (pLiveObject = dynamic_cast<LiveObject*>(it->get()))) {
			liveObjectCount++;
			if (ppLiveObject) {
				*ppLiveObject = pLiveObject;
			}
		}
	}

	return liveObjectCount;
}

//void GameStateManager::onTankCollisionDetected(DrawableObjectRef other, const CollisionInfo& poistion, float t) {
//}

void GameStateManager::initState() {
	_lastStandDetectedAt = -1;
	// max match duration is 5 minutes
	_maxMatchDuration = 5 * 60;
	_startMatchTime = GameEngine::getInstance()->getCurrentTime();
	_gameOver = false;
	_winner = -1;
	_snapshotKillingObjects.clear();
	_firstTimeNoKillingObjectDetectedAt = -1;
	_mustAliveObjects.clear();
}

void GameStateManager::update(float t) {
	if (_gameOver) return;
	if (_mustAliveObjects.size() == 0) return;
	int mustAliveObjectCount = 0;
	int aliveId = -1;
	for (auto it = _mustAliveObjects.begin(); it != _mustAliveObjects.end(); it++) {
		if (it->get()->isAvailable()) {
			mustAliveObjectCount++;
			aliveId = it->get()->getId();
		}
	}

	if (t - _startMatchTime >= _maxMatchDuration) {
		ILogger::getInstance()->logV(LogLevel::Info, "The time is over, the match has been force terminate\n");
		_winner = mustAliveObjectCount == 1 ? aliveId : -1;
		_gameOver = true;
		return;
	}

	

	// check if there is no object in monitored list alive
	if (mustAliveObjectCount == 0) {
		_winner = -1;
		_gameOver = true;
		return;
	}

	if (mustAliveObjectCount == 1) {
		LiveObject* lastLiveObject;
		int aliveCount = countLiveObject(&lastLiveObject);
		if (aliveCount == 1) {
			if (_lastStandDetectedAt == -1) {
				// take a snapshot of killing object at the time when only one live object left
				_snapshotKillingObjects.clear();
				auto& physicalObjects = GameEngine::getInstance()->getScene()->getDrawableObjects();
				for (auto it = physicalObjects.begin(); it != physicalObjects.end(); it++) {
					// current game, there is only bullet game object can hurt other object
					if (dynamic_cast<Bullet*>(it->get())) {
						_snapshotKillingObjects.push_back(*it);
					}
				}

				_lastStandDetectedAt = t;
			}
			// check if last stand is alive last 2 seconds
			else {
				// wait until all killing object is self-destroyed;
				for (auto it = _snapshotKillingObjects.begin(); it != _snapshotKillingObjects.end(); it++) {
					if (it->get()->isAvailable()) {
						return;
					}
				}
				// no need to store these objects
				_snapshotKillingObjects.clear();
				if (_firstTimeNoKillingObjectDetectedAt < 0) {
					_firstTimeNoKillingObjectDetectedAt = t;
				}

				// wait more 0.5 second to set game is over
				if ((t - _firstTimeNoKillingObjectDetectedAt) >= 0.5f) {
					// game over and one last stand
					_gameOver = true;
					_winner = lastLiveObject->getId();
				}
			}
		}
	}
}

GameObjectId GameStateManager::getWinner() const {
	return _winner;
}

bool GameStateManager::isGameOver() const {
	return _gameOver;
}

void GameStateManager::addMonitorObject(const std::shared_ptr<LiveObject>& liveObject) {
	_mustAliveObjects.push_back(liveObject);
}