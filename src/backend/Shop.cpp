
#include <memory>
#include <vector>
#include "PlayerInfo.h"

#include "CardDatabase.h"
#include "RelicFactory.h"
#include "Shop.h"

int Shop::calculatePrice(char rarity) {
    int basePrice = 0;
    switch (rarity) {
        case 'C': basePrice = 5; break; 
        case 'U': basePrice = 7; break; 
        case 'R': basePrice = 10; break;
        case 'L': basePrice = 20; break;
        case 'B': basePrice = 15; break; 
        default:  basePrice = 50; break;  }
    return basePrice; 
}

void Shop::generateRandomStock() {
	cardStock.clear();
	relicStock.clear();
	for (int i = 0; i < 3; i++) {
        auto randomCard = CardDatabase::getInstance().getRandomCard();
        if (randomCard) {
           int price = calculatePrice(randomCard->getRarity());
             cardStock.push_back({randomCard, price, false}); 
        }
    }

    for (int i = 0; i < 2; i++) {
        auto randomRelic = RelicDatabase::getInstance().getRandomRelic();
        if (randomRelic) {
            int price = calculatePrice(randomRelic->getRarity()) * 2;
            relicStock.push_back({randomRelic, price, false});
        }
    }
}

const std::vector<ShopItem<Card>>& Shop::getCards() const { return cardStock; }
const std::vector<ShopItem<IRelic>>& Shop::getRelics() const { return relicStock; }

BuyResult Shop::buyCard(int index, PlayerInfo& player) {
	if (index < 0 || static_cast<size_t>(index) >= cardStock.size()) return BuyResult::INVALID;
	if (cardStock[index].isSold) return BuyResult::SOLD_OUT;

	if (player.spendGold(cardStock[index].price)) {
		player.getDeck().addCard(cardStock[index].item);
		cardStock[index].isSold = true;
		return BuyResult::SUCCESS;
	}
	return BuyResult::NO_GOLD;
}

BuyResult Shop::buyRelic(int index, PlayerInfo& player) {
	if (index < 0 || static_cast<size_t>(index) >= relicStock.size()) return BuyResult::INVALID;
	if (relicStock[index].isSold) return BuyResult::SOLD_OUT;

	if (player.spendGold(relicStock[index].price)) {
		if (player.getRelicZone().addRelic(relicStock[index].item)) {
			relicStock[index].isSold = true;
			return BuyResult::SUCCESS;
		} else {
			player.addGold(relicStock[index].price);  
			return BuyResult::INVALID;
		}
	}
	return BuyResult::NO_GOLD;
}
