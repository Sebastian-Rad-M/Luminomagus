#pragma once
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

#include "Card.h"
/// mmmm mmmm i love me some singletons
class CardDatabase {
   private:
	CardDatabase();
	std::unordered_map<std::string, Card> library;

   public:
	static CardDatabase& getInstance();

	CardDatabase(const CardDatabase&) = delete;
	CardDatabase& operator=(const CardDatabase&) = delete;
	void loadAllCards();
	std::shared_ptr<Card> createCard(const std::string& cardID);
	
	/// @brief No rarity checks, a common and a legendary are equally likely, not to be called on 0.
	std::shared_ptr<Card> getTrueRandomCard();
	
	std::shared_ptr<Card> getRandomCard();
	
};
