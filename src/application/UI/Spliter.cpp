#include "Spliter.h"

void Spliter::updateChildrenrGeometrics() {
	if (_layoutType == LayoutType::Auto) {
		if (_horizontal) {
			if (_child1) {
				_child1->setPos(_window_pos.x, _window_pos.y);
				_child1->setSize(_window_size.x, _window_size.y*_relativeSize);
			}
			if (_child2) {
				_child2->setPos(_window_pos.x, _window_size.y*_relativeSize);
				_child2->setSize(_window_size.x, _window_pos.y + _window_size.y - _child2->getY());
			}
		}
		else {
			if (_child1) {
				_child1->setPos(_window_pos.x, _window_pos.y);
				_child1->setSize(_window_size.x*_relativeSize, _window_size.y);
			}
			if (_child2) {
				_child2->setPos(_window_size.x*_relativeSize, _window_pos.y);
				_child2->setSize(_window_size.x - _child2->getX(), _window_size.y);
			}
		}
	}
	else {
		auto panel1Size = _panelSize;
		if (_horizontal) {
			if (_layoutType == LayoutType::Panel2Fixed) {
				panel1Size = _window_size.y - _panelSize;
			}

			if (_child1) {
				_child1->setPos(_window_pos.x, _window_pos.y);
				_child1->setSize(_window_size.x, panel1Size);
			}
			if (_child2) {
				_child2->setPos(_window_pos.x, _window_pos.y + panel1Size);
				_child2->setSize(_window_size.x, _window_pos.y +_window_size.y - _child2->getY());
			}
		}
		else {
			if(_layoutType == LayoutType::Panel2Fixed) {
				panel1Size = _window_size.x - _panelSize;
			}

			if (_child1) {
				_child1->setPos(_window_pos.x, _window_pos.y);
				_child1->setSize(panel1Size, _window_size.y);
			}
			if (_child2) {
				_child2->setPos(_window_pos.x + panel1Size, _window_pos.y);
				_child2->setSize(_window_size.x - _child2->getX(), _window_size.y);
			}
		}
	}
}

Spliter::Spliter() :_relativeSize(0.5), _horizontal(true), _layoutType(LayoutType::Auto) {
	CiWidget::setPos(0, 0);
	CiWidget::setSize(300, 200);
}
Spliter::~Spliter(){}

void Spliter::update() {
	if (_child1) _child1->update();
	if (_child2) _child2->update();
}

void Spliter::draw() {
	FUNCTON_LOG();
	if (_child1) _child1->draw();
	if (_child2) _child2->draw();
}

void Spliter::setSize(float w, float h) {
	CiWidget::setSize(w, h);
	updateChildrenrGeometrics();
}

void Spliter::setPos(float x, float y) {
	CiWidget::setPos(x, y);
	updateChildrenrGeometrics();
}

void Spliter::setChild1(const std::shared_ptr<Widget>& child1) {
	_child1 = child1;
	updateChildrenrGeometrics();
}

void Spliter::setChild2(const std::shared_ptr<Widget>& child2) {
	_child2 = child2;
	updateChildrenrGeometrics();
}

const std::shared_ptr<Widget>& Spliter::getChild1() const {
	return _child1;
}

const std::shared_ptr<Widget>& Spliter::getChild2() const {
	return _child2;
}

void Spliter::updateRelativeSize() {
	_relativeSize = 0.5f;
	if (_horizontal) {
		if (_child1) {
			_relativeSize = _child1->getHeight() / getHeight();
		}
		else if (_child2) {
			_relativeSize = 1.0f - _child2->getHeight() / getHeight();
		}
	}
	else
	{
		if (_child1) {
			_relativeSize = _child1->getWidth() / getWidth();
		}
		else if (_child2) {
			_relativeSize = 1.0f - _child2->getWidth() / getWidth();
		}
	}

	if (_relativeSize > 1) {
		_relativeSize = 1;
	}
	if (_relativeSize < 0) {
		_relativeSize = 0;
	}
}

void Spliter::setLayoutType(Spliter::LayoutType layoutType) {
	_layoutType = layoutType;
	if (_layoutType != LayoutType::Auto) {
		updateRelativeSize();
	}
}

void Spliter::setVertical(bool isVertical) {
	bool newStyle = !isVertical;
	if (_horizontal != newStyle) {

		_horizontal = newStyle;
		updateChildrenrGeometrics();
	}
}

void Spliter::setLayoutFixedSize(float size) {
	_panelSize = size;
}

void Spliter::setLayoutRelative(float panel1SizeRatio) {
	_relativeSize = panel1SizeRatio;
}