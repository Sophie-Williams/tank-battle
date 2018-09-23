/******************************************************************
* File:        AutoScope.cpp
* Description: implement AutoScope class. This class is
*              designed to execute a task when a scope entered or
*              left.
* Author:      Vincent Pham
*
* Copyright (c) 2018 VincentPT.
** Distributed under the MIT License (http://opensource.org/licenses/MIT)
**
*
**********************************************************************/

#include "AutoScope.h"

AutoScope::AutoScope(std::function<void(bool)>&& scopeAccess) : _scopeAccess(scopeAccess)
{
	_scopeAccess(true);
}

AutoScope::~AutoScope()
{
	_scopeAccess(false);
}
