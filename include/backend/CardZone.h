#include <memory>
#include <vector>
#include <algorithm>
#include "RNG.h"
#pragma once

class Card;

template <typename T>
class Zone {
   protected:
	std::vector<std::shared_ptr<T>> items;

   public:
	void addItem(std::shared_ptr<T> c) { items.push_back(c); }
	void removeItem(int index) { items.erase(items.begin() + index); }
	int getSize() const { return items.size(); }
	void clearZone() { items.clear(); }
	void moveItemTo(int index, Zone<T>& destination) {
		destination.addItem(items[index]);
		removeItem(index);
	}
	const std::vector<std::shared_ptr<T>>& getItems() const { return items; }
	std::shared_ptr<T> popTopItem() {
		if (items.empty()) return nullptr;
		auto topItem = items.back();
		items.pop_back();
		return topItem;
	}
    void addItemToBottom(std::shared_ptr<T> c) { items.insert(items.begin(), c); }
	void shuffle() { std::shuffle(items.begin(), items.end(), RNG::engine()); }
};

class CardZone : public Zone<Card> {
   public:
	void addCard(std::shared_ptr<Card> c) { addItem(c); }
	void removeCard(int index) { removeItem(index); }
	void moveCardTo(int index, CardZone& destination) {
		destination.addItem(this->items[index]);
		removeItem(index);
	}
	const std::vector<std::shared_ptr<Card>>& getCards() const { return getItems(); }
	std::shared_ptr<Card> popTopCard() { return popTopItem(); }
    void addCardToBottom(std::shared_ptr<Card> c) { addItemToBottom(c); }
};
