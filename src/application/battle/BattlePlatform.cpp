#include "BattlePlatform.h"

BattlePlatform::BattlePlatform(float width, float height) : 
	_battleArea(-width / 2, -height / 2, width / 2, height / 2) {
}

BattlePlatform::~BattlePlatform() {

}

const ci::Rectf& BattlePlatform::getMapArea() const {
	return _battleArea;
}

const std::list<std::shared_ptr<SnapshotObject>>& BattlePlatform::getSnapshotObjects() {
	return _snapshotObjects;
}