#pragma once
#include "PlayerInfo.h"
#include "Shop.h"

class ActiveRun {
   private:
	PlayerInfo player;
	Shop currentShop;
	int currentRound = 1;
	int roundsWon = 0;

   public:
	void finishRoundSequence();
	int calcTargetScore() const;
	void resetRun(PlayerInfo p);
	PlayerInfo& getPlayer();
	Shop& getShop();
	void printStatus() const;
	int getCurrentRound() const;
};
