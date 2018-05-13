#pragma once
#include "ImWidget.h"
#include <functional>
#include <vector>
#include <mutex>
#include <string>

class WxPlayerInfo :
	public ImWidget
{
	std::string _playerIdStr;
public:
	WxPlayerInfo(const std::string& playerName);
	virtual ~WxPlayerInfo();
	virtual void update();

	void setOnStartStopButtonClickHandler(ButtonClickEventHandler&& handler);
	void setStarStopButtonText(const char* buttonText);

	const char* getPlayer1() const;
	const char* getPlayer2() const;
};

