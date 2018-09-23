/******************************************************************
* File:        Utility.cpp
* Description: implement utility function of logging.
* Author:      Vincent Pham
*
* Copyright (c) 2018 VincentPT.
** Distributed under the MIT License (http://opensource.org/licenses/MIT)
**
*
**********************************************************************/

#ifdef WIN32
#include <Windows.h>
#endif // WIN32


#include "Utility.h"

void Utility::logScopeAccess(ILogger* logger, const char* functionName, bool access) {
	if (logger) {
		if (access) {
			logger->logV(LogLevel::Debug, ">>> %s\n", functionName);
		}
		else {
			logger->logV(LogLevel::Debug, "<<< %s\n", functionName);
		}
	}
}