/******************************************************************
* File:        LogAdapter.cpp
* Description: implement LogAdapter class. A logger adapter class
*              used to print debugging information in game engine to
*              game UI.
* Author:      Vincent Pham
*
* Copyright (c) 2018 VincentPT.
** Distributed under the MIT License (http://opensource.org/licenses/MIT)
**
*
**********************************************************************/

#include "LogAdapter.h"
#include "UI/WxAppLog.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/rotating_file_sink.h"



class LogAdapter::FileLoggerImpl {
	std::shared_ptr<spdlog::logger> _logger;
public:
	FileLoggerImpl() {
		_logger = spdlog::rotating_logger_mt("main log", "TankBattle.log", 1048576 * 5, 3);
	}

	void setLogLevel(LogLevel level) {
		switch (level)
		{
		case LogLevel::Verbose:
			_logger->set_level(spdlog::level::trace);
			break;
		case LogLevel::Debug:
			_logger->set_level(spdlog::level::debug);
			break;
		case LogLevel::Info:
			_logger->set_level(spdlog::level::info);
			break;
		case LogLevel::Error:
			_logger->set_level(spdlog::level::err);
			break;
		default:
			_logger->set_level(spdlog::level::info);
			break;
		}
	}

	void log(LogLevel level, const char* message) {
		switch (level)
		{
		case LogLevel::Verbose:
			_logger->trace(message);
			break;
		case LogLevel::Debug:
			_logger->debug(message);
			break;
		case LogLevel::Info:
			_logger->info(message);
			break;
		case LogLevel::Error:
			_logger->error(message);
			break;
		default:
			_logger->warn("unknown log level {}", message);
			break;
		}

		_logger->flush();
	}

	void logV(LogLevel level, const char* fmt, ...) {
		va_list args;
		va_start(args, fmt);
		char buffer[256];
		vsprintf(buffer, fmt, args);
		va_end(args);

		log(level, buffer);
	}

	void logVA(LogLevel level, const char* fmt, va_list args) {
		char buffer[256];
		vsprintf(buffer, fmt, args);

		log(level, buffer);
	}
};

LogAdapter::LogAdapter() : _fileLogger(nullptr) {
}

LogAdapter::~LogAdapter() {
	if (_fileLogger) {
		delete _fileLogger;
	}
}

void LogAdapter::setApplog(const std::shared_ptr<WxAppLog>& appLog) {
	_appLog = appLog;
	_fileLogger = new FileLoggerImpl();
	_currenLoglevel = (LogLevel)_appLog->getLogLevel();
}

#define UPDATE_LOGLEVEL() \
if(_currenLoglevel != (LogLevel)_appLog->getLogLevel()) { _currenLoglevel = (LogLevel)_appLog->getLogLevel(); \
_fileLogger->setLogLevel(_currenLoglevel); \
}

void LogAdapter::log(LogLevel logLevel, const char* message) {
	_appLog->addLog((WxAppLog::LogLevel)logLevel, message);

	UPDATE_LOGLEVEL();
	_fileLogger->log(logLevel, message);
}

 void LogAdapter::logV(LogLevel logLevel, const char* fmt, ...) {
	 va_list args;
	 va_start(args, fmt);
	 _appLog->addLogV((WxAppLog::LogLevel)logLevel, fmt, args);

	 UPDATE_LOGLEVEL();
	 _fileLogger->logVA(logLevel, fmt, args);

	 va_end(args);
}

 void LogAdapter::logVA(LogLevel logLevel, const char* fmt, va_list args) {
	 _appLog->addLogV((WxAppLog::LogLevel)logLevel, fmt, args);

	 UPDATE_LOGLEVEL();
	 _fileLogger->logVA(logLevel, fmt, args);
 }