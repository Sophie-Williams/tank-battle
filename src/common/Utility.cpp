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