#pragma once
#include "../common/ILogger.h"

class WxAppLog;
class LogAdapter : public ILogger {
	WxAppLog* _appLog;
	class FileLoggerImpl;
	FileLoggerImpl* _fileLogger;
	LogLevel _currenLoglevel;

public:
	LogAdapter(WxAppLog* appLog);
	~LogAdapter();
	virtual void log(LogLevel logLevel, const char* message);
	virtual void logV(LogLevel logLevel, const char* fmt, ...);
	virtual void logVA(LogLevel logLevel, const char* fmt, va_list args);
};