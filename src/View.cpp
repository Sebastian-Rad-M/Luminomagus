#include "ActiveRun.h"
#include "CardDatabase.h"
#include "RoundTracker.h"
#include "View.h"

void View::clearScreen() {
#if defined(_WIN32)
	std::system("cls");	 // Windows command
#else
	std::system("clear");  // Linux/Mac command
#endif
}
void View::printSeparator(const std::string& title) {
	std::cout << "\n╔══════════════════════════════════╗\n"
			  << "║  " << title;
	const int boxWidth = 34;
	int padding = boxWidth - static_cast<int>(title.size()) - 2;
	for (int i = 0; i < padding; ++i) {
		std::cout << ' ';
	}
	std::cout << "║\n"
			  << "╚══════════════════════════════════╝\n";
}

int View::readInt(int l, int h) {
	int value;
	while (!(std::cin >> value) || value < l || value > h) {
		std::cin.clear();
		std::cin.ignore(10000, '\n');
		std::cout << "  Please enter a number between " << l << " and " << h << ": ";
	}
	return value;
}
void View::loseTheGame(){
	clearScreen();
	std::cout << "\n\n"; //is this tacky? yes, but i am bored
 	std::cout << "  ██╗   ██╗ ██████╗ ██╗   ██╗    ██████╗ ██╗███████╗██████╗ \n";
	std::cout << "  ╚██╗ ██╔╝██╔═══██╗██║   ██║    ██╔══██╗██║██╔════╝██╔══██╗\n";
	std::cout << "   ╚████╔╝ ██║   ██║██║   ██║    ██║  ██║██║█████╗  ██║  ██║\n";
	std::cout << "    ╚██╔╝  ██║   ██║██║   ██║    ██║  ██║██║██╔══╝  ██║  ██║\n";
	std::cout << "     ██║   ╚██████╔╝╚██████╔╝    ██████╔╝██║███████╗██████╔╝\n";
	std::cout << "     ╚═╝    ╚═════╝  ╚═════╝     ╚═════╝ ╚═╝╚══════╝╚═════╝ \n\n";
	std::cout << "  Press ENTER to accept your fate...\n";
	
	std::cin.clear();
	std::cin.ignore(10000, '\n');
	std::cin.get();
}
	


void View::showMainMenu(GameState& state, ActiveRun& activeRun) {
	printSeparator("MAIN MENU");
	std::cout << "  [1] New Run\n"
			  << "  [2] Quit\n"
			  << "  Choice: ";

	int choice = readInt(1, 2);

	if (choice == 1) {
		activeRun.resetRun(PlayerInfo());  ///!!
		const std::string APPEND_COLORS[3] = {"red", "blue", "green"};
		for (int i = 0; i < 2; i++) {
			for (size_t j = 0; j < 3; j++) {
				activeRun.getPlayer().getDeck().addCard(
					CardDatabase::getInstance().createCard("c_basic_" + APPEND_COLORS[j]));
			}
		}
		state = GameState::DRAFT;
	} else {
		state = GameState::GAME_OVER;
	}
}

void View::showDraft(GameState& state, ActiveRun& activeRun) {
	clearScreen();
	printSeparator("DRAFT PHASE");
	std::cout << "  Choose 5 cards from this pool of 10 to form your starting deck.\n\n";

	std::vector<std::shared_ptr<Card>> pool;
	for (int i = 0; i < 10; i++) {
		std::shared_ptr<Card> randomCard = CardDatabase::getInstance().getRandomCard();
		if (randomCard) {
			pool.push_back(randomCard);
		} else {
			std::cout << "  [ERROR] Database is empty! Cannot draft.\n";
			state = GameState::MAIN_MENU;
			return;
			/// try catch or error or some shi
		}
	}

	for (int pick = 0; pick < 5; pick++) {
		std::cout << "  --- Pick " << (pick + 1) << " of 5 ---\n";

		for (size_t i = 0; i < pool.size(); i++) {
			std::cout << "  [" << (i + 1) << "] " << *pool[i] << "\n";
		}

		std::cout << "  Choice: ";
		int choice = readInt(1, pool.size());

		std::shared_ptr<Card> chosenCard = pool[choice - 1];
		activeRun.getPlayer().getDeck().addCard(chosenCard);
		clearScreen();
		std::cout << "  --> Drafted " << chosenCard->getName() << "!\n\n";

		pool.erase(pool.begin() + (choice - 1));
	}

	std::cout << "  Draft complete! Your starting deck has "
			  << activeRun.getPlayer().getDeck().getSize() << " cards.\n";
	std::cout << "  Press ENTER to enter combat...\n";
	std::cin.ignore(10000, '\n');
	std::cin.get();
	state = GameState::COMBAT;
}
void View::showCombat(GameState& state, ActiveRun& activeRun, RoundTracker& combatRound,
					  bool& playerWon) {
	clearScreen();
	printSeparator("COMBAT - Round " + std::to_string(activeRun.getCurrentRound()));
	combatRound.printStatus();
	std::cout << "\n  --- EQUIPPED RELICS ---\n";
	const auto& equippedRelics = activeRun.getPlayer().getRelicZone().getRelicZone();

	if (equippedRelics.empty()) {
		std::cout << "  (None)\n";
	} else {
		int i = 1;
		for (const auto& relic : equippedRelics) {
			std::cout << i << "." << relic->getName() << "\n";
			i++;
		}
	}
	std::cout << "\n  --- YOUR HAND ---\n";
	const auto& handCards = combatRound.getHand().getCards();

	if (handCards.empty()) std::cout << "  (Hand is empty!)\n";
		else for (size_t i = 0; i < handCards.size(); i++) std::cout << "  [" << (i + 1) << "] " << *handCards[i] << "\n";

	
	std::cout << "  Which card do you want to play? (1-" << handCards.size() << ", 0 for Menu): ";
	int cardChoice = readInt(0, handCards.size());

	if (cardChoice == 0) {
		clearScreen();
		printSeparator("COMBAT MENU");
		std::cout << "  [1] Back to Combat\n"
				  << "  [2] View Graveyard\n"
				  << "  [3] View Exile\n"
				  << "  [4] Activate Relic\n"
				  << "  [5] Sell a Relic\n"
				  << "  [0] Concede Run\n"
				  << "  Choice: ";

		int menuChoice = readInt(0, 5);

		if (menuChoice == 1) {
			return;
		} else if (menuChoice == 2) {
			std::cout << "\n  --- GRAVEYARD ---\n";
			const auto& grave = combatRound.getGraveyard().getCards();
			if (grave.empty()) {
				std::cout << "  (Empty)\n";
			} else {
				for (const auto& c : grave) {
					std::cout << "  - " << c->getName() << "\n";
				}
			}
			std::cout << "\n  Press ENTER to return...\n";
			std::cin.ignore(10000, '\n');
			std::cin.get();
			return;
		} else if (menuChoice == 3) {
			std::cout << "\n  --- EXILE ---\n";
			const auto& exile = combatRound.getExile().getCards();
			if (exile.empty()) {
				std::cout << "  (Empty)\n";
			} else {
				for (const auto& c : exile) {
					std::cout << "  - " << c->getName() << "\n";
				}
			}
			std::cout << "\n  Press ENTER to return...\n";
			std::cin.ignore(10000, '\n');
			std::cin.get();
			return;
		} else if (menuChoice == 4) {
            auto& permRelics = combatRound.getRelicZone().getRelicZone(); 
            std::vector<int> activatableIndices;
            for (size_t i = 0; i < permRelics.size(); i++) {
                if (permRelics[i]->isActivatable()) activatableIndices.push_back(i);
            }
            if (activatableIndices.empty()) {
                std::cout << "  [!] You have no activatable relics!\n";
            } else if (activatableIndices.size() == 1) { 
                std::cout << "\n  --- ACTIVATE RELIC ---\n";
                std::cout << "  --> Auto-activating " << permRelics[activatableIndices[0]]->getName() << "...\n";
                permRelics[activatableIndices[0]]->activate(combatRound);
            } else {
                std::cout << "\n  --- ACTIVATE RELIC ---\n";
                for (size_t i = 0; i < activatableIndices.size(); i++) std::cout << "  [" << (i + 1) << "] " << permRelics[activatableIndices[i]]->getName() << "\n";
                std::cout << "  Select relic to activate (0 to cancel): ";
                int actChoice = readInt(0, activatableIndices.size());
                if (actChoice > 0) permRelics[activatableIndices[actChoice - 1]]->activate(combatRound);
            }
            std::cout << "  Press ENTER to return...\n";
            std::cin.ignore(10000, '\n'); std::cin.get();
         } else if (menuChoice == 5) {
	        auto& permRelics = activeRun.getPlayer().getRelicZone().getRelicZone(); 
            
            if (permRelics.empty()) {
                std::cout << "  [!] You have no relics to shatter!\n";
            } else {
                std::cout << "\n  --- SHATTER RELIC (+2 Generic Mana) ---\n";
                for (size_t i = 0; i < permRelics.size(); i++) {
                    std::cout << "  [" << (i + 1) << "] " << permRelics[i]->getName() << "\n";
                }
                std::cout << "  Select relic to shatter (0 to cancel): ";
                
                int sellChoice = readInt(0, permRelics.size());
                
                if (sellChoice > 0) {
                    int index = sellChoice - 1;
                    std::cout << "  --> Shattered " << permRelics[index]->getName() << " for 2 Mana!\n";
					combatRound.promptForManaColor(combatRound.getManaPool(),2);
                    
                    combatRound.getRelicZone().removeRelic(index); 
                    activeRun.getPlayer().getRelicZone().removeRelic(index);
                }
            }
            std::cout << "  Press ENTER to return...\n";
            std::cin.ignore(10000, '\n'); std::cin.get();
            return;
		} else if (menuChoice == 0) {
			playerWon = false;
			state = GameState::GAME_OVER;
			return;
		}
	} else combatRound.playCardFromHand(cardChoice - 1);
	if (combatRound.hasLost()) {
		View::loseTheGame();
		playerWon = false;
		state = GameState::GAME_OVER;
		return;
	}
	if (combatRound.isRoundWon()) {
		combatRound.getRelicZone().triggerOnRoundEnd(combatRound);
		std::cout << "\n  *** BLIND DEFEATED! ***\n";
		std::cout << "  Score : " << combatRound.getCurrentScore() << " / "
				  << combatRound.getTargetScore() << "\n";
		std::cout << "  Press ENTER to visit the Shop...\n";
		std::cin.ignore(10000, '\n');
		std::cin.get();
		activeRun.finishRoundSequence();
		activeRun.getShop().generateRandomStock();
		state = GameState::SHOP;
		return;
	}
}

void View::showShop(GameState& state, ActiveRun& activeRun) {
	clearScreen();

	printSeparator("THE MERCHANT");

	Shop& shop = activeRun.getShop();  

	std::cout << "  Gold: " << activeRun.getPlayer().getGold() << "G\n\n";

	const auto& cards = shop.getCards();
	const auto& relics = shop.getRelics();

	int totalCards = cards.size();
	int totalRelics = relics.size();
	int totalItems = totalCards + totalRelics;

	std::cout << "  --- CARDS ---\n";
	for (int i = 0; i < totalCards; ++i) {
		std::cout << "  [" << (i + 1) << "] ";
		if (cards[i].isSold) {
			std::cout << "[SOLD OUT]\n";
		} else {
			std::cout << cards[i].item->getName() << " - " << cards[i].price << "G\n";
		}
	}

	std::cout << "\n  --- RELICS ---\n";
	for (int i = 0; i < totalRelics; ++i) {
		int displayIndex = i + 1 + totalCards;
		std::cout << "  [" << displayIndex << "] ";
		if (relics[i].isSold) {
			std::cout << "[SOLD OUT]\n";
		} else {
			std::cout << relics[i].item->getName() << " (" << relics[i].item->getDescription()
					  << ") - " << relics[i].price << "G\n";
		}
	}

	std::cout << "\n  [0] Leave Shop\n"
			  << "\n  ["<< totalItems+1 <<"] Sell Relic for Cash\n"
			  << "  Choice (0-" << totalItems+1 << "): ";

	int choice = readInt(0, totalItems+1);

	if (choice == 0) {
		state = GameState::COMBAT;
		return;
	}

	BuyResult result = BuyResult::INVALID;
	std::string itemName = "";

	if (choice > 0 && choice <= totalCards) {
		int cardIndex = choice - 1;
		if (!cards[cardIndex].isSold) {
			itemName = cards[cardIndex].item->getName();
		}
		result = shop.buyCard(cardIndex, activeRun.getPlayer());
	} else if (choice > totalCards && choice <= totalItems) {
		int relicIndex = choice - 1 - totalCards;
		if (!relics[relicIndex].isSold) {
			itemName = relics[relicIndex].item->getName();
		}
		result = shop.buyRelic(relicIndex, activeRun.getPlayer());
	}
	if (choice == totalItems+1 ) { 
            auto& permRelics = activeRun.getPlayer().getRelicZone().getRelicZone();
            if (permRelics.empty()) {
                std::cout << "  [!] You have no relics to sell!\n";
            } else {
                std::cout << "\n  --- SELL RELIC ---\n";
                for (size_t i = 0; i < permRelics.size(); i++) {
                    std::cout << "  [" << (i + 1) << "] " << permRelics[i]->getName() << " (+"<<Shop::calculatePrice(permRelics[i]->getRarity())<<"G)\n";
                }
                std::cout << "  Select relic to sell (0 to cancel): ";
                
                int sellChoice = readInt(0, permRelics.size());
                if (sellChoice > 0) {
                    int index = sellChoice - 1;
                    std::cout << "  --> Sold " << permRelics[index]->getName();    
                    activeRun.getPlayer().addGold(Shop::calculatePrice(permRelics[index]->getRarity()));
				    activeRun.getPlayer().getRelicZone().removeRelic(index);
                }
            }
            std::cout << "  Press ENTER to continue...\n";
            std::cin.ignore(10000, '\n'); std::cin.get();
        }

	std::cout << "\n";
	switch (result) {
		case BuyResult::SUCCESS:
			std::cout << "  *** Successfully purchased: " << itemName << " ***\n";
			break;
		case BuyResult::NO_GOLD:
			std::cout << "  [!] Not enough gold for " << itemName << ".\n";
			std::cout << "  Press ENTER to continue...\n";
			std::cin.ignore(10000, '\n');
			std::cin.get();
			break;
		case BuyResult::SOLD_OUT:
			std::cout << "  [!] That item is already sold out.\n";
			break;
		case BuyResult::INVALID:
			std::cout << "  [!] Invalid selection.\n";
			
			std::cout << "  Press ENTER to continue...\n";
			std::cin.ignore(10000, '\n');
			std::cin.get(); break;
	}
}

void View::showGameOver(bool playerWon, const ActiveRun& activeRun) {
	printSeparator("GAME OVER");
	if (playerWon) {
		std::cout << "  You beat every blind! Congratulations!\n";
	} else {
		std::cout << "  Your run has ended. Better luck next time.\n";
	}

	std::cout << "  Rounds won: " << activeRun.getCurrentRound() << "\n\n";
}

int View::requestChoice(const std::string& promptText, int min, int max) {
    std::cout << promptText;
    return readInt(min, max); // Reusing your existing bulletproof readInt!
}

char View::requestManaColor() {
    char choice = ' ';
    while (true) {
        std::cout << "\n  --- CHOOSE A MANA COLOR -[R][G][B]: ";
        std::cin >> choice;
        choice = std::toupper(choice);
        if (choice == 'R' || choice == 'B' || choice == 'G') {
            return choice;
        }
        std::cout << "  [!] Invalid choice. Please type R, B, or G.\n";
        std::cin.clear();
        std::cin.ignore(10000, '\n'); 
    }
}

void View::showMessage(const std::string& message) {
    std::cout << message << "\n";
}