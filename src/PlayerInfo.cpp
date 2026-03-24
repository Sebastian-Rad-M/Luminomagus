#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "PlayerInfo.h"

int PlayerInfo::getGold() const { return gold; }

void PlayerInfo::addGold(const int amount) { gold += amount; }
bool PlayerInfo::spendGold(const int amount) {
	if (gold < amount) {
		return false;
	}
	gold -= amount;
	return true;
}
CardZone& PlayerInfo::getDeck() { return deck; }
RelicZone& PlayerInfo::getRelicZone() { return relics; }

PlayerInfo::PlayerInfo(const PlayerInfo& other) {
	this->gold = other.gold;
	for (const auto& card : other.deck.getCards()) {
		this->deck.addCard(std::make_shared<Card>(*card));
	}
	for (const auto& relic : other.relics.getRelicZone()) {
		this->relics.addRelic(relic->clone());
	}
}

PlayerInfo& PlayerInfo::operator=(const PlayerInfo& other) {
	if (this == &other) {
		return *this;
	}

	this->gold = other.gold;

	this->deck.clearZone();
	this->relics.clearRelicZone();

	for (const auto& card : other.deck.getCards()) {
		this->deck.addCard(std::make_shared<Card>(*card));
	}

	for (const auto& relic : other.relics.getRelicZone()) {
		this->relics.addRelic(relic->clone());
	}
	return *this;
}

std::ostream& operator<<(std::ostream& os, const IRelic& relic) {
    os << "Relic [" << relic.getName() << "]\n"
       << " - Rarity: " << relic.getRarity() << "\n"
       << " - Status: " << (relic.isDisabled() ? "Disabled" : "Active") << "\n"
       << " - Description: " << relic.getDescription();
    return os;
}