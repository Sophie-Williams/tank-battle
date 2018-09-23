/******************************************************************
* File:        WxGameStatistics.cpp
* Description: implement WxGameStatistics class. This class is responsible
*              for showing result of matches in game.
* Author:      Vincent Pham
*
* Copyright (c) 2018 VincentPT.
** Distributed under the MIT License (http://opensource.org/licenses/MIT)
**
*
**********************************************************************/

#include "WxGameStatistics.h"

WxGameStatistics::WxGameStatistics()
{
	_window_flags |= ImGuiWindowFlags_NoTitleBar;
	_window_flags |= ImGuiWindowFlags_NoMove;
	_window_flags |= ImGuiWindowFlags_NoResize;
	_window_flags |= ImGuiWindowFlags_NoCollapse;
	_window_flags |= ImGuiWindowFlags_NoScrollbar;
}


WxGameStatistics::~WxGameStatistics()
{
}

std::list<RoundRecord>& WxGameStatistics::records() {
	return _records;
}

void WxGameStatistics::setPlayers(const std::vector<std::string>& players) {
	_players = players;
}

const std::vector<std::string>& WxGameStatistics::getPlayers() const {
	return _players;
}

void WxGameStatistics::addPlayer(const std::string& player) {
	_players.push_back(player);
}

void WxGameStatistics::clearPlayers() {
	_players.clear();
	_records.clear();
}

struct SummaryRecord {
	int kills;
	int wins;
	SummaryRecord() : wins(0), kills(0) {

	}
};

void WxGameStatistics::update() {
	ImGui::SetNextWindowSize(_window_size, ImGuiCond_Always);
	ImGui::SetNextWindowPos(_window_pos);

	if (!ImGui::Begin("#GameStatistics", nullptr, _window_flags))
	{
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}
	char buffer[128];
	ImGui::CollapsingHeader("Game statistics", ImGuiTreeNodeFlags_Leaf);
	if (ImGui::TreeNode("Summary")) {
		ImGui::Columns(3, nullptr, false);
		ImGui::SetColumnWidth(0, 150);
		ImGui::SetColumnWidth(1, 30);
		ImGui::Text("Player"); ImGui::NextColumn();
		ImGui::Text("Win"); ImGui::NextColumn();
		ImGui::Text("Average kills"); ImGui::NextColumn();

		std::vector<SummaryRecord> summaryRecords(_players.size());

		for (auto it = _records.begin(); it != _records.end(); it++) {
			if (it->winner >= 0) {
				summaryRecords[it->winner].wins++;
			}
			auto& playerRecords = it->playerRecords;

			for (auto jt = playerRecords.begin(); jt != playerRecords.end(); jt++) {
				summaryRecords[jt->playerIdx].kills += jt->kills;
			}
		}
		static int selected = -1;
		for (int i = 0; i < (int)_players.size(); i++) {
			auto& record = summaryRecords[i];
			sprintf(buffer, "%s##%d", _players[i].c_str(),  i);
			if (ImGui::Selectable(buffer, selected == i, ImGuiSelectableFlags_SpanAllColumns)) {
				selected = i;
			}
			ImGui::NextColumn();
			ImGui::Text( "%d", record.wins); ImGui::NextColumn();
			if (_records.size()) {
				ImGui::Text("%.1f", (record.kills / (float)_records.size()));
			}
			else {
				ImGui::Text( "0");
			}
			ImGui::NextColumn();
		}

		ImGui::Columns(1);
		ImGui::TreePop();
	}
	ImGui::Separator();
	int roundIdx = (int)_records.size();
	for (auto it = _records.rbegin(); it != _records.rend(); it++, roundIdx--) {
		auto& roundRecord = *it;
		if (roundRecord.winner >= 0) {
			sprintf(buffer, "round %d(%s)", roundIdx, _players[roundRecord.winner].c_str());
		}
		else {
			sprintf(buffer, "round %d", roundIdx);
		}

		if (ImGui::TreeNode(buffer)) {
			ImGui::Columns(3, nullptr, false);
			ImGui::SetColumnWidth(0, 150);
			ImGui::SetColumnWidth(1, 30);
			ImGui::Text("Player"); ImGui::NextColumn();
			ImGui::Text("Kill"); ImGui::NextColumn();
			ImGui::Text("Time per frame"); ImGui::NextColumn();

			auto& playerRecords = roundRecord.playerRecords;
			int id = 1;
			for (auto jt = playerRecords.begin(); jt != playerRecords.end(); jt++, id++) {
				auto& playerRecord = *jt;

				sprintf(buffer, "%s##%d", _players[playerRecord.playerIdx].c_str(), id);
				ImGui::Selectable(buffer, false, ImGuiSelectableFlags_SpanAllColumns);
				ImGui::NextColumn();				
				ImGui::Text("%d", playerRecord.kills); ImGui::NextColumn();
				if (playerRecord.averageTimePerFrame < 0) {
					ImGui::Text("N/A");
				}
				else {
					ImGui::Text("%.3f us", (playerRecord.averageTimePerFrame * 1000));
				}
				ImGui::NextColumn();
			}

			ImGui::Columns(1);
			ImGui::TreePop();
		}
		ImGui::Separator();
	}
	ImGui::End();
}
