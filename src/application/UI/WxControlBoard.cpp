#include "WxControlBoard.h"
#include "../common/Utility.h"
#include <sstream>


#define START_STOP_BUTTON_ID "StartStop"

WxControlBoard::WxControlBoard(const std::vector<std::string>& players) : ImPopup("")
{
	_window_flags |= ImGuiWindowFlags_NoMove;
	_window_flags |= ImGuiWindowFlags_NoResize;
	_window_flags |= ImGuiWindowFlags_NoCollapse;
	_window_flags |= ImGuiWindowFlags_NoScrollbar;

	_players = players;
	if (_players.size()) {
		_player1 = 0;
		_player2 = _players.size() - 1;
	}
	else {
		_player1 = -1;
		_player2 = -1;
	}

	setStarStopButtonText("");
}

WxControlBoard::~WxControlBoard()
{
}

void WxControlBoard::updateContent() {
	FUNCTON_LOG();

	// current control align base on the control window at 130 width length
	int widthAutoFilledLength = _window_size.x - 130;

	_starStopButtonStrMutex.lock();
	auto tempButtonStr = _starStopButtonStr;
	_starStopButtonStrMutex.unlock();
	if (ImGui::Button(tempButtonStr.c_str(), ImVec2(120 + widthAutoFilledLength, 35))) {
		if (_startButtonClickHandler) {
			_startButtonClickHandler(this);
		}
	}
}

void WxControlBoard::setOnStartStopButtonClickHandler(ButtonClickEventHandler&& handler) {
	_startButtonClickHandler = handler;
}

void WxControlBoard::setStarStopButtonText(const char* buttonText) {
	std::unique_lock<std::mutex> lk(_starStopButtonStrMutex);
	_starStopButtonStr = buttonText;
	_starStopButtonStr.append("##" START_STOP_BUTTON_ID);
}

const char* WxControlBoard::getPlayer1() const {
	if (_player1 >= 0 && _player1 <= _players.size()) {
		return _players[_player1].c_str();
	}

	return nullptr;
}

const char* WxControlBoard::getPlayer2() const {
	if (_player1 >= 0 && _player1 <= _players.size()) {
		return _players[_player1].c_str();
	}

	return nullptr;
}
