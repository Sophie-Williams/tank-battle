#include "GameController.h"
#include "Bullet.h"
#include "Tank.h"
#include "engine/Scene.h"
#include "engine/LiveObject.h"


GameController* GameController::getInstance() {
	static GameController s_Instance;
	return &s_Instance;
}

GameController::GameController() {
}

GameController::~GameController() {
}

void GameController::OnBulletCollisionDetected(GameObjectRef bullet, DrawableObjectRef other, const CollisionInfo& poistion, float t) {
	auto pBullet = dynamic_cast<Bullet*>(bullet.get());
	if (pBullet) {
		// the bullet is not impact to its owner
		GameObject* owner = pBullet->getOwner().get();
		if (owner == other.get()) {
			return;
		}
		if (dynamic_cast<Bullet*>(other.get()) != nullptr) {
			// does not allow bullet hit together
			return;
		}

		// check if the bullet hit a lived object
		auto liveObject = dynamic_cast<LiveObject*>(other.get());
		if (liveObject) {
			// make it hurt
			liveObject->takeDamage(pBullet->getDamage());
			if (liveObject->isAvailable() == false) {
				auto it = _killCounter.insert(std::make_pair(owner->getId(), 1));
				if (it.second == false) {
					it.first->second++;
				}
			}
		}
		
		if (other->canBeWentThrough() == false) {
			// destroy bullet
			bullet->destroy(t);
		}
	}
}

int GameController::getKills(GameObjectId id) {
	auto it = _killCounter.find(id);
	if (it == _killCounter.end()) {
		return 0;
	}

	return it->second;
}