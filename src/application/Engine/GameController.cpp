#include "GameController.h"
#include "Bullet.h"
#include "Tank.h"
#include "Scene.h"
#include "LiveObject.h"


GameController* GameController::getInstance() {
	static GameController s_Instance;
	return &s_Instance;
}

GameController::GameController() {
}

GameController::~GameController() {
}

void GameController::OnBulletCollisionDetected(GameObjectRef bullet, DrawableObjectRef other, const ColissionPosition& poistion, float t) {
	auto pBullet = dynamic_cast<Bullet*>(bullet.get());
	if (pBullet) {
		// the bullet is not impact to its owner
		if (pBullet->getOwner().get() == other.get()) {
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
		}
		
		if (other->canBeWentThrough() == false) {
			// destroy bullet
			bullet->destroy(t);
		}
	}
}