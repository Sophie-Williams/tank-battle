#include "WxControlBoard.h"
#include "../common/Utility.h"
#include <sstream>

#define START_STOP_BUTTON_ID "StartStop"
#define PAUSE_RESUME_BUTTON_ID "PauseResume"

WxControlBoard::WxControlBoard()
{
	_window_flags |= ImGuiWindowFlags_NoTitleBar;
	_window_flags |= ImGuiWindowFlags_NoMove;
	_window_flags |= ImGuiWindowFlags_NoResize;
	_window_flags |= ImGuiWindowFlags_NoCollapse;
	_window_flags |= ImGuiWindowFlags_NoScrollbar;

	setStarStopButtonText("");
}


WxControlBoard::~WxControlBoard()
{
}

void WxControlBoard::update() {
	ImGui::SetNextWindowSize(_window_size, ImGuiCond_Always);
	ImGui::SetNextWindowPos(_window_pos);

	if (!ImGui::Begin("Control board", nullptr, _window_flags))
	{
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}

	ImGui::Columns(2, nullptr, false);
	ImGui::SetColumnWidth(0, 250);
	ImGui::SetColumnWidth(1, 150); 

	ImGui::CollapsingHeader("Game Settings", ImGuiTreeNodeFlags_Leaf);
	ImGui::SliderInt("bot", &_numberOfBot, 0, 10);
	ImGui::SliderInt("tank health", &_tankHeathCapacity, 20, 1000);
	ImGui::SliderInt("round", &_round, 1, 101);

	ImGui::CollapsingHeader("Players", ImGuiTreeNodeFlags_Leaf);

	int oldPlayer1 = _player1;
	showPlayers("Player 1", _player1);
	if (oldPlayer1 != _player1 && _player1ChangedHander) {
		_player1ChangedHander(this);
	}

	int oldPlayer2 = _player2;
	showPlayers("Player 2", _player2);
	if (oldPlayer2 != _player2 && _player2ChangedHander) {
		_player2ChangedHander(this);
	}

	ImGui::NextColumn();
	if (ImGui::Button("Generate tanks", ImVec2(130, 35))) {
		if (_generateButtonClickHandler) {
			_generateButtonClickHandler(this);
		}
	}

	if (ImGui::Button(_starStopButtonStr.c_str(), ImVec2(130, 35))) {
		if (_startButtonClickHandler) {
			_startButtonClickHandler(this);
		}
	}

	if (ImGui::Button(_pauseResumeButtonStr.c_str(), ImVec2(130, 35))) {
		if (_pauseButtonClickHandler) {
			_pauseButtonClickHandler(this);
		}
	}

	if (_showCompileButton && ImGui::Button("Compile Script", ImVec2(130, 35))) {
		if (_compileButtonClickHandler) {
			_compileButtonClickHandler(this);
		}
	}
	//ImGui::NextColumn();
	//ImGui::CollapsingHeader("Game statistics", ImGuiTreeNodeFlags_Leaf);
	//if (ImGui::TreeNode("Sumary")) {
	//	ImGui::Text("Player"); ImGui::SameLine();
	//	ImGui::Text("Win"); ImGui::SameLine();
	//	ImGui::Text("Time Per frame");

	//	ImGui::Text("Time Per frame");
	//	ImGui::TreePop();
	//}
	
	ImGui::Columns(1);

	ImGui::End();
}

void WxControlBoard::showPlayers(const char* label, int& selected) {
	const char* preview = "";
	if (selected < (int)_players.size()) {
		preview = _players[selected].c_str();
	}
	if (ImGui::BeginCombo(label, preview)) {
		for (int i = 0; i < (int)_players.size(); i++)
		{
			bool is_selected = selected == i;
			if (ImGui::Selectable(_players[i].c_str(), is_selected))
				selected = i;
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
}

void WxControlBoard::setOnStartStopButtonClickHandler(ButtonClickEventHandler&& handler) {
	_startButtonClickHandler = handler;
}

void WxControlBoard::setOnPauseResumeClickHandler(ButtonClickEventHandler&& handler) {
	_pauseButtonClickHandler = handler;
}

void WxControlBoard::setOnGenerateClickHandler(ButtonClickEventHandler&& handler) {
	_generateButtonClickHandler = handler;
}

void WxControlBoard::setOnCompileButtonClickHandler(ButtonClickEventHandler&& handler) {
	_compileButtonClickHandler = handler;
}

void WxControlBoard::setOnPlayer1ChangedHandler(SelectedItemChangedHandler&& handler) {
	_player1ChangedHander = handler;
}

void WxControlBoard::setOnPlayer2ChangedHandler(SelectedItemChangedHandler&& handler) {
	_player2ChangedHander = handler;
}

void WxControlBoard::setStarStopButtonText(const char* buttonText) {
	_starStopButtonStr = buttonText;
	_starStopButtonStr.append("##" START_STOP_BUTTON_ID);
}

void WxControlBoard::setPauseResumeButtonText(const char* buttonText) {
	_pauseResumeButtonStr = buttonText;
	_pauseResumeButtonStr.append("##" PAUSE_RESUME_BUTTON_ID);
}

void WxControlBoard::setPlayers(const std::vector<std::string>& players) {
	_players = players;
	if (_players.size() > 0 && _player1 < 0) {
		_player1 = 0;
	}
	if (_players.size() > 1 && _player2 < 0) {
		_player2 = 1;
	}
	if (_players.size() > 0 && _player2 < 0) {
		_player2 = 0;
	}
}

const std::vector<std::string>& WxControlBoard::getPlayers() const {
	return _players;
}

int WxControlBoard::getPlayer1() const {
	return _player1;
}

int WxControlBoard::getPlayer2() const {
	return _player2;
}

const std::string& WxControlBoard::getPlayerName(int playerIdx) const {
	return _players[playerIdx];
}

int WxControlBoard::getNumberOfBot() const {
	return _numberOfBot;
}

int WxControlBoard::getTankHeathCapacity() const {
	return _tankHeathCapacity;
}

int WxControlBoard::getRoundCount() const {
	return _round;
}

void WxControlBoard::showCompileButton(bool show) {
	_showCompileButton = show;
}