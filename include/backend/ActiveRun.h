#pragma once
#include "PlayerInfo.h"
#include "Shop.h"

class ActiveRun {
   private:
	PlayerInfo player;
	Shop currentShop; 
	int currentRound;
	int roundsWon;

   public:
	void finishRoundSequence();
	int calcTargetScore() const;
	void resetRun(const PlayerInfo& p);
	PlayerInfo& getPlayer();
	Shop& getShop();
	int getCurrentRound() const;
};
