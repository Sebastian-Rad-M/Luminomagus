#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "PlayerInfo.h"

void PlayerInfo::addGold(const int amount) { gold += amount; }
bool PlayerInfo::spendGold(const int amount) {
	if (gold < amount) return false;
	gold -= amount;
	return true;
}
PlayerInfo::PlayerInfo(const PlayerInfo& other) {
	this->gold = other.gold;
	for (const auto& card : other.deck.getCards()) this->deck.addCard(std::make_shared<Card>(*card));
	for (const auto& relic : other.relics.getRelicZone()) this->relics.addRelic(relic->clone());
}

PlayerInfo& PlayerInfo::operator=(const PlayerInfo& other) {
	if (this == &other)	return *this;
	this->gold = other.gold;
	this->deck.clearZone();
	this->relics.clearRelicZone();
	for (const auto& card : other.deck.getCards())this->deck.addCard(std::make_shared<Card>(*card));
	for (const auto& relic : other.relics.getRelicZone())this->relics.addRelic(relic->clone());
	return *this;
}

int PlayerInfo::getGold() const { return gold; }
CardZone& PlayerInfo::getDeck() { return deck; }
RelicZone& PlayerInfo::getRelicZone() { return relics; }

