/******************************************************************
* File:        Panel.cpp
* Description: implement Panel class. This class is a container class.
*              Its object are used to contain UI controls.
* Author:      Vincent Pham
*
* Copyright (c) 2018 VincentPT.
** Distributed under the MIT License (http://opensource.org/licenses/MIT)
**
*
**********************************************************************/

#include "Panel.h"

void Panel::updateChildrenrGeometrics() {
	for (auto it = _children.begin(); it != _children.end(); it++) {
		(*it)->setSize(getWidth(), getHeight());
		(*it)->setPos(getX(), getY());
	}
}

Panel::Panel() {
	CiWidget::setPos(0, 0);
	CiWidget::setSize(300, 200);
}
Panel::~Panel(){}

void Panel::update() {
	for (auto it = _children.begin(); it != _children.end(); it++) {
		(*it)->update();
	}
}

void Panel::draw() {
	for (auto it = _children.begin(); it != _children.end(); it++) {
		(*it)->draw();
	}
}

void Panel::setSize(float w, float h) {
	CiWidget::setSize(w, h);
	updateChildrenrGeometrics();
}

void Panel::setPos(float x, float y) {
	CiWidget::setPos(x, y);
	updateChildrenrGeometrics();
}

void Panel::addChild(const std::shared_ptr<Widget>& child) {
	_children.push_back(child);
	updateChildrenrGeometrics();
}

const std::list<std::shared_ptr<Widget>>& Panel::getChildren() const {
	return _children;
}