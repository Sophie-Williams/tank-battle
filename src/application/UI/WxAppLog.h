/******************************************************************
* File:        WxAppLog.h
* Description: declare WxAppLog class. This class is responsible
*              for showing log.
* Author:      Vincent Pham
*
* Copyright (c) 2018 VincentPT.
** Distributed under the MIT License (http://opensource.org/licenses/MIT)
**
*
**********************************************************************/

#pragma once
#include "ImWidget.h"

class WxAppLog : public ImWidget
{
public:
	enum class LogLevel : int
	{
		Verbose = 0,
		Debug,
		Info,
		Error,
	};
private:
    ImGuiTextBuffer     Buf;
    ImGuiTextFilter     Filter;
    ImVector<int>       LineOffsets;        // Index to lines offset
	LogLevel _logLevel;
    bool                ScrollToBottom;
	MouseDoubleClickEventHandler _doubleClickHandler;
	mutable std::mutex _mutex;
public:
	WxAppLog();
	~WxAppLog();
    void    clear()     { Buf.clear(); LineOffsets.clear(); }

    void    addLog(LogLevel logLevel, const char* fmt, ...) ;//IM_FMTARGS(2);
	void    addLogV(LogLevel logLevel, const char* fmt, va_list args);
	void    update();
	void setLogLevel(LogLevel logLevel);
	LogLevel getLogLevel() const;
	void setDoubleClickHandler(MouseDoubleClickEventHandler&& handler);
};
