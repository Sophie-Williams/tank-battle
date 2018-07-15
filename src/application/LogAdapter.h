#pragma once
#include "../common/ILogger.h"
#include <memory>

class WxAppLog;
class LogAdapter : public ILogger {
	std::shared_ptr<WxAppLog> _appLog;
	class FileLoggerImpl;
	FileLoggerImpl* _fileLogger;
	LogLevel _currenLoglevel;

public:
	LogAdapter();
	~LogAdapter();
	void setApplog(const std::shared_ptr<WxAppLog>& appLog);
	virtual void log(LogLevel logLevel, const char* message);
	virtual void logV(LogLevel logLevel, const char* fmt, ...);
	virtual void logVA(LogLevel logLevel, const char* fmt, va_list args);
};