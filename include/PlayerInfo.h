#pragma once
#include "Card.h"
#include "CardZone.h"
#include "RelicZone.h"
class PlayerInfo {
   private:
	int gold;
	CardZone deck;
	RelicZone relics;

   public:
	PlayerInfo() : gold(100) {}	 // initial gold for testing, make 10 or someshit
	explicit PlayerInfo(int gold) : gold(gold) {}

	~PlayerInfo() = default;

	int getGold() const;

	void addGold(const int amount);
	bool spendGold(const int amount);
	CardZone& getDeck();
	RelicZone& getRelicZone();

	// copy =opp
	PlayerInfo(const PlayerInfo& other);
	PlayerInfo& operator=(const PlayerInfo& other);
	friend std::ostream& operator<<(std::ostream& os, const PlayerInfo& player);
};
