#include "GameManager.h"

GameManager& GameManager::instance() {
	static GameManager inst;
	return inst;
}

void GameManager::run() {
	state = GameState::MAIN_MENU;
	while (state != GameState::GAME_OVER) {
		switch (state) {
			case GameState::MAIN_MENU:
				view.showMainMenu(state, activeRun);
				break;
			case GameState::DRAFT:
				view.showDraft(state, activeRun);
				round.emplace(activeRun, &view);
				round->setupDeck(activeRun.getPlayer().getDeck(),
								 activeRun.getPlayer().getRelicZone());
				round->startNewRound();
				 break;

			case GameState::COMBAT:
				view.showCombat(state, activeRun, round.value(), playerWon);
				break;

			case GameState::SHOP:
				view.showShop(state, activeRun);
				round.emplace(activeRun, &view);
				if (state == GameState::COMBAT) { 
					round.emplace(activeRun, &view);
					round->setupDeck(activeRun.getPlayer().getDeck(),
									 activeRun.getPlayer().getRelicZone());
					round->startNewRound();
				}
				break;

			default:
				break;
		}
	}

	view.showGameOver(playerWon, activeRun);
}
