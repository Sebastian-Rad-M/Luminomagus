#include <memory>
#include <algorithm>

#include "RNG.h"
#include "Card.h"
#include "CardZone.h"

void CardZone::addCard(std::shared_ptr<Card> c) { cards.push_back(c); }
void CardZone::removeCard(int index) { cards.erase(cards.begin() + index); }
int CardZone::getSize() const { return cards.size(); }
void CardZone::clearZone() { cards.clear(); }

//slight redundance with popTopCard 
void CardZone::moveCardTo(int index, CardZone& destination) {
	destination.addCard(cards[index]);
	removeCard(index);
}
std::shared_ptr<Card> CardZone::popTopCard() {
	if (cards.empty()) return nullptr;
	auto topCard = cards.back();
	cards.pop_back();
	return topCard;
}
void CardZone::addCardToBottom(std::shared_ptr<Card> c) {cards.insert(cards.begin(), c);}
const std::vector<std::shared_ptr<Card>>& CardZone::getCards() const { return cards; }
void CardZone::shuffle() { std::shuffle(cards.begin(), cards.end(), RNG::engine()); }

