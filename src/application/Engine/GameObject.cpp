#include "GameObject.h"

GameObject::GameObject() : _available(true) {
}

GameObject::~GameObject() {
}

bool GameObject::isAvailable() const {
	return _available;
}

void GameObject::setAvailable(bool flag) {
	_available = flag;
}

void GameObject::destroy() {
	setAvailable(false);
}