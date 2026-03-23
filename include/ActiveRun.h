#pragma once
#include "PlayerInfo.h"
#include "Shop.h"

class ActiveRun {
   private:
	PlayerInfo player;
	Shop currentShop; //TODO:move shop from here
	int currentRound=0;
	int roundsWon=0;

   public:
	void finishRoundSequence();
	int calcTargetScore() const;
	void resetRun(const PlayerInfo& p);
	PlayerInfo& getPlayer();
	Shop& getShop();
	void printStatus() const;
	int getCurrentRound() const;
};
