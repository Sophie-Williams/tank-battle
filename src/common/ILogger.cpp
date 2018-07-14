#include "ILogger.h"

static ILogger* s_ILoggerInstance = NULL;

ILogger::ILogger() {
    s_ILoggerInstance = this;
}

ILogger::~ILogger() {
	s_ILoggerInstance = nullptr;
}

ILogger* ILogger::getInstance() {
    return s_ILoggerInstance;
}