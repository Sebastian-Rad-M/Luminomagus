#include "RelicZone.h"

RelicZone::RelicZone(int maxCapacity) : capacity(maxCapacity) {}
bool RelicZone::addRelic(std::shared_ptr<IRelic> relic) {
	if (relics.size() >= static_cast<size_t>(capacity)) {
		std::cout << "  [!] Relic Zone is full! Cannot add " << relic->getName() << ".\n";
		return false;
	}
	relics.push_back(relic);
	std::cout << "  Acquired Relic: " << relic->getName() << "!\n";
	return true;
}

void RelicZone::removeRelic(int index) {
if (index >= 0 && static_cast<size_t>(index) < relics.size()) relics.erase(relics.begin() + index);
    }


void RelicZone::triggerOnManaAdded(int& r, int& b, int& g, RoundTracker& state) {
	for (auto& relic : relics) {
		relic->onManaAdded(r, b, g, state);
	}
}

void RelicZone::triggerOnCardPlayed(RoundTracker& state) {
	for (auto& relic : relics) {
		relic->onCardPlayed(state);
	}
}
void RelicZone::triggerOnCardDrawn(RoundTracker& state) {
	for (auto& relic : relics) {
		relic->onCardDrawn(state);
	}
}
void RelicZone::triggerOnDamageDealt(int& scoredPts, RoundTracker& state) {
	for (auto& relic : relics) {
		relic->onDamageDealt(scoredPts, state);
	}
}
void RelicZone::triggerOnRoundStart(RoundTracker& state) {
	for (auto& relic : relics) relic->onRoundStart(state);
}
void RelicZone::triggerOnRoundEnd(RoundTracker& state) {
	for (auto& relic : relics) relic->onRoundEnd(state);
}