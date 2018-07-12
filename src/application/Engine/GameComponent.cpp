#include "GameComponent.h"

GameComponent::GameComponent() : _owner(nullptr), _isAvailble(true){
}

GameComponent::~GameComponent() {
	destroy();
}

void GameComponent::destroy() {
	_isAvailble = false;
}

bool GameComponent::isAvailable() {
	return _isAvailble;
}