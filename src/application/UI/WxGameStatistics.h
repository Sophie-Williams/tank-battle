#pragma once
#include "ImWidget.h"
#include <vector>
#include <string>
#include <list>

struct PlayerRecord {
	int playerIdx;
	int kills;
	float averageTimePerFrame;
};

struct RoundRecord {
	int winner;
	std::list<PlayerRecord> playerRecords;
};

class WxGameStatistics :
	public ImWidget
{
	std::list<RoundRecord> _records;
	std::vector<std::string> _players;
public:
	WxGameStatistics();
	virtual ~WxGameStatistics();

	virtual void update();

	std::list<RoundRecord>& records();
	void setPlayers(const std::vector<std::string>& players);
	const std::vector<std::string>& getPlayers() const;
	void addPlayer(const std::string& player);
	void clearPlayers();
};

