#pragma once
#include "ImWidget.h"
#include <functional>
#include <vector>

typedef ButtonClickEventHandler FilterActiveChanged;

class WxControlBoard :
	public ImWidget
{
	ButtonClickEventHandler _startButtonClickHandler;
	ButtonClickEventHandler _pauseButtonClickHandler;
	ButtonClickEventHandler _generateButtonClickHandler;
	
	std::string _starStopButtonStr;
	std::string _pauseResumeButtonStr;
	std::vector<std::string> _players;

	int _numberOfBot = 4;
	int _tankHeathCapacity = 50;
	int _round = 1;
	int _player1 = -1;
	int _player2 = -1;

private:
	void showPlayers(const char* label, int& selected);

public:
	WxControlBoard();
	virtual ~WxControlBoard();

	virtual void update();

	void setOnStartStopButtonClickHandler(ButtonClickEventHandler&& handler);
	void setOnPauseResumeClickHandler(ButtonClickEventHandler&& handler);
	void setOnGenerateClickHandler(ButtonClickEventHandler&& handler);

	void setStarStopButtonText(const char* buttonText);
	void setPauseResumeButtonText(const char* buttonText);

	void setPlayers(const std::vector<std::string>& players);
	const std::vector<std::string>& getPlayers() const;
	const std::string& getPlayer1() const;
	const std::string& getPlayer2() const;

	int getNumberOfBot() const;
	int getTankHeathCapacity() const;
	int getRoundCount() const;
};

