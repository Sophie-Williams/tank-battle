#pragma once
#include <cstdarg>

enum class LogLevel
{
	Verbose,
	Debug,
	Info,
	Error
};

class ILogger {
public:
	ILogger();
	virtual ~ILogger();
	static ILogger* getInstance();

	virtual void log(LogLevel logLevel, const char* message) = 0;
	virtual void logV(LogLevel logLevel, const char* fmt,...) = 0;
	virtual void logVA(LogLevel logLevel, const char* fmt, va_list args) = 0;
};
