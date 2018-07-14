#include "WxPlayerInfo.h"


static int playerId = 0;

WxPlayerInfo::WxPlayerInfo(const std::string& playerName)
{
	_window_flags |= ImGuiWindowFlags_NoTitleBar;
	_window_flags |= ImGuiWindowFlags_NoMove;
	_window_flags |= ImGuiWindowFlags_NoResize;
	_window_flags |= ImGuiWindowFlags_NoCollapse;
	_window_flags |= ImGuiWindowFlags_NoScrollbar;
	_window_flags |= ImGuiWindowFlags_NoSavedSettings;
	_window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

	playerId++;

	_playerIdStr = playerName;
	_playerIdStr += "##";
	_playerIdStr += std::to_string(playerId);
}

WxPlayerInfo::~WxPlayerInfo()
{
}

void WxPlayerInfo::update() {
	ImGui::SetNextWindowSize(_window_size, ImGuiCond_Always);
	ImGui::SetNextWindowPos(_window_pos);

	// current control align base on the control window at 130 width length
	int widthAutoFilledLength = _window_size.x - 130;

	if (!ImGui::Begin(_playerIdStr.c_str(), nullptr, _window_flags))
	{
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}


	ImGui::End();
}