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
	FUNCTON_LOG();

	ImGui::SetNextWindowSize(_window_size, ImGuiCond_Always);
	ImGui::SetNextWindowPos(_window_pos);

	if (!ImGui::Begin("#GameStatistics", nullptr, _window_flags))
	{
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}
	char buffer[128];
	sprintf(buffer, "Summary(%d round)", (int)_records.size());
	ImGui::CollapsingHeader("Game statistics", ImGuiTreeNodeFlags_Leaf);
	if (ImGui::TreeNode(buffer)) {
		ImGui::Columns(3, nullptr, false);
		ImGui::SetColumnWidth(0, 50);
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
			sprintf(buffer, "%d", record.wins);
			ImGui::Text(buffer); ImGui::NextColumn();
			if (_records.size()) {
				sprintf(buffer, "%.1f", (record.kills / (float)_records.size()));
			}
			else {
				sprintf(buffer, "0");
			}
			ImGui::Text(buffer); ImGui::NextColumn();
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
			ImGui::SetColumnWidth(0, 50);
			ImGui::SetColumnWidth(1, 40);
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
				sprintf(buffer, "%d", playerRecord.kills);
				ImGui::Text(buffer); ImGui::NextColumn();
				ImGui::Text("N/A"); ImGui::NextColumn();
			}

			ImGui::Columns(1);
			ImGui::TreePop();
		}
		ImGui::Separator();
	}
	ImGui::End();
}
