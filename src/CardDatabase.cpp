#include "CardDatabase.h"
#include "Effect.h"
#include "RNG.h"

int CardDatabase::readInt(int l, int h) {
	int value;
	while (!(std::cin >> value) || value < l || value > h) {
		std::cin.clear();
		std::cin.ignore(10000, '\n');
		std::cout << "  Please enter a number between " << l << " and " << h << ": ";
	}
	return value;
}

CardDatabase& CardDatabase::getInstance() {
	static CardDatabase instance;
	return instance;
}

CardDatabase::CardDatabase() { loadAllCards(); }

void CardDatabase::loadAllCards() {
	//remember: View.readint
	/*i am hardcoding for the moment... or at lease partially hardcoding them, building enough classes to be able to make it fully modular is not in my cards
	*/

	//   [name  ]("name ",generic,red,blue green,rarity) 
	Card basicRed("Basic Red", 0, 0, 0, 0, 'B');
	basicRed.addEffect(std::make_unique<ConditionalEffect>([](const RoundTracker& state) {return state.getStormCount() < 3;}, std::make_unique<AddManaEffect>(1, 0, 0)));
	library["c_basic_red"] = basicRed;

	Card basicBlue("Basic Blue", 0, 0, 0, 0, 'B');
	basicBlue.addEffect(std::make_unique<ConditionalEffect>([](const RoundTracker& state) { return state.getStormCount() < 3; }, std::make_unique<AddManaEffect>(0,1, 0)));
	library["c_basic_blue"] = basicBlue;

	Card basicGreen("Basic Green", 0, 0, 0, 0, 'B');
	basicGreen.addEffect(std::make_unique<ConditionalEffect>([](const RoundTracker& state) { return state.getStormCount() < 3; }, std::make_unique<AddManaEffect>(0,0,1)));
	library["c_basic_green"] = basicGreen;

	// --- COMMON CARDS ---
	//Probe into nothingness
	//cost 0: draw 1,increase score requiremnent by 2
	Card gitaxianProbe("Gitaxian Probe", 0, 0, 0, 0, 'C');
	gitaxianProbe.addEffect(std::make_unique<DrawCardEffect>(1));
	gitaxianProbe.addEffect(std::make_unique<LambdaEffect>([](RoundTracker& state) 
	{state.addScoreToTarget(2);}));
	library["c_gitaxian_probe"] = gitaxianProbe;
	//Weight the Past
	// cost B:draw 1+1*nr of weigh the past in gy
	Card weightThePast("Weight the Past", 0, 0, 1, 0, 'C');
	weightThePast.addEffect(std::make_unique<DrawCardEffect>(1));
	weightThePast.addEffect(std::make_unique<GraveyardScaleEffect>(
		"Weight the Past", std::make_unique<DrawCardEffect>(1)));
	library["c_weightThePast"] = weightThePast;
	//rite of flame....
	Card kindle("Kindle", 0, 1, 0, 0, 'C');
	kindle.addEffect(std::make_unique<AddManaEffect>(2, 0, 0));
	kindle.addEffect(std::make_unique<GraveyardScaleEffect>(
		"Kindle", std::make_unique<AddManaEffect>(1, 0, 0)));
	library["c_kindle"] = kindle;
	
	//discern from darkness
	// look at the top 2+2*x cards, keep 1+1*x on top,rest GY. X=(nr of discern from darkness in gy)
		//i cant figure out for the life of me how to use gyscale, f the guy who wrote the cards
	Card discernFromDarkness("Discern from Darkness", 0, 0, 0, 1, 'C'); 
	discernFromDarkness.addEffect(std::make_unique<LambdaEffect>([](RoundTracker& state) {
        auto& deck = state.getDeck();
        auto& grave = state.getGraveyard();
        int x=1;
		for (const auto& card : grave.getCards()) if (card->getName() == "Discern from Darkness") x++;
        std::cout<<"  [!] Found "<<x<<" copies in GY. Looking at "<<2*x<<", keeping "<<x<<".\n";
        std::vector<std::shared_ptr<Card>> revealed;
        int actualLook = std::min(2*x, (int)deck.getCards().size());
        if (actualLook == 0)  return;
   		 for (int i = 0; i < actualLook; i++)  revealed.push_back(deck.popTopCard());
        std::vector<std::shared_ptr<Card>> keptCards;
        int actualKeep = std::min(x, (int)revealed.size());
        for (int p = 0; p < actualKeep; p++) {
            std::cout << "\n  --- DISCERN FROM DARKNESS (Keeping " << (p + 1) << " of " << actualKeep << ") ---\n";
            for (size_t i = 0; i < revealed.size(); i++) std::cout << "  [" << (i + 1) << "] " << revealed[i]->getName() << "\n";
            int index =readInt(1, revealed.size()) - 1;
            keptCards.push_back(revealed[index]);
            revealed.erase(revealed.begin() + index); }
        for (int i = keptCards.size() - 1; i >= 0; i--)  deck.addCard(keptCards[i]); 
        if (!revealed.empty()) for (auto& leftover : revealed)  grave.addCard(leftover);
            }));
	library["c_discern_from_darkness"] = discernFromDarkness;
		//grapeshot but 5
	Card fiatLux("Fiat Lux", 0, 1, 0, 0, 'C');
	fiatLux.addEffect(std::make_unique<Score>(5));
	fiatLux.addEffect(std::make_unique<StormEffect>(std::make_unique<Score>(5)));
	library["c_fiatLux"] = fiatLux;

		// its a pun, bolt but 10
	Card lightingBolt("Lighting Bolt", 0, 1, 0, 0, 'C');
	lightingBolt.addEffect(std::make_unique<Score>(10));
	library["c_lightingBolt"] = lightingBolt;
	
	Card refract("Refract", 1, 1, 0, 0, 'C');
	refract.addEffect(std::make_unique<DrawCardEffect>(1));
	refract.addEffect(std::make_unique<AddManaEffect>(0, 0, 0, 2));
	library["c_refract"] = refract;  

	Card appelDuVide("L'Appel du Vide", 0, 1, 0, 0, 'C'); 
	appelDuVide.addEffect(std::make_unique<AddManaEffect>(3, 0, 0));
	appelDuVide.addEffect(std::make_unique<DiscardEffect>(1));
	library["c_appel_du_vide"] = std::move(appelDuVide);
	
	Card dispersion("Dispersion", 2, 0, 1, 0, 'C');
	dispersion.addEffect(std::make_unique<DrawCardEffect>(2));
	dispersion.addEffect(std::make_unique<AddManaEffect>(1, 1, 1));
	dispersion.addEffect(std::make_unique<DiscardEffect>(2));
	library["c_dispersion"] =dispersion;
	
	Card siftTheScrolls("Sift the scrolls", 0, 0, 1, 0, 'C');
	siftTheScrolls.addEffect(std::make_unique<DrawCardEffect>(2));
	siftTheScrolls.addEffect(std::make_unique<DiscardEffect>(2));
	library["c_siftTheScrolls"] =siftTheScrolls;
		
	Card catalogue("Catalogue", 0, 0, 2, 0, 'U');
	catalogue.addEffect(std::make_unique<DrawCardEffect>(3));
	library["c_siftTheScrolls"] =catalogue;
	// Uncommons

	Card interfereWithThePattern("Interfere with the pattern", 0, 0, 1, 1, 'U'); 
	interfereWithThePattern.addEffect(std::make_unique<LambdaEffect>(
    [](RoundTracker& state) {
        std::cout << "\n  --- INTERFERE WITH THE PATTERN ---\n"<< "  [1] Deck\n"<< "  [2] Hand\n"<< "  [3] Graveyard\n"<< "  [4] Exile\n" << "  Select a zone to shuffle:";
        int choice = readInt(1, 4);
        switch(choice) {
            case 1: 
                state.getDeck().shuffle(); 
                std::cout << "  --> Deck shuffled.\n"; 
                break;
            case 2: 
                state.getHand().shuffle(); 
                std::cout << "  --> Hand shuffled? what are you doing man\n"; 
                break;
            case 3: 
                state.getGraveyard().shuffle(); 
                std::cout << "  --> Graveyard shuffled.\n"; 
                break;
            case 4: 
                state.getExile().shuffle(); 
                std::cout << "  --> Exile shuffled.\n"; 
                break;
        }
    }
));
interfereWithThePattern.addEffect(std::make_unique<DrawCardEffect>(1)); 

library["c_interfere_with_the_pattern"] = std::move(interfereWithThePattern);
	
	Card reflect("Reflect", 0, 1, 1, 0, 'U');
	reflect.addEffect(
		std::make_unique<ApplyStatusEffect>(std::make_unique<OverchargeStatus>(1)));
	library["c_reflect"] = reflect;
	// 3 Generic, 1 Green, 1 Blue, Uncommon. 
	// (Adjust the cost arguments to match your exact Card constructor order!)
	Card adNauseum("Ad nauseum", 2, 0, 1, 1,'U'); 

	adNauseum.addEffect(std::make_unique<LambdaEffect>([](RoundTracker& state) {
			auto& deck = state.getDeck();auto& hand = state.getHand();int totalCostRevealed = 0;
			std::cout << "\n  --- AD NAUSEUM ---\n";
			while (true) {
				if (deck.getCards().empty())  break;
				auto topCard = deck.popTopCard();
				int cardCost = topCard->totalCost(); 
				totalCostRevealed += cardCost;
				hand.addCard(topCard);

				std::cout << "  --> Revealed: " << topCard->getName() 
						<< " (Cost: " << cardCost << "). Total so far: " 
						<< totalCostRevealed << "/13\n";

				if (totalCostRevealed > 13) {
					std::cout << "  [!!!] Total cost exceeded 13 (" << totalCostRevealed << "). YOU LOSE.\n";
					state.addScoreToTarget(9999); 
					std::cout << "Please concede here";
					//TODO: make an actual lose the game func
					break;
				}
				std::cout << "\n  [1] Reveal another card\n";
				std::cout << "  [2] Stop here\n";
				int choice = readInt(1, 2);
				if (choice == 2) {
					std::cout << "  --> You stepped back from the brink. Ad Nauseum ends.\n";
					break;
				}
			}
		}
	));

	library["c_ad_nauseum"] = std::move(adNauseum);

	Card internalReflection("Internal Reflection", 0, 0, 0, 2, 'U'); // 2 Green, Uncommon

	internalReflection.addEffect(std::make_unique<LambdaEffect>(
		[](RoundTracker& state) {
			auto& hand = state.getHand();
			//auto& deck = state.getDeck();
			auto& grave = state.getGraveyard();
			int maxX = hand.getCards().size();
			std::cout<<"Choose an X(lower than"<<maxX<<")";
			int x = readInt(0, maxX);
			for (int i = 0; i < x; i++) {
				auto& handCards = hand.getCards();
				for (size_t j = 0; j < handCards.size(); j++) {
					std::cout << "  [" << (j + 1) << "] " << handCards[j]->getName() << "\n";
				}
				int choice = readInt(1, handCards.size());
				int index = choice - 1;

				std::cout << "  --> Discarded " << handCards[index]->getName() << "\n";
				grave.addCard(handCards[index]);
				hand.removeCard(index);
			}
		}
	));
	library["c_internal_reflection"] = std::move(internalReflection);

	Card strokeTheFadingEmbers("Stroke the fading embers", 0, 2, 0, 0, 'U'); 
	strokeTheFadingEmbers.addEffect(std::make_unique<AddManaEffect>(4, 0, 0)); 
	library["c_stroke_the_fading_embers"] = std::move(strokeTheFadingEmbers);

	Card radiantDecay("Radiant Decay", 0, 1, 0, 1, 'U'); 

radiantDecay.addEffect(std::make_unique<LambdaEffect>([](RoundTracker& state) {
        auto& grave = state.getGraveyard();
        auto& graveCards = grave.getCards();
        if (graveCards.empty()) return;
        for (size_t i = 0; i < graveCards.size(); i++)std::cout << "  [" << (i + 1) << "] " << graveCards[i]->getName() << "\n";
        int index = readInt(1, graveCards.size()) - 1;
        auto target = graveCards[index];
        state.addMana(target->getRedCost()+target-> getGenericCost(), target->getBlueCost(), target->getGreenCost());
        state.getExile().addCard(target);
        grave.removeCard(index); 
    }
));

library["c_radiant_decay"] = std::move(radiantDecay);
	// Rares and Legendaries
	Card recallWisdom("Recall Wisdom", 0, 0, 1, 0, 'R');
	recallWisdom.addEffect(std::make_unique<DrawCardEffect>(3));
	library["c_recallWisdom"] = recallWisdom;

	Card ponderTheSilence("Ponder the silence", 0, 0, 1, 0, 'R');
	ponderTheSilence.addEffect(std::make_unique<StormEffect>((std::make_unique<DrawCardEffect>(3))));
	library["c_ponderTheSilence"] = ponderTheSilence;


	Card doomsday("Doomsday", 0, 0, 2, 1, 'L'); // 2 Blue, 1 Green, Legendary
	doomsday.addEffect(std::make_unique<LambdaEffect>([](RoundTracker& state) {
        auto& grave = state.getGraveyard();
        auto& deck = state.getDeck();
        auto& exile = state.getExile();
        while (!grave.getCards().empty())deck.addCard(grave.popTopCard()); 
        std::vector<std::shared_ptr<Card>> purgatory;
        while (!deck.getCards().empty()) purgatory.push_back(deck.popTopCard());
		int cardsToPick = std::min(5, (int)purgatory.size());
        std::vector<std::shared_ptr<Card>> chosenCards;
        for (int p = 0; p < cardsToPick; p++) {
            std::cout << "\nPick " << (p + 1) << " of " << cardsToPick << ") ---\n";
            for (size_t i = 0; i < purgatory.size(); i++)std::cout << "  [" << (i + 1) << "] " << purgatory[i]->getName() << "\n";
            int choice = readInt(1, purgatory.size());
            int index = choice - 1;
            chosenCards.push_back(purgatory[index]);
            purgatory.erase(purgatory.begin() + index);
        }
        int exiledCount = purgatory.size();
        for (auto& leftover : purgatory)exile.addCard(leftover);
        std::cout << "\n  --> " << exiledCount << " remaining cards have been EXILED.\n";
        for (auto& c : chosenCards) deck.addCard(c);
            }));
	library["c_doomsday"] = std::move(doomsday);

	Card symphonyOfDust("Symphony of Dust", 1, 1, 0, 0, 'R'); // Cost: 1 Red, Rare
	symphonyOfDust.addEffect(std::make_unique<LambdaEffect>([](RoundTracker& state) 
	{
        auto& grave = state.getGraveyard();
        auto& exile = state.getExile();        
        int cardsToExile = grave.getCards().size();
            while (!grave.getCards().empty()) {
                exile.addCard(grave.popTopCard()); 
            }
            state.addScore(5*cardsToExile); 
            std::cout << "  --> Exiled " << cardsToExile << " cards! Dealt " << 5* cardsToExile << " damage!\n";
        }));
	library["c_symphonyOfDust"] = std::move(symphonyOfDust);
	
	Card digTroughMemories("Dig through memories", 1, 0, 0, 1,'U'); 
	digTroughMemories.addEffect(std::make_unique<LambdaEffect>(
    [](RoundTracker& state) {
        auto& grave = state.getGraveyard();
		auto& exile = state.getExile();
		auto& hand  = state.getHand();
        auto& deck  = state.getDeck(); 
        int maxExile = grave.getCards().size();
        if (maxExile == 0) {
            std::cout << "  [!] Graveyard is empty. You exile 0, look at 0.\n";
            return; 
        }
		int numToExile = -1;
        while (numToExile < 0 || numToExile > maxExile) {
            std::cout << "  Graveyard has " << maxExile << " cards.\n"<< "  How many do you want to exile from the top? (0 - " << maxExile << "): ";
            std::cin >> numToExile;
            if (std::cin.fail()) {std::cin.clear(); std::cin.ignore(10000, '\n');numToExile = -1; }
        }
        if (numToExile == 0) return; 
        for (int i = 0; i < numToExile; i++) exile.addCard(grave.popTopCard());  //TODO:maybe make em pick? nah, its a feature
        std::vector<std::shared_ptr<Card>> revealed;
        int cardsC = std::min(numToExile, (int)deck.getCards().size());
        for (int i = 0; i < cardsC; i++) revealed.push_back(deck.popTopCard());
        int picks = (numToExile >= 5) ? 2 : 1; 
        picks = std::min(picks, (int)revealed.size());
        for (int p = 0; p < picks; p++) {
            std::cout << "\n  --- MEMORY DREDGE (Pick " << (p + 1) << "/" << picks << ") ---\n";
            for (size_t i = 0; i < revealed.size(); i++) std::cout << "  [" << (i + 1) << "] " << revealed[i]->getName() << "\n";
            int choice = 0;
            while (choice < 1 ||static_cast<size_t>(choice) > revealed.size()) {
                std::cout << "  Select card to keep: ";
                std::cin >> choice;
                if (std::cin.fail()) {
                    std::cin.clear(); std::cin.ignore(10000, '\n'); choice = 0;
                }
            }
            int index = choice - 1;
            hand.addCard(revealed[index]);
            std::cout << "  --> Added " << revealed[index]->getName() <<"\n";
            revealed.erase(revealed.begin() + index);
        }

        if (!revealed.empty()) {
            std::cout << "  --> Put " << revealed.size() << " remaining cards on the bottom of the deck.\n";
            for (auto& leftover : revealed)
                deck.addCardToBottom(leftover); 
        }
    }
));

library["c_digTroughMemories"] = std::move(digTroughMemories);
	
Card fleurDuNeant("Fleur du neant", 0, 0, 0, 0, 'L');
	fleurDuNeant.addEffect(std::make_unique<AddManaEffect>(3, 3, 3));
	library["c_fleurDuNeant"] =fleurDuNeant;

	// TODO: JSON PARSER
}
std::shared_ptr<Card> CardDatabase::createCard(const std::string& cardID) {
	if (library.find(cardID) != library.end()) {
		return std::make_shared<Card>(library[cardID]);
	}
	std::cerr << "Error: Card ID " << cardID << " not found!\n";
	return nullptr;
}

std::shared_ptr<Card> CardDatabase::getTrueRandomCard() {
	if (library.empty()) {
		return nullptr;
	}
	int randomIndex = RNG::range(0, library.size() - 1);
	auto rCard = library.begin();
	std::advance(rCard, randomIndex);
	return std::make_shared<Card>(rCard->second);
}

std::shared_ptr<Card> CardDatabase::getRandomCard() {
	if (library.empty()) {
		return nullptr;
	}
	int totalWeight = 0;
	for (const auto& pair : library) {
		char r = pair.second.getRarity();

		if (r == 'B') {
			continue;  // Basics = 0
		} else if (r == 'C') {
			totalWeight += 60;	// Commons = 60%
		} else if (r == 'U') {
			totalWeight += 30;	// Uncommons = 30%
		} else if (r == 'R') {
			totalWeight += 10;	// Rares = 10%
		} else if (r == 'L') {
			totalWeight += 1;  // Legendaries = 1%
		}
	}
	if (totalWeight == 0) {
		std::cerr << "  [!] Error: No draftable cards in the database!\n";
		return nullptr;
	}

	int winningTicket = RNG::range(1, totalWeight);
	int currentWeight = 0;
	for (const auto& pair : library) {
		char r = pair.second.getRarity();

		if (r == 'B') {
			continue;
		}
		if (r == 'C') {
			currentWeight += 60;
		} else if (r == 'U') {
			currentWeight += 30;
		} else if (r == 'R') {
			currentWeight += 10;
		} else if (r == 'L') {
			currentWeight += 1;
		}
		if (currentWeight >= winningTicket) {
			return std::make_shared<Card>(pair.second);
		}
	}

	return nullptr;	 // Fallback
					 /// TODO: error handling
}
