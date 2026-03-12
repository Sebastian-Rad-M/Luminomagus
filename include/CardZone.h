#include <memory>
#include <vector>
#pragma once
class Card;
class CardZone {
   protected:
	std::vector<std::shared_ptr<Card>> cards;

   public:
	void addCard(std::shared_ptr<Card> c);
	void removeCard(int index);
	int getSize() const;
	void clearZone();
	void moveCardTo(int index, CardZone& destination);
	const std::vector<std::shared_ptr<Card>>& getCards() const;
	std::shared_ptr<Card> popTopCard();
    void addCardToBottom(std::shared_ptr<Card> c);
	void shuffle();

};
