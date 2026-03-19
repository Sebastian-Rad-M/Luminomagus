#include "ActiveRun.h"
#include "RoundTracker.h"
RoundTracker::RoundTracker(const ActiveRun& runData, IPlayerPrompt* uiPrompt)
    : manaPool(0, 0, 0), currentRun(runData), currentScore(0), stormCount(0), isLost(false), ui(uiPrompt) { // Initialize ui
    targetScore = currentRun.calcTargetScore();
}

void RoundTracker::drawCard() {
	auto card = deck.popTopCard();
	if (card) {
		hand.addCard(card);
		relics.triggerOnCardDrawn(*this);
	} else isLost = true;
}
void RoundTracker::drawCards(int amount) {
	for (int i = 0; i < amount; i++) {
		drawCard();
	}
}

void RoundTracker::addStatus(std::unique_ptr<IStatus> status) {
	activeStatuses.push_back(std::move(status));
}

bool RoundTracker::promptDiscard() {
	if (hand.getSize() == 0) {
		return false;
	}

	std::cout << "\n  --- DISCARD A CARD ---\n";
	const auto& handCards = hand.getCards();
	for (size_t j = 0; j < handCards.size(); ++j) {
		std::cout << "  [" << (j + 1) << "] " << *handCards[j] << "\n";
	}

	int choice = -1;
	while (true) {
		std::cout << "  Choose a card to discard (1-" << handCards.size() << "): ";

		if (std::cin >> choice && choice >= 1 && static_cast<size_t>(choice) <= handCards.size()) {
			break;
		}
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		std::cout << "  [!] Invalid choice. Try again.\n";
	}
	std::cout << "  --> Discarded " << handCards[choice - 1]->getName() << "!\n";
	hand.moveCardTo(choice - 1, graveyard);

	return true;
}

bool RoundTracker::isRoundWon() const { return currentScore >= targetScore; }

void RoundTracker::startNewRound() {
	currentScore = 0;
	stormCount = 0;
	targetScore = currentRun.calcTargetScore();
	relics.triggerOnRoundStart(*this);
}

void RoundTracker::addScore(int amount) {
	relics.triggerOnDamageDealt(amount, *this);
	currentScore += amount;
}

void RoundTracker::addScoreToTarget(int amount) {
//	relics.triggerOnDamageDealt(amount, *this);
	targetScore+= amount;
}
void RoundTracker::addMana(int r, int b, int g) {
	relics.triggerOnManaAdded(r, b, g, *this);
	manaPool.addMana(r, b, g);
}
void RoundTracker::promptForManaColor(ManaPool& currentmanaPool,int nr) {
    for(int i=0;i<nr;i++)
	{char choice = ' ';
    bool valid = false;

    while (!valid) {
        std::cout << "\n  --- CHOOSE A MANA COLOR -[R][G][B]:\n";
        std::cin >> choice;
        choice = std::toupper(choice);
        if (choice == 'R' || choice == 'B' || choice == 'G') {
            valid = true;
        } else {
            std::cout << "  [!] Invalid choice. Please type R, B, or G.\n";
            std::cin.clear();
            std::cin.ignore(10000, '\n'); 
        }
		
	}
	currentmanaPool.addManaByColor(choice);
}
}
int RoundTracker::getStormCount() const { return stormCount; }

ManaPool& RoundTracker::getManaPool() { return manaPool; }
CardZone&  RoundTracker::getDeck(){return deck;}
CardZone& RoundTracker::getGraveyard() { return graveyard; }
CardZone& RoundTracker::getExile() { return exile; }
CardZone& RoundTracker::getHand() { return hand; }
int RoundTracker::getCurrentScore() const { return currentScore; }
int RoundTracker::getTargetScore() const { return targetScore; }
RelicZone& RoundTracker::getRelicZone() {return relics;}
void RoundTracker::printStatus() const {
	std::cout << "  Score : " << currentScore << " / " << targetScore << "\n"
			  << "  Storm : " << stormCount << "\n"
			  << "  Mana  : " << manaPool << "\n"
              << "  Deck  : " << deck.getSize() << " cards remaining\n";
}

void RoundTracker::setupDeck(const CardZone& library, const RelicZone& startingRelics) {
	deck.clearZone();
	for (const auto& card : library.getCards()) {
		deck.addCard(card);
	}

	deck.shuffle();
	// draw 5
	for (int i = 0; i < 5; i++) {
		drawCard();
	}
	this->relics = startingRelics;
}

bool RoundTracker::playCardFromHand(int index) {
	if (index < 0 || index >= hand.getSize()) {
		return false;
	}

	std::shared_ptr<Card> card = hand.getCards()[index];

	int r = card->getRedCost();
	int b = card->getBlueCost();
	int g = card->getGreenCost();
	int gen = card->getGenericCost();
	for (auto& status : activeStatuses) {
		status->modifyCost(r, b, g, gen);
	}
	if (manaPool.canAfford(r, b, g, gen)) {
		manaPool.spendMana(r, b, g, gen);
		card->play(*this);
		stormCount++;
		std::cout << "  --> Successfully played " << card->getName() << "!\n";
		for (auto& status : activeStatuses) {
			status->onCardPlayed(*card, *this);
		}
		relics.triggerOnCardPlayed(*this);
		
		CardZone* targetZone = &graveyard; 

		for (auto it = activeStatuses.begin(); it != activeStatuses.end();) 
			if ((*it)->isExpired()) it = activeStatuses.erase(it);
				else {
					(*it)->modifyDestination(targetZone, *this);
					it++;
					}
		
		hand.moveCardTo(index, *targetZone);
		return true;

	} else {
		std::cout << "  --> [!] Not enough mana to play " << card->getName() << "!\n";
		return false;
	}
}
int RoundTracker::requestHandTarget() {
	const auto& handCards = hand.getCards();
	if (handCards.empty()) return -1;
	std::cout << "\n  --- TARGET ---\n";
for (size_t i = 0; i < handCards.size(); i++) std::cout << "  [" << (i + 1) << "] " << *handCards[i] << "\n";
	std::cout << "  Select a card (1-" << handCards.size() << "): ";
	return View::readInt(1, handCards.size()) - 1;
}
void RoundTracker::setStormCount(int nr){stormCount=nr;}

