#include "GameObject.h"

static int s_globalObjectId = 0;

GameObject::GameObject() : _available(true) {
	s_globalObjectId++;
	_objectId = s_globalObjectId;
}

GameObject::~GameObject() {
}

bool GameObject::isAvailable() const {
	return _available;
}

void GameObject::setAvailable(bool flag) {
	_available = flag;
}

void GameObject::destroy(float t) {
	setAvailable(false);
}

void GameObject::addComponent(const GameComponentRef& component) {
	_components.push_back(component);
	component->_owner = this;
}

GameObjectId GameObject::getId() const {
	return _objectId;
}

void GameObject::update(float t) {
	for (auto it = _components.begin(); it != _components.end(); it++) {
		it->get()->update(t);
	}
}