#include <string>

#include "GameManager.h"
#include "backend/ActiveRun.h"
#include "backend/CardDatabase.h"
#include "backend/CardZone.h"
#include "backend/PlayerInfo.h"
#include "ui/MainMenuUI.h"

void MainMenuListener::ProcessEvent(Rml::Event& event) {
	std::string id = event.GetCurrentElement()->GetId();

	if (id == "btn-new-run") {
		GameManager::instance().getActiveRun().resetRun(PlayerInfo());

		// Add starting cards to the deck
		const std::string APPEND_COLORS[3] = {"red", "blue", "green"};
		auto& deck = GameManager::instance().getActiveRun().getPlayer().getDeck();
		auto& db = CardDatabase::getInstance();
		for (int i = 0; i < 2; i++) {
			for (size_t j = 0; j < 3; j++) {
				deck.addCard(db.createCard("c_basic_" + APPEND_COLORS[j]));
			}
		}
		GameManager::instance().setState(GameState::DRAFT);
	} else if (id == "btn-quit") {
		GameManager::instance().setState(GameState::QUIT_APP);
	} else if (id == "btn-post-restart") {
		GameManager::instance().getActiveRun().resetRun(PlayerInfo());

		// Add starting cards to the deck
		const std::string APPEND_COLORS[3] = {"red", "blue", "green"};
		auto& deck = GameManager::instance().getActiveRun().getPlayer().getDeck();
		auto& db = CardDatabase::getInstance();
		for (int i = 0; i < 2; i++) {
			for (size_t j = 0; j < 3; j++) {
				deck.addCard(db.createCard("c_basic_" + APPEND_COLORS[j]));
			}
		}
		GameManager::instance().setState(GameState::DRAFT);
	} else if (id == "btn-post-menu") {
		GameManager::instance().setState(GameState::MAIN_MENU);
	}
}
