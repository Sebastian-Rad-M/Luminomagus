#pragma once
#include <cstdlib>
#include <iostream>
#include <limits>
#include <string>

#include "IPlayerPrompt.h"
#include "Shop.h"
class ActiveRun;
class RoundTracker;

enum class GameState { MAIN_MENU, DRAFT, COMBAT, SHOP, GAME_OVER };

class View : public IPlayerPrompt{
   private:
	void clearScreen();
	static void printSeparator(const std::string& title);

   public:
	static int readInt(int l, int h);
	
	static void showMainMenu(GameState& state, ActiveRun& activeRun);
	void showDraft(GameState& state, ActiveRun& activeRun);
	void showCombat(GameState& state, ActiveRun& activeRun, RoundTracker& combatRound,
					bool& playerWon);
	void showShop(GameState& state, ActiveRun& activeRun);
	static void showGameOver(bool playerWon, const ActiveRun& activeRun);
	 void loseTheGame();
	 int requestChoice(const std::string& promptText, int min, int max) override;
    char requestManaColor() override;
    void showMessage(const std::string& message) override;
};
