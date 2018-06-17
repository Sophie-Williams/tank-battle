#include "BattlePlatform.h"

static BattlePlatform* g_currentInstane = nullptr;

SnapshotObject::SnapshotObject() : objectBound(4) {
}

SnapshotObject::~SnapshotObject() {
}

SnapshotObject* SnapshotObject::clone() {
	return new SnapshotObject();
}

TankSnapshot::TankSnapshot() {
}

TankSnapshot::~TankSnapshot() {}

SnapshotObject* TankSnapshot::clone() {
	auto pTankSnapshot =  new TankSnapshot();
	pTankSnapshot->gun = gun;
	return pTankSnapshot;
}

BattlePlatform::BattlePlatform(float width, float height) : 
	_battleArea(-width / 2, -height / 2, width / 2, height / 2) {
	g_currentInstane = this;
}

BattlePlatform::~BattlePlatform() {
	g_currentInstane = nullptr;
}

const ci::Rectf& BattlePlatform::getMapArea() const {
	return _battleArea;
}

std::list<std::shared_ptr<SnapshotObject>>& BattlePlatform::getSnapshotObjects() {
	return _snapshotObjects;
}

BattlePlatform* BattlePlatform::getInstance() {
	return g_currentInstane;
}