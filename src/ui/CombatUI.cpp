#include <string>

#include "GameManager.h"
#include "backend/ActiveRun.h"
#include "backend/LightPool.h"
#include "backend/PlayerInfo.h"
#include "backend/RelicZone.h"
#include "backend/RoundTracker.h"
#include "ui/CombatUI.h"
#include "ui/UIHelpers.h"

void CombatListener::startCombat(Rml::ElementDocument* doc) {
	combatDoc = doc;
	menuOpen = false;
	pendingXValue = 0;
	activePileView = PileView::NONE;
	GameManager::instance().getRound().value().setPromptMessage("");
	if (cardTemplateRML.empty()) {
		cardTemplateRML = UIHelpers::LoadFileToString("assets/card.rml");
	}
	if (relicTemplateRML.empty()) {
		relicTemplateRML = UIHelpers::LoadFileToString("assets/relic.rml");
	}
	refreshUI();
	// light
	if (auto el = combatDoc->GetElementById("mana-red")) {
		el->AddEventListener(Rml::EventId::Click, this);
	}
	if (auto el = combatDoc->GetElementById("mana-blue")) {
		el->AddEventListener(Rml::EventId::Click, this);
	}
	if (auto el = combatDoc->GetElementById("mana-green")) {
		el->AddEventListener(Rml::EventId::Click, this);
	}

	// ☰ menu
	if (auto el = combatDoc->GetElementById("btn-combat-menu")) {
		el->AddEventListener(Rml::EventId::Click, this);
	}

	// X prompt
	if (auto el = combatDoc->GetElementById("btn-x-minus")) {
		el->AddEventListener(Rml::EventId::Click, this);
	}
	if (auto el = combatDoc->GetElementById("btn-x-plus")) {
		el->AddEventListener(Rml::EventId::Click, this);
	}
	if (auto el = combatDoc->GetElementById("btn-x-confirm")) {
		el->AddEventListener(Rml::EventId::Click, this);
	}

	// Yes/No
	if (auto el = combatDoc->GetElementById("btn-yes")) {
		el->AddEventListener(Rml::EventId::Click, this);
	}
	if (auto el = combatDoc->GetElementById("btn-no")) {
		el->AddEventListener(Rml::EventId::Click, this);
	}

	// Pause-menu overlay
	if (auto el = combatDoc->GetElementById("btn-menu-resume")) {
		el->AddEventListener(Rml::EventId::Click, this);
	}
	if (auto el = combatDoc->GetElementById("btn-menu-restart")) {
		el->AddEventListener(Rml::EventId::Click, this);
	}
	if (auto el = combatDoc->GetElementById("btn-menu-quit")) {
		el->AddEventListener(Rml::EventId::Click, this);
	}

	// Pile Viewer
	if (auto el = combatDoc->GetElementById("void-section")) {
		el->AddEventListener(Rml::EventId::Click, this);
	}
	if (auto el = combatDoc->GetElementById("graveyard-section")) {
		el->AddEventListener(Rml::EventId::Click, this);
	}
	if (auto el = combatDoc->GetElementById("btn-close-pile-view")) {
		el->AddEventListener(Rml::EventId::Click, this);
	}
}

void CombatListener::refreshUI() {
	if (!combatDoc || !GameManager::instance().getRound().has_value()) {
		return;
	}
	auto& round = GameManager::instance().getRound().value();
	auto& activeRun = GameManager::instance().getActiveRun();

	// Top prompts
	if (auto el = combatDoc->GetElementById("prompt-line-1")) {
		el->SetInnerRML("Cycle " + std::to_string(activeRun.getCurrentRound()));
	}

	// Set the prompt message (this fixes your old bug where Score was overwritten by prompts)
	if (auto el = combatDoc->GetElementById("boss-name-display")) {
		std::string bossName = round.getActiveBossName();
		if (bossName.empty()) {
			el->SetInnerRML("THE VOID SLUMBERS");
		} else {
			el->SetInnerRML(bossName);
		}
	}
	if (auto el = combatDoc->GetElementById("boss-desc-display")) {
		std::string bossDesc = round.getActiveBossDescription();
		if (bossDesc.empty()) {
			el->SetInnerRML("Only the creeping dark remains. Hold it back.");
		} else {
			el->SetInnerRML(bossDesc);
		}
	}
	// Populate the new Status Panel!
	if (auto el = combatDoc->GetElementById("count-echo")) {
		el->SetInnerRML(std::to_string(round.getStormCount()));
	}
	if (auto el = combatDoc->GetElementById("count-score")) {
		el->SetInnerRML(std::to_string(round.getCurrentScore()) + "/" +
						std::to_string(round.getTargetScore()));
	}
	if (auto el = combatDoc->GetElementById("count-deck")) {
		el->SetInnerRML(std::to_string(round.getDeck().getSize()));
	}
	// --- Mana / X prompt / YesNo overlays ---
	if (auto overlay = combatDoc->GetElementById("inline-mana-prompt")) {
		overlay->SetProperty("display", (round.getPendingManaChoices() > 0 &&
										 !round.isXPromptActive() && !round.isYesNoPromptActive())
											? "block"
											: "none");
	}

	// Hand click prompt (discard / select from hand)
	if (auto el = combatDoc->GetElementById("prompt-line-1")) {
		el->SetInnerRML("Cycle " + std::to_string(activeRun.getCurrentRound()) + " // DECAY");
	}

	// THIS IS THE FIX: Route the prompts into the heavy center text!
	if (auto el = combatDoc->GetElementById("prompt-line-2")) {
		if (round.isHandClickActive()) {
			el->SetInnerRML(round.getHandClickMessage());
		} else if (round.isYesNoPromptActive()) {
			el->SetInnerRML(round.getYesNoMessage());
		} else if (round.isXPromptActive() || round.getPendingManaChoices() > 0) {
			el->SetInnerRML("Manifesting Energy...");  // Generic fallback for box-prompts
		} else {
			el->SetInnerRML("");  // Clear when no prompt
		}
	}

	// THIS is the block that makes the Codex prompt appear!
	if (auto overlay = combatDoc->GetElementById("inline-x-prompt")) {
		overlay->SetProperty("display", (round.isXPromptActive() && !round.isYesNoPromptActive())
											? "block"
											: "none");
		if (round.isXPromptActive()) {
			if (auto xVal = combatDoc->GetElementById("x-value-display")) {
				xVal->SetInnerRML(std::to_string(pendingXValue));
			}
		}
	}

	if (auto overlay = combatDoc->GetElementById("inline-yesno-prompt")) {
		overlay->SetProperty("display", round.isYesNoPromptActive() ? "block" : "none");
		if (round.isYesNoPromptActive()) {
			if (auto msg = combatDoc->GetElementById("yesno-message")) {
				msg->SetInnerRML(round.getYesNoMessage());
			}
		}
	}

	// Populate the Boss Info directly into the right-side panel
	if (auto el = combatDoc->GetElementById("boss-name-display")) {
		std::string bossName = round.getActiveBossName();
		if (bossName.empty()) {
			el->SetInnerRML("No Boss Active");
		} else {
			el->SetInnerRML(bossName);
		}
	}
	if (auto el = combatDoc->GetElementById("boss-desc-display")) {
		std::string bossDesc = round.getActiveBossDescription();
		if (bossDesc.empty()) {
			el->SetInnerRML("Standard rules apply this round.");
		} else {
			el->SetInnerRML(bossDesc);
		}
	}
	// ── Pile View / Selection Modal ───────────────────────────────────────────
	bool showSelection = round.isCardSelectionPromptActive();
	bool showPile = activePileView != PileView::NONE;

	if (auto el = combatDoc->GetElementById("pile-view-bg")) {
		el->SetProperty("display", (showSelection || showPile) ? "block" : "none");
		if (showSelection || showPile) {
			auto title = combatDoc->GetElementById("pile-view-title");
			auto container = combatDoc->GetElementById("pile-view-cards");
			if (title && container) {
				std::string html;
				const std::vector<std::shared_ptr<Card>>* cards = nullptr;

				if (showSelection) {
					title->SetInnerRML(round.getSelectionMessage());
					cards = &round.getSelectionChoices();
				} else if (activePileView == PileView::GRAVEYARD) {
					title->SetInnerRML("DISCARD");
					cards = &round.getGraveyard().getCards();
				} else {
					title->SetInnerRML("VOID");
					cards = &round.getExile().getCards();
				}

				for (size_t i = 0; i < cards->size(); i++) {
					std::string idStr =
						showSelection ? " id=\"select-card-" + std::to_string(i) + "\"" : "";

					std::string styleStr = " style=\"width: 264px; height: 368px;\"";

					html += "<div class=\"pile-card-wrapper\"" + idStr + styleStr + ">";
					html += UIHelpers::GenerateCardRML(cardTemplateRML, *(*cards)[i]);
					html += "</div>";
				}
				container->SetInnerRML(html);
				if (showSelection) {
					for (size_t i = 0; i < cards->size(); i++) {
						if (auto btn =
								combatDoc->GetElementById("select-card-" + std::to_string(i))) {
							btn->AddEventListener(Rml::EventId::Click, this);
						}
					}
				}
			}
		}
	}

	// ── Hand ──────────────────────────────────────────────────────────────────
	if (auto handContainer = combatDoc->GetElementById("hand-container")) {
		const auto& handCards = round.getHand().getCards();
		std::string handHtml;
		for (size_t i = 0; i < handCards.size(); i++) {
			std::string cardVisuals = UIHelpers::GenerateCardRML(cardTemplateRML, *handCards[i]);
			handHtml += "<div class=\"card-wrapper\" id=\"hand-card-" + std::to_string(i) + "\">" +
						cardVisuals + "</div>";
		}
		handContainer->SetInnerRML(handHtml);
		for (size_t i = 0; i < handCards.size(); i++) {
			if (auto btn = combatDoc->GetElementById("hand-card-" + std::to_string(i))) {
				btn->AddEventListener(Rml::EventId::Click, this);
			}
		}
	}

	// ── Relics ────────────────────────────────────────────────────────────────
	if (auto relicContainer = combatDoc->GetElementById("relic-container")) {
		const auto& relics = activeRun.getPlayer().getRelicZone().getRelicZone();
		std::string relicHtml = "";

		for (size_t i = 0; i < relics.size(); i++) {
			std::string singleRelic = relicTemplateRML;

			// Replace our template tags with actual data
			singleRelic = UIHelpers::ReplaceAll(singleRelic, "[INDEX]", std::to_string(i));
			singleRelic = UIHelpers::ReplaceAll(singleRelic, "[NAME]", relics[i]->getName());
			singleRelic = UIHelpers::ReplaceAll(singleRelic, "[DESC]", relics[i]->getDescription());

			// Handle the activatable visual state
			std::string activeClass = relics[i]->isActivatable() ? "relic-activatable" : "";
			singleRelic = UIHelpers::ReplaceAll(singleRelic, "[ACTIVATABLE_CLASS]", activeClass);

			relicHtml += singleRelic;
		}

		relicContainer->SetInnerRML(relicHtml);

		// Attach event listeners (This remains exactly the same!)
		for (size_t i = 0; i < relics.size(); i++) {
			if (auto btn = combatDoc->GetElementById("relic-" + std::to_string(i))) {
				btn->AddEventListener(Rml::EventId::Click, this);
			}
			if (auto sellBtn = combatDoc->GetElementById("sell-relic-" + std::to_string(i))) {
				sellBtn->AddEventListener(Rml::EventId::Click, this);
			}
		}
	}
	// ── Graveyard and Void ────────────────────────────────────────────────────
	if (auto graveyardContainer = combatDoc->GetElementById("graveyard-section")) {
		const auto& cards = round.getGraveyard().getCards();
		if (!cards.empty()) {
			std::string cardVisuals = UIHelpers::GenerateCardRML(cardTemplateRML, *cards.back());
			// std::cout << cardTemplateRML << "\n\n\n";
			// std::cout << cardVisuals << "\n\n\n";
			graveyardContainer->SetInnerRML("<div class=\"small-card-wrapper\">" + cardVisuals +
											"</div>");
			if (auto title = combatDoc->GetElementById("graveyard-title")) {
				title->SetProperty("display", "none");
			}
		} else {
			graveyardContainer->SetInnerRML("");
			if (auto title = combatDoc->GetElementById("graveyard-title")) {
				title->SetProperty("display", "block");
			}
		}
	}

	if (auto voidContainer = combatDoc->GetElementById("void-container")) {
		const auto& cards = round.getExile().getCards();
		if (!cards.empty()) {
			std::string cardVisuals = UIHelpers::GenerateCardRML(cardTemplateRML, *cards.back());

			voidContainer->SetInnerRML("<div class=\"small-card-wrapper\">" + cardVisuals +
									   "</div>");
			if (auto title = combatDoc->GetElementById("void-title")) {
				title->SetProperty("display", "none");
			}
		} else {
			voidContainer->SetInnerRML("");
			if (auto title = combatDoc->GetElementById("void-title")) {
				title->SetProperty("display", "block");
			}
		}
	}

	// ── Light Pool ────────────────────────────────────────────────────────────
	const auto& lightPool = round.getManaPool();
	if (auto el = combatDoc->GetElementById("count-light-red")) {
		el->SetInnerRML(std::to_string(lightPool.getRed()));
	}
	if (auto el = combatDoc->GetElementById("count-light-blue")) {
		el->SetInnerRML(std::to_string(lightPool.getBlue()));
	}
	if (auto el = combatDoc->GetElementById("count-light-green")) {
		el->SetInnerRML(std::to_string(lightPool.getGreen()));
	}
}

// ─────────────────────────────────────────────────────────────────────────────
//  Menu toggle
// ─────────────────────────────────────────────────────────────────────────────
void CombatListener::toggleMenu() {
	if (!combatDoc) {
		return;
	}
	menuOpen = !menuOpen;
	if (auto overlay = combatDoc->GetElementById("combat-menu-overlay")) {
		overlay->SetProperty("display", menuOpen ? "block" : "none");
	}
}

// ─────────────────────────────────────────────────────────────────────────────
//  ProcessEvent
// ─────────────────────────────────────────────────────────────────────────────
void CombatListener::ProcessEvent(Rml::Event& event) {
	std::string id = event.GetCurrentElement()->GetId();
	auto& roundOpt = GameManager::instance().getRound();

	// ── Pause Menu ────────────────────────────────────────────────────────────
	if (id == "btn-combat-menu") {
		toggleMenu();
		return;
	}
	if (id == "btn-menu-resume") {
		toggleMenu();  // close it
		return;
	}
	if (id == "btn-menu-restart") {
		// Full restart → back to main menu; GameManager::run() will reset state
		GameManager::instance().setPlayerWon(false);
		GameManager::instance().setState(GameState::MAIN_MENU);
		return;
	}
	if (id == "btn-menu-quit") {
		GameManager::instance().setState(GameState::MAIN_MENU);
		return;
	}

	// ── Pile Viewer Overlay ───────────────────────────────────────────────────
	if (id == "void-section") {
		activePileView = PileView::VOID;
		refreshUI();
		return;
	}
	if (id == "graveyard-section") {
		activePileView = PileView::GRAVEYARD;
		refreshUI();
		return;
	}
	if (id == "btn-close-pile-view") {
		if (!roundOpt.value().isCardSelectionPromptActive()) {
			activePileView = PileView::NONE;
			refreshUI();
		}
		return;
	}

	// ── Card Selection Modal ──────────────────────────────────────────────────
	if (id.find("select-card-") == 0) {
		int index = std::stoi(id.substr(12));
		roundOpt.value().resolveCardSelection(index);
		checkWinCondition();
		checkLossCondition();
		refreshUI();
		return;
	}

	// Don't process game events while menu or a blocking modal is open
	if (menuOpen || roundOpt.value().isCardSelectionPromptActive()) {
		return;
	}

	// If the user takes a valid game action while the pile viewer is open, auto-close it!
	if (activePileView != PileView::NONE) {
		activePileView = PileView::NONE;
	}

	if (!roundOpt.has_value()) {
		return;
	}
	auto& round = roundOpt.value();
	auto& activeRun = GameManager::instance().getActiveRun();

	// ── Relic mouse events ────────────────────────────────────────────────────
	if (id.find("relic-") == 0 && id.find("wrapper") == std::string::npos &&
		id.find("container") == std::string::npos) {
		int index = std::stoi(id.substr(6));

		// Click: activate if activatable
		if (event.GetId() == Rml::EventId::Click) {
			const auto& relics = activeRun.getPlayer().getRelicZone().getRelicZone();
			if (index >= 0 && index < static_cast<int>(relics.size())) {
				if (relics[index]->isActivatable()) {
					relics[index]->activate(round);
					checkWinCondition();
					checkLossCondition();
					refreshUI();
				}
			}
			return;
		}
	}

	// ── Sell Relic (from tooltip) ─────────────────────────────────────────────
	if (id.find("sell-relic-") == 0) {
		int idx = std::stoi(id.substr(11));
		// Give 2 mana of player's choice (as before, but was skipped in my rewrite by mistake?
		// Adding it here).
		round.requestManaChoices(2);

		round.getRelicZone().removeRelic(idx);
		activeRun.getPlayer().getRelicZone().removeRelic(idx);
		refreshUI();
		return;
	}

	// ── Play a Card (or resolve a hand-click prompt) ─────────────────────────
	if (id.find("hand-card-") == 0) {
		int index = std::stoi(id.substr(10));
		if (round.isHandClickActive()) {
			round.resolveHandClick(index);
		} else {
			round.playCardFromHand(index);
		}
		checkWinCondition();
		checkLossCondition();
		refreshUI();
		return;
	}

	// ── X Selection ───────────────────────────────────────────────────────────
	if (id == "btn-x-minus") {
		if (pendingXValue > 0) {
			pendingXValue--;
		}
		refreshUI();
		return;
	}
	if (id == "btn-x-plus") {
		int maxMana = round.getManaPool().getTotalMana();
		if (pendingXValue < maxMana) {
			pendingXValue++;
		}
		refreshUI();
		return;
	}
	if (id == "btn-x-confirm") {
		round.resolveXPrompt(pendingXValue);
		pendingXValue = 0;
		checkWinCondition();
		checkLossCondition();
		refreshUI();
		return;
	}

	// ── Yes/No Selection ──────────────────────────────────────────────────────
	if (id == "btn-yes") {
		round.resolveYesNoPrompt(true);
		checkWinCondition();
		checkLossCondition();
		refreshUI();
		return;
	}
	if (id == "btn-no") {
		round.resolveYesNoPrompt(false);
		checkWinCondition();
		checkLossCondition();
		refreshUI();
		return;
	}

	// ── Mana Selection ────────────────────────────────────────────────────────
	if (id.find("mana-") == 0) {
		std::string color = id.substr(5);
		if (color == "red") {
			round.addManaByColor('R');
		} else if (color == "blue") {
			round.addManaByColor('B');
		} else if (color == "green") {
			round.addManaByColor('G');
		}
		refreshUI();
		return;
	}
}

// ─────────────────────────────────────────────────────────────────────────────
//  Win / Loss checks
// ─────────────────────────────────────────────────────────────────────────────
void CombatListener::checkWinCondition() {
	auto& round = GameManager::instance().getRound().value();
	if (round.isRoundWon()) {
		round.getRelicZone().triggerOnRoundEnd(round);
		GameManager::instance().getActiveRun().finishRoundSequence();
		GameManager::instance().getActiveRun().getShop().generateRandomStock();
		GameManager::instance().setState(GameState::SHOP);
	}
}

void CombatListener::checkLossCondition() {
	auto& round = GameManager::instance().getRound().value();
	if (round.hasLost()) {
		GameManager::instance().setPlayerWon(false);
		GameManager::instance().setState(GameState::POST_RUN);
	}
}
