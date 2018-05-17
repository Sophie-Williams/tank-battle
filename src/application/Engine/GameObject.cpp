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

void GameObject::addComponent(const GameComponentRef& component) {
	_components.push_back(component);
	component->_owner = this;
}

void GameObject::update(float t) {
	for (auto it = _components.begin(); it != _components.end(); it++) {
		it->get()->update(t);
	}
}