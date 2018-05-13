#pragma once
#include "ImPopup.h"
#include <functional>
#include <vector>
#include <mutex>

typedef ButtonClickEventHandler FilterActiveChanged;

class WxControlBoard :
	public ImPopup
{
	ButtonClickEventHandler _startButtonClickHandler;
	mutable std::mutex _starStopButtonStrMutex;
	std::string _starStopButtonStr;
	std::vector<std::string> _players;
	int _player1;
	int _player2;
protected:
	virtual void updateContent();
public:
	WxControlBoard(const std::vector<std::string>& players);
	virtual ~WxControlBoard();

	void setOnStartStopButtonClickHandler(ButtonClickEventHandler&& handler);
	void setStarStopButtonText(const char* buttonText);

	const char* getPlayer1() const;
	const char* getPlayer2() const;
};

