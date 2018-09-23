/******************************************************************
* File:        GameObject.cpp
* Description: implement GameObject class. This is a base class
*              of any game object.
* Author:      Vincent Pham
*
* Copyright (c) 2018 VincentPT.
** Distributed under the MIT License (http://opensource.org/licenses/MIT)
**
*
**********************************************************************/

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
	for (auto it = _components.begin(); it != _components.end();) {
		auto gameComponent = it->get();
		if (gameComponent->isAvailable()) {
			gameComponent->update(t);
			it++;
		}
		else {
			auto itTemp = it;
			it++;
			_components.erase(itTemp);
		}
	}
}

const std::list<GameComponentRef>& GameObject::getComponents() const {
	return _components;
}