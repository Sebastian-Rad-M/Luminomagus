#include "RelicFactory.h"
#include "RelicEffects.h"
#include "RNG.h"
#include "ActiveRun.h"
#include "CardDatabase.h"

RelicDatabase::RelicDatabase() {loadAllRelics();}

RelicDatabase& RelicDatabase::getInstance() {static RelicDatabase inst;return inst;}

void RelicDatabase::loadAllRelics() {
    registry["r_hallOfMirrors"] = std::make_shared<OnManaRelic>("Hall of Mirrors", "+1 to all mana ",'R',2);
    registry["r_bellowsNotOfThisLand"] = std::make_shared<OnCastTriggerRelic>("Bellows not of this Land"," spell -> add 1 Red mana.",'U',std::make_unique<AddManaEffect>(1, 0, 0));
    registry["r_bloodstoneWetstone"] = std::make_shared<OnCastTriggerRelic>("Bloodstone Wetstone","spell -> deal 2 damage.",'C',std::make_unique<Score>(2));
    registry["r_archmagesTome"] = std::make_shared<OnCastTriggerRelic>("Archmage's Tome","spell -> draw 1 card.",'R',std::make_unique<DrawCardEffect>(1));
    registry["r_monacleOfBeyond"] = std::make_shared<OnRoundStartRelic>("Monacle of Beyond","start of round, mill 2",'C',std::make_unique<LambdaEffect>([](RoundTracker& state) {for(int i=0; i<2; i++) {auto c = state.getDeck().popTopCard(); if(c) state.getGraveyard().addCard(c);}}));
    registry["r_osmoticOrb"] = std::make_shared<OnRoundStartRelic>("Osmotic orb", "Start of round you may discard 1 to draw 1", 'C',std::make_unique<LambdaEffect>([](RoundTracker& state) {auto& handCards = state.getHand().getCards();if (handCards.empty()) return; state.requestYesNoPrompt("Discard a card to draw 1?", [](bool yes, RoundTracker& s){ if(yes) { s.promptDiscard([](int idx, RoundTracker& st){ if (idx>=0) st.drawCard(); }); } });}));
    registry["r_golden_ink_pen"] = std::make_shared<OnRoundEndRelic>("Golden ink pen", "if you end the round with 0 cards in hand, gain 5 gold", 'C',std::make_unique<LambdaEffect>([](RoundTracker& state) {if (state.getHand().getCards().empty()) { state.getRun().getPlayer().addGold(5);}}));
    registry["r_ignition_lens"] = std::make_shared<CustomManaRelic>("Ignition lens", "First basic gives double mana", 'C',[](int& r, int& b, int& g, RoundTracker& state) {if (state.getStormCount() == 0 && (r > 0 || b > 0 || g > 0)) {r *= 2;b *= 2;g *= 2;}});
    registry["r_echoes_of_the_first_word"] = std::make_shared<OnRoundStartRelic>("Echoes of the First Word", "All spells have echo", 'L',std::make_unique<LambdaEffect>([](RoundTracker& state) {state.addStatus(std::make_unique<EchoStatus>(999));}));
    registry["r_electrical_kineticism"] = std::make_shared<OnRoundStartRelic>("Electrical kineticism", "Spells cost 1 generic less", 'C',std::make_unique<LambdaEffect>([](RoundTracker& state) {state.addStatus(std::make_unique<CostReductionStatus>(999));}));
    registry["r_sympathetic_lodestone"] = std::make_shared<SympatheticLodestoneRelic>();
    registry["r_altar_of_kindling"] = std::make_shared<ActiveRelic>("Altar of Kindling", "Discard a card: add 1 mana of its color", 'U',[](RoundTracker& state) {
        state.promptDiscard([](int idx, RoundTracker& st) {
            if (idx >= 0) {
                auto& graveCards = st.getGraveyard().getCards();
                if (!graveCards.empty()) {
                    auto discardedCard = graveCards.back();
                    int r = discardedCard->getRedCost() > 0 ? 1 : 0;
                    int b = discardedCard->getBlueCost() > 0 ? 1 : 0;
                    int g = discardedCard->getGreenCost() > 0 ? 1 : 0;
                    if (r > 0 || b > 0 || g > 0) st.addMana(r, b, g);
                }
            }
        });
    });
    registry["r_locket_of_yesterdays"] = std::make_shared<OnRoundStartRelic>("Locket of yesterdays", "Copy spells for each with the same name in GY", 'C',std::make_unique<LambdaEffect>([](RoundTracker& state) {auto locketStatus = std::make_unique<LambdaStatus>("Locket", 999);locketStatus->setPlayAction([](Card& c, RoundTracker& s) {int copies = 0;for (const auto& gyCard : s.getGraveyard().getCards()) if (gyCard->getName() == c.getName()) copies++;for (int i = 0; i < copies; i++) {c.play(s); }});state.addStatus(std::move(locketStatus));}));
    registry["r_undescifrable_codex"] = std::make_shared<ActiveRelic>("Undescifrable codex", "Pay X, cast a random spell with X cost", 'L',[](RoundTracker& state) {
        state.requestXPrompt([](int x, RoundTracker& s) {
            if (x == 0) return;
            s.getManaPool().spendMana(0, 0, 0, x);
            std::shared_ptr<Card> castCard = nullptr;
            for (int i = 0; i < 500; i++) {auto c = CardDatabase::getInstance().getTrueRandomCard();if (c && c->totalCost() == x) {castCard = c;break;}}
            if (castCard) {
                castCard->play(s);
                s.setPromptMessage("Codex cast: " + castCard->getName());
            } else {
                s.setPromptMessage("Codex found no " + std::to_string(x) + "-cost spell.");
            }
        });
    });
}


std::shared_ptr<IRelic> RelicDatabase::getRelic(const std::string& id) {
    auto it = registry.find(id);
    if (it != registry.end()) return it->second->clone(); 
    std::cerr << "  [!] Relic Database Error: Unknown ID '" << id << "'\n";
    return nullptr;
}

std::shared_ptr<IRelic> RelicDatabase::getRandomRelic() {
    if (registry.empty()) return nullptr;
    int totalWeight = 0;
    for (const auto& pair : registry) {
        char r = pair.second->getRarity();
        if (r == 'B') continue;  
        if (r == 'C') totalWeight += 60;  // Commons = 60%
        if (r == 'U') totalWeight += 30;  // Uncommons = 30%
        if (r == 'R') totalWeight += 10;  // Rares = 10%
        if (r == 'L') totalWeight += 1;   // Legendaries = 1%
    }

    if (totalWeight == 0) {
        std::cerr << "  [!] Error: No draftable relics in the database!\n";
        return nullptr;
    }
    int winningTicket = RNG::range(1, totalWeight);
    int currentWeight = 0;
    for (const auto& pair : registry) {
        char r = pair.second->getRarity();

       if (r == 'B') continue;  
        if (r == 'C') currentWeight += 60;  
        if (r == 'U')currentWeight += 30;  
        if (r == 'R') currentWeight += 10;  
        if (r == 'L') currentWeight += 1;
        if (currentWeight >= winningTicket) return pair.second->clone();
    }

    return nullptr;
}

//TODO: rem, its for debug
std::shared_ptr<IRelic> RelicDatabase::getRandomRareOrLegendaryRelic() {
    if (registry.empty()) return nullptr;
    
    std::vector<std::shared_ptr<IRelic>> candidates;
    for (const auto& pair : registry) {
        char r = pair.second->getRarity();
        // Filter for Rare ('R') or Legendary ('L') relics
        if (r == 'R' || r == 'L') {
            candidates.push_back(pair.second);
        }
    }
    
    if (candidates.empty()) return nullptr;
    
    // Pick a random index from the filtered pool
    int idx = RNG::range(0, candidates.size() - 1);
    return candidates[idx]->clone();
}