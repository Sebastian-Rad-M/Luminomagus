#include "ActiveRun.h"
/// @brief give gold, increse round and generate shop stock
void ActiveRun::finishRoundSequence() {
	player.addGold(calcTargetScore() / 10);
	roundsWon++;
	currentRound++;
	currentShop.generateRandomStock();
}
/// @brief int 
/// @return  10* current round
int ActiveRun::calcTargetScore() const { return 10 + (currentRound - 1) * 10; }	 
void ActiveRun::resetRun(const PlayerInfo& p) {
	currentRound = 1;
	roundsWon = 0;
	player = p;
} 
int ActiveRun::getCurrentRound() const { return currentRound; }
PlayerInfo& ActiveRun::getPlayer() { return player; }
Shop& ActiveRun::getShop() { return currentShop; }
