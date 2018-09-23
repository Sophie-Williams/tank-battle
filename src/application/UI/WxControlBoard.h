/******************************************************************
* File:        WxControlBoard.h
* Description: declare WxControlBoard class. This class is responsible
*              for showing command control and forwarding their event
*              to handler for processing.
* Author:      Vincent Pham
*
* Copyright (c) 2018 VincentPT.
** Distributed under the MIT License (http://opensource.org/licenses/MIT)
**
*
**********************************************************************/

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
	ButtonClickEventHandler _compileButtonClickHandler;
	SelectedItemChangedHandler _player1ChangedHander;
	SelectedItemChangedHandler _player2ChangedHander;
	
	std::string _starStopButtonStr;
	std::string _pauseResumeButtonStr;
	std::vector<std::string> _players;

	int _numberOfBot = 4;
	int _tankHeathCapacity = 50;
	int _round = 1;
	int _player1 = -1;
	int _player2 = -1;

	bool _showCompileButton = false;

private:
	void showPlayers(const char* label, int& selected);

public:
	WxControlBoard();
	virtual ~WxControlBoard();

	virtual void update();

	void setOnStartStopButtonClickHandler(ButtonClickEventHandler&& handler);
	void setOnPauseResumeClickHandler(ButtonClickEventHandler&& handler);
	void setOnGenerateClickHandler(ButtonClickEventHandler&& handler);
	void setOnCompileButtonClickHandler(ButtonClickEventHandler&& handler);
	void setOnPlayer1ChangedHandler(SelectedItemChangedHandler&& handler);
	void setOnPlayer2ChangedHandler(SelectedItemChangedHandler&& handler);

	void setStarStopButtonText(const char* buttonText);
	void setPauseResumeButtonText(const char* buttonText);

	void setPlayers(const std::vector<std::string>& players);
	const std::vector<std::string>& getPlayers() const;
	int getPlayer1() const;
	int getPlayer2() const;
	const std::string& getPlayerName(int) const;

	int getNumberOfBot() const;
	int getTankHeathCapacity() const;
	int getRoundCount() const;

	void showCompileButton(bool show);
};

