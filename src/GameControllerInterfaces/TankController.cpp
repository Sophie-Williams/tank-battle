#include "TankController.h"

#include <string.h>

TankController::TankController() : _playerName(nullptr) {}

TankController::~TankController() {
	if (_playerName) {
		free(_playerName);
	}
}

void TankController::setup(TankPlayerContext* player) {}

void TankController::cleanup(TankPlayerContext* player){}

void TankController::setName(const char* name) {
	if (_playerName) {
		free(_playerName);
	}
	auto len = strlen(name);
	_playerName = (char*)malloc(len + 1);
	strcpy(_playerName, name);
	_playerName[len] = 0;
}

const char* TankController::getName() const {
	return _playerName;
}