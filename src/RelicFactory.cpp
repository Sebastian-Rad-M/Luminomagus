#include "RelicFactory.h"
#include "RelicEffects.h"
#include "RNG.h"
#include "ActiveRun.h"
RelicDatabase::RelicDatabase() { 
    loadAllRelics(); 
}

RelicDatabase& RelicDatabase::getInstance() {
    static RelicDatabase inst;
    return inst;
}

void RelicDatabase::loadAllRelics() {
    registry["r_hallOfMirrors"] = std::make_shared<OnManaRelic>("Hall of Mirrors", "+1 to all mana ",'R',2);
    registry["r_bellowsNotOfThisLand"] = std::make_shared<OnCastTriggerRelic>("Bellows not of this Land"," spell -> add 1 Red mana.",'U',std::make_unique<AddManaEffect>(1, 0, 0));
    registry["r_bloodstoneWetstone"] = std::make_shared<OnCastTriggerRelic>("Bloodstone Wetstone","spell -> deal 2 damage.",'C',std::make_unique<Score>(2));
    registry["r_archmagesTome"] = std::make_shared<OnCastTriggerRelic>("Archmage's Tome","spell -> draw 1 card.",'R',std::make_unique<DrawCardEffect>(1));
    registry["r_monacleOfBeyond"] = std::make_shared<OnRoundStartRelic>("Monacle of Beyond","start of round, mill 2",'C',std::make_unique<LambdaEffect>([](RoundTracker& state) {for(int i=0; i<2; i++) {auto c = state.getDeck().popTopCard(); if(c) state.getGraveyard().addCard(c);}}));
    registry["r_osmoticOrb"] = std::make_shared<OnRoundStartRelic>("Osmotic orb", "Start of round you may discard 1 to draw 1", 'C',std::make_unique<LambdaEffect>([](RoundTracker& state) { if (state.getHand().getCards().empty()) return; if (state.getUI()->requestChoice("  Use Osmotic orb to discard 1 and draw 1?\n  [1] Yes\n  [2] No\n  Choice: ", 1, 2) == 1) {if (state.promptDiscard()) state.drawCard();}}));
    registry["r_golden_ink_pen"] = std::make_shared<OnRoundEndRelic>("Golden ink pen", "if you end the round with 0 cards in hand, gain 5 gold", 'C',std::make_unique<LambdaEffect>([](RoundTracker& state) {if (state.getHand().getCards().empty()) { state.getRun().getPlayer().addGold(5);state.getUI()->showMessage("  --> Golden ink pen triggered: Gained 5 gold!");}}));
    registry["r_ignition_lens"] = std::make_shared<CustomManaRelic>("Ignition lens", "First basic gives double mana", 'C',[](int& r, int& b, int& g, RoundTracker& state) {if (state.getStormCount() == 0 && (r > 0 || b > 0 || g > 0)) {r *= 2;b *= 2;g *= 2;state.getUI()->showMessage(" ! [Ignition lens] Doubled mana!");}});
    registry["r_echoes_of_the_first_word"] = std::make_shared<OnRoundStartRelic>("Echoes of the First Word", "All spells have echo", 'L',std::make_unique<LambdaEffect>([](RoundTracker& state) {state.addStatus(std::make_unique<EchoStatus>(999));}));
    registry["r_electrical_kineticism"] = std::make_shared<OnRoundStartRelic>("Electrical kineticism", "Spells cost 1 generic less", 'C',std::make_unique<LambdaEffect>([](RoundTracker& state) {state.addStatus(std::make_unique<CostReductionStatus>(999));}));
    registry["r_sympathetic_lodestone"] = std::make_shared<SympatheticLodestoneRelic>();
    registry["r_altar_of_kindling"] = std::make_shared<ActiveRelic>("Altar of Kindling", "Discard a card: add 1 mana of its color", 'U',[](RoundTracker& state) {auto ui = state.getUI();auto& hand = state.getHand();if (hand.getCards().empty()) return;std::string prompt = "\n";const auto& cards = hand.getCards();for (size_t i = 0; i < cards.size(); i++) prompt += "  [" + std::to_string(i + 1) + "] " + cards[i]->getName() + "\n";prompt += "  Select card to discard (0 to cancel): ";int choice = ui->requestChoice(prompt, 0, cards.size());if (choice > 0) {int idx = choice - 1;auto c = cards[idx];state.addMana(c->getRedCost(), c->getBlueCost(), c->getGreenCost());ui->showMessage("  --> Discarded " + c->getName() + " for mana!");hand.moveCardTo(idx, state.getGraveyard());}});}


std::shared_ptr<IRelic> RelicDatabase::getRelic(const std::string& id) {
    auto it = registry.find(id);
    if (it != registry.end()) {
        return it->second->clone(); 
    }
    std::cerr << "  [!] Relic Database Error: Unknown ID '" << id << "'\n";
    return nullptr;
}

std::shared_ptr<IRelic> RelicDatabase::getRandomRelic() {
    if (registry.empty()) {
        return nullptr;
    }

    int totalWeight = 0;
    for (const auto& pair : registry) {
        char r = pair.second->getRarity();
        if (r == 'B') {
            continue;  
        } else if (r == 'C') {
            totalWeight += 60;  // Commons = 60%
        } else if (r == 'U') {
            totalWeight += 30;  // Uncommons = 30%
        } else if (r == 'R') {
            totalWeight += 10;  // Rares = 10%
        } else if (r == 'L') {
            totalWeight += 1;   // Legendaries = 1%
        }
    }

    if (totalWeight == 0) {
        std::cerr << "  [!] Error: No draftable relics in the database!\n";
        return nullptr;
    }

    int winningTicket = RNG::range(1, totalWeight);
    
    int currentWeight = 0;
    for (const auto& pair : registry) {
        char r = pair.second->getRarity();

        if (r == 'B') {
            continue;
        } else if (r == 'C') {
            currentWeight += 60;
        } else if (r == 'U') {
            currentWeight += 30;
        } else if (r == 'R') {
            currentWeight += 10;
        } else if (r == 'L') {
            currentWeight += 1;
        }

        if (currentWeight >= winningTicket) {
            return pair.second->clone();
        }
    }

    return nullptr;
}