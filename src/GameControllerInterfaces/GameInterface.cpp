#include "GameInterface.h"

static GameInterface* s_gameInstance = nullptr;

GameInterface::GameInterface() {
	s_gameInstance = this;
}

GameInterface::~GameInterface() {
	s_gameInstance = nullptr;
}

GameInterface* GameInterface::getInstance() {
	return s_gameInstance;
}