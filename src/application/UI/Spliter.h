/******************************************************************
* File:        Spliter.h
* Description: declare Spliter class. This class is a binary.
*              container class which splits its two control objects
*              side by side or up and down.
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

class Spliter : public CiWidget
{
public:
	enum class LayoutType
	{
		Auto = 0,
		Panel1Fixed,
		Panel2Fixed,
	};
protected:
	std::shared_ptr<Widget> _child1;
	std::shared_ptr<Widget> _child2;
	LayoutType _layoutType;
	bool _horizontal;
	float _relativeSize;
	float _panelSize;

	void updateRelativeSize();
	void updateChildrenrGeometrics();
public:
	Spliter();
	virtual ~Spliter();

	void update();
	void draw();
	void setSize(float w, float h);
	void setPos(float x, float y);

	void setChild1(const std::shared_ptr<Widget>& child1);
	void setChild2(const std::shared_ptr<Widget>& child2);
	const std::shared_ptr<Widget>& getChild1() const;
	const std::shared_ptr<Widget>& getChild2() const;

	void setLayoutType(LayoutType layoutType);
	void setVertical(bool);
	void setLayoutFixedSize(float size);
	void setLayoutRelative(float panel1SizeRatio);
};

