/******************************************************************
* File:        Panel.h
* Description: declare Panel class. This class is a container class.
*              Its object are used to contain UI controls.
* Author:      Vincent Pham
*
* Copyright (c) 2018 VincentPT.
** Distributed under the MIT License (http://opensource.org/licenses/MIT)
**
*
**********************************************************************/

#pragma once
#include "CiWidget.h"
#include <memory>

class Panel : public CiWidget
{
protected:
	std::list<std::shared_ptr<Widget>> _children;

	void updateChildrenrGeometrics();
public:
	Panel();
	virtual ~Panel();

	void update();
	void draw();
	void setSize(float w, float h);
	void setPos(float x, float y);

	void addChild(const std::shared_ptr<Widget>& child);
	const std::list<std::shared_ptr<Widget>>& getChildren() const;
};

