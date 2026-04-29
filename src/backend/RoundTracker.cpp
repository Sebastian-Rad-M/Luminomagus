#include "ActiveRun.h"
#include "RoundTracker.h"

RoundTracker::RoundTracker(const ActiveRun& runData)
	: manaPool(0, 0, 0),
	  currentRun(runData),
	  currentScore(0),
	  stormCount(0),
	  isLost(false),
	  pendingManaChoices(0) {
	targetScore = currentRun.calcTargetScore();
}

void RoundTracker::drawCard() {
	auto card = deck.popTopCard();
	if (card) {
		hand.addCard(card);
		relics.triggerOnCardDrawn(*this);
		for (const auto& status : activeStatuses) {
			status->onCardDrawn(*card, *this);
		}
	} else {
		isLost = true;
	}
}

void RoundTracker::drawCards(int amount) {
	for (int i = 0; i < amount; i++) {
		drawCard();
	}
}

void RoundTracker::addStatus(std::unique_ptr<IStatus> status) {
	activeStatuses.push_back(std::move(status));
	activeStatuses.back()->onApplied(*this);
}
// TODO: make it Gui prompt for which one
void RoundTracker::promptDiscard(std::function<void(int, RoundTracker&)> cb) {
	if (hand.getSize() == 0) {
		if (cb) {
			cb(-1, *this);
		}
		return;
	}
	requestHandClick("Select a card to discard", [cb](int idx, RoundTracker& st) {
		if (idx >= 0 && idx < st.getHand().getSize()) {
			st.getHand().moveCardTo(idx, st.getGraveyard());
			if (cb) {
				cb(idx, st);
			}
		} else {
			if (cb) {
				cb(-1, st);
			}
		}
	});
}

bool RoundTracker::isRoundWon() const { return currentScore >= targetScore; }

void RoundTracker::startNewRound() {
	currentScore = 0;
	stormCount = 0;
	targetScore = currentRun.calcTargetScore();
	for (const auto& relic : relics.getRelicZone()) {
		if (relic) {
			relic->setDisabled(false);
		}
	}
	for (const auto& status : activeStatuses) {
		status->modifyTargetScore(targetScore, *this);
	}
	relics.triggerOnRoundStart(*this);
}

void RoundTracker::addScore(int amount) {
	relics.triggerOnDamageDealt(amount, *this);
	currentScore += amount;
}

void RoundTracker::addScoreToTarget(int amount) { targetScore += amount; }

void RoundTracker::addMana(int r, int b, int g) {
	relics.triggerOnManaAdded(r, b, g, *this);
	manaPool.addMana(r, b, g);
	for (const auto& status : activeStatuses) {
		status->onManaAdded(r, b, g, *this);
	}
}

void RoundTracker::addManaByColor(char color) {
	switch (color) {
		case 'R':
		case 'r':
			addMana(1, 0, 0);
			break;
		case 'B':
		case 'b':
			addMana(0, 1, 0);
			break;
		case 'G':
		case 'g':
			addMana(0, 0, 1);
			break;
		default:
			break;
	}
	if (pendingManaChoices > 0) {
		pendingManaChoices--;
	}
}

int RoundTracker::getStormCount() const { return stormCount; }
ManaPool& RoundTracker::getManaPool() { return manaPool; }
CardZone& RoundTracker::getDeck() { return deck; }
CardZone& RoundTracker::getGraveyard() { return graveyard; }
CardZone& RoundTracker::getExile() { return exile; }
CardZone& RoundTracker::getHand() { return hand; }
int RoundTracker::getCurrentScore() const { return currentScore; }
int RoundTracker::getTargetScore() const { return targetScore; }
RelicZone& RoundTracker::getRelicZone() { return relics; }

void RoundTracker::setupDeck(const CardZone& library, const RelicZone& startingRelics) {
	deck.clearZone();
	for (const auto& card : library.getCards()) {
		deck.addCard(card);
	}
	deck.shuffle();
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
	for (const auto& status : activeStatuses) {
		if (!status->canPlayCard(*card, *this)) {
			return false;
		}
	}
	int r = card->getRedCost();
	int b = card->getBlueCost();
	int g = card->getGreenCost();
	int gen = card->getGenericCost();
	for (auto& status : activeStatuses) {
		status->modifyCost(r, b, g, gen, *this);
	}
	if (manaPool.canAfford(r, b, g, gen)) {
		manaPool.spendMana(r, b, g, gen);
		card->play(*this);
		stormCount++;
		for (auto& status : activeStatuses) {
			status->onCardPlayed(*card, *this);
		}
		relics.triggerOnCardPlayed(*this);
		CardZone* targetZone = &graveyard;
		for (auto it = activeStatuses.begin(); it != activeStatuses.end();) {
			if ((*it)->isExpired()) {
				it = activeStatuses.erase(it);
			} else {
				(*it)->modifyDestination(targetZone, *this);
				it++;
			}
		}
		hand.moveCardTo(index, *targetZone);
		return true;
	} else {
		return false;
	}
}

// TODO: make it Gui prompt for which one
void RoundTracker::requestHandTarget(std::function<void(int, RoundTracker&)> cb) {
	if (hand.getCards().empty()) {
		if (cb) {
			cb(-1, *this);
		}
		return;
	}
	requestHandClick("Select a card from your hand", [cb](int idx, RoundTracker& st) {
		if (cb) {
			cb(idx, st);
		}
	});
}

void RoundTracker::setStormCount(int nr) { stormCount = nr; }

std::string RoundTracker::getActiveBossName() const {
	for (const auto& status : activeStatuses) {
		if (status->isBoss()) {
			return status->getName();
		}
	}
	return "";
}
std::string RoundTracker::getActiveBossDescription() const {
	for (const auto& status : activeStatuses) {
		if (status->isBoss()) {
			return status->getDescription();
		}
	}
	return "";
}
