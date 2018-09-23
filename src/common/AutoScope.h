/******************************************************************
* File:        AutoScope.h
* Description: declare AutoScope class. This class is
*              designed to execute a task when a scope entered or
*              left.
* Author:      Vincent Pham
*
* Copyright (c) 2018 VincentPT.
** Distributed under the MIT License (http://opensource.org/licenses/MIT)
**
*
**********************************************************************/

#pragma once
#include <functional>

class AutoScope
{
	std::function<void(bool)> _scopeAccess;
public:
	AutoScope(std::function<void(bool)>&& scopeAccess);
	~AutoScope();
};

