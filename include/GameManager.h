#pragma once
#include <optional>

#include "ActiveRun.h"
#include "GameState.h"
#include "RoundTracker.h"

class GameManager {
   private:
	GameManager() = default;
	ActiveRun activeRun;
	std::optional<RoundTracker> round;
	GameState state = GameState::MAIN_MENU;
	bool playerWon = false;

   public:
	static GameManager& instance();
	GameManager(const GameManager&) = delete;
	GameManager& operator=(const GameManager&) = delete;

	ActiveRun& getActiveRun() { return activeRun; }
	void setState(GameState newState) { state = newState; }
	GameState getState() const { return state; }
	std::optional<RoundTracker>& getRound() { return round; }
	bool getPlayerWon() const { return playerWon; }
	void setPlayerWon(bool won) { playerWon = won; }

	void run();
};
