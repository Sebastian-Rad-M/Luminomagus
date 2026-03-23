#include "CardDatabase.h"
#include "Effect.h"
#include "RNG.h"

CardDatabase &CardDatabase::getInstance() {
  static CardDatabase instance;
  return instance;
}

CardDatabase::CardDatabase() { loadAllCards(); }


 //Card cardID("Card Name", genericCost, redCost, blueCost, greenCost, 'rarity(char)');
void CardDatabase::loadAllCards() {
 
  // --- BASIC CARDS ---
  Card basicRed("Basic Red", 0, 0, 0, 0, 'B');
  basicRed.setText("If echo is less than 3, add R.");
  basicRed.addEffect(std::make_unique<ConditionalEffect>([](const RoundTracker &state) { return state.getStormCount() < 3; },std::make_unique<AddManaEffect>(1, 0, 0)));
  library["c_basic_red"] = basicRed;

  Card basicBlue("Basic Blue", 0, 0, 0, 0, 'B');
  basicBlue.setText("If echo is less than 3, add B.");
  basicBlue.addEffect(std::make_unique<ConditionalEffect>([](const RoundTracker &state) { return state.getStormCount() < 3; },std::make_unique<AddManaEffect>(0, 1, 0)));
  library["c_basic_blue"] = basicBlue;

  Card basicGreen("Basic Green", 0, 0, 0, 0, 'B');
  basicGreen.setText("If echo is less than 3, add G.");
  basicGreen.addEffect(std::make_unique<ConditionalEffect>([](const RoundTracker &state) { return state.getStormCount() < 3; },std::make_unique<AddManaEffect>(0, 0, 1)));
  library["c_basic_green"] = basicGreen;

  // --- COMMON CARDS ---
  
  Card gitaxianProbe("Gitaxian Probe", 0, 0, 0, 0, 'C');
  gitaxianProbe.setText("Add 2 to target score, draw a card");
  gitaxianProbe.addEffect(std::make_unique<DrawCardEffect>(1));
  gitaxianProbe.addEffect(std::make_unique<LambdaEffect>([](RoundTracker &state) { state.addScoreToTarget(2); }));
  library["c_gitaxian_probe"] = gitaxianProbe;
  
  Card weightThePast("Weight the Past", 0, 0, 1, 0, 'C');
  weightThePast.setText("Draw a card. Draw an additional one for each Weight the Past in your graveyard.");
  weightThePast.addEffect(std::make_unique<DrawCardEffect>(1));
  weightThePast.addEffect(std::make_unique<GraveyardScaleEffect>("Weight the Past", std::make_unique<DrawCardEffect>(1)));
  library["c_weightThePast"] = weightThePast;
  
  Card kindle("Kindle", 0, 1, 0, 0, 'C');
  kindle.setText("Add RR. Add an additional R for each Kindle in your graveyard.");
  kindle.addEffect(std::make_unique<AddManaEffect>(2, 0, 0));
  kindle.addEffect(std::make_unique<GraveyardScaleEffect>("Kindle", std::make_unique<AddManaEffect>(1, 0, 0)));
  library["c_kindle"] = kindle;

  Card showcaseOfKnowlege("Showcase of knowlege", 0, 0, 1, 0, 'C');
  showcaseOfKnowlege.setText("Score 3 for each card in your hand.");
  showcaseOfKnowlege.addEffect(std::make_unique<LambdaEffect>([](RoundTracker &state) {state.addScore(state.getHand().getCards().size() * 3);}));
  library["c_showcase_of_knowlege"] = std::move(showcaseOfKnowlege);

  // discern from darkness
  //  look at the top 2+2*x cards, keep 1+1*x on top,rest GY. X=(nr of discern
  //  from darkness in gy)
  // i cant figure out for the life of me how to use gyscale, f the guy who
  // wrote the cards
  
  //TODO:just fucking rework this 
  Card discernFromDarkness("Discern from Darkness", 0, 0, 0, 1, 'C');
  discernFromDarkness.addEffect(
      std::make_unique<LambdaEffect>([](RoundTracker &state) {
        auto &deck = state.getDeck();
        auto &grave = state.getGraveyard();
        int x = 1;
        for (const auto &card : grave.getCards())
          if (card->getName() == "Discern from Darkness")
            x++;
        std::vector<std::shared_ptr<Card>> revealed;
        int actualLook = std::min(2 * x, (int)deck.getCards().size());
        if (actualLook == 0)
          return;
        for (int i = 0; i < actualLook; i++)
          revealed.push_back(deck.popTopCard());
        std::vector<std::shared_ptr<Card>> keptCards;
        int actualKeep = std::min(x, (int)revealed.size());
        for (int p = 0; p < actualKeep; p++) {
          std::string prompt = "";
          for (size_t i = 0; i < revealed.size(); i++)
            prompt += "  [" + std::to_string(i + 1) + "] " +
                      revealed[i]->getName() + "\n";
          prompt += "  Select card to keep: ";
          int index = 0;
          keptCards.push_back(revealed[index]);
          revealed.erase(revealed.begin() + index);
        }
        for (int i = keptCards.size() - 1; i >= 0; i--)
          deck.addCard(keptCards[i]);
        if (!revealed.empty())
          for (auto &leftover : revealed)
            grave.addCard(leftover);
      }));
  library["c_discern_from_darkness"] = discernFromDarkness;


  Card fiatLux("Fiat Lux", 1, 1, 0, 0, 'C');
  fiatLux.setText("Score 5 for each echo");
  fiatLux.addEffect(std::make_unique<Score>(5));
  fiatLux.addEffect(std::make_unique<StormEffect>(std::make_unique<Score>(5)));
  library["c_fiatLux"] = fiatLux;

  Card lightingBolt("Lighting Bolt", 0, 1, 0, 0, 'C');
  lightingBolt.setText("Score 10. (get it? light-ing bolt?)");
  lightingBolt.addEffect(std::make_unique<Score>(10));
  library["c_lightingBolt"] = lightingBolt;

  Card refract("Refract", 1, 1, 0, 0, 'C');
  refract.setText("Add 2 light of any color. Draw a card.");
  refract.addEffect(std::make_unique<DrawCardEffect>(1));
  refract.addEffect(std::make_unique<AddManaEffect>(0, 0, 0, 2));
  library["c_refract"] = refract;

  Card appelDuVide("L'Appel du Vide", 0, 1, 0, 0, 'C');
  appelDuVide.setText("Add RRR. Discard a card");
  appelDuVide.addEffect(std::make_unique<AddManaEffect>(3, 0, 0));
  appelDuVide.addEffect(std::make_unique<DiscardEffect>(1));
  library["c_appel_du_vide"] = std::move(appelDuVide);

  Card dispersion("Dispersion", 2, 0, 1, 0, 'C');
  dispersion.setText("Draw 2. Add RGB. Discard 2.");
  dispersion.addEffect(std::make_unique<DrawCardEffect>(2));
  dispersion.addEffect(std::make_unique<AddManaEffect>(1, 1, 1));
  dispersion.addEffect(std::make_unique<DiscardEffect>(2));
  library["c_dispersion"] = dispersion;

  Card siftTheScrolls("Sift the scrolls", 0, 0, 1, 0, 'C');
  siftTheScrolls.setText("Draw 2.Discard 2.");
  siftTheScrolls.addEffect(std::make_unique<DrawCardEffect>(2));
  siftTheScrolls.addEffect(std::make_unique<DiscardEffect>(2));
  library["c_siftTheScrolls"] = siftTheScrolls;

  // --- UNCOMMONS --- 
  //ps:these are actually written by me, not ai, but i like being able to ctrl+F "---" for each delimitator

  Card catalogue("Catalogue", 0, 0, 2, 0, 'U');
  catalogue.setText("Draw 3");
  catalogue.addEffect(std::make_unique<DrawCardEffect>(3));
  library["c_siftTheScrolls"] = catalogue;

  //TODO:make ask for which zone
  Card interfereWithThePattern("Interfere with the pattern", 0, 0, 1, 1, 'U');
  interfereWithThePattern.setText("Shuffle one zone of your choice. Draw a card.");
  interfereWithThePattern.addEffect(std::make_unique<LambdaEffect>([](RoundTracker &state) {
        int choice = 1;
        switch (choice) {
        case 1:
          state.getDeck().shuffle();
          break;
        case 2:
          state.getHand().shuffle();
          break;
        case 3:
          state.getGraveyard().shuffle();
          break;
        case 4:
          state.getExile().shuffle();
          break;
        }
      }));
  interfereWithThePattern.addEffect(std::make_unique<DrawCardEffect>(1));
  library["c_interfere_with_the_pattern"] = std::move(interfereWithThePattern);

  Card reflect("Reflect", 0, 1, 1, 0, 'U');
  reflect.setText("Copy the next card you cast.");
  reflect.addEffect(std::make_unique<ApplyStatusEffect>(std::make_unique<OverchargeStatus>(1)));
  library["c_reflect"] = reflect;

  Card adNauseum("Ad nauseum", 2, 0, 1, 1, 'U');
  adNauseum.setText("Draw a card. You may repeat this any number of times. if the total cost of the cards drawn is greater than 13, lose the game");
  adNauseum.addEffect(std::make_unique<LambdaEffect>([](RoundTracker &state) {
    struct AdLoop {
      static void step(RoundTracker &s, int totalCostRevealed) {
        auto &deck = s.getDeck();
        auto &hand = s.getHand();
        if (deck.getCards().empty()) return;
        auto topCard = deck.popTopCard();
        int cardCost = topCard->totalCost();
        totalCostRevealed += cardCost;
        hand.addCard(topCard);
        if (totalCostRevealed > 13) {
          s.triggerLoss();
          return;
        }
        s.requestYesNoPrompt("Stop revealing?",[totalCostRevealed](bool stop, RoundTracker &st) {if (!stop)step(st, totalCostRevealed);});
      }
    };
    AdLoop::step(state, 0);
  }));
  library["c_ad_nauseum"] = adNauseum;

  //TODO:rework TS
  Card internalReflection("Internal Reflection", 0, 0, 0, 2,'U');
  internalReflection.addEffect(
      std::make_unique<LambdaEffect>([](RoundTracker &state) {
        auto &hand = state.getHand();
        auto &grave = state.getGraveyard();
        //int maxX = hand.getCards().size();
        int x = 0;
        for (int i = 0; i < x; i++) {
          auto &handCards = hand.getCards();
          std::string prompt = "";
          for (size_t j = 0; j < handCards.size(); j++) {
            prompt += "  [" + std::to_string(j + 1) + "] " +
                      handCards[j]->getName() + "\n";
          }
          int choice = 1;
          int index = choice - 1;

          grave.addCard(handCards[index]);
          hand.removeCard(index);
        }
      }));
  library["c_internal_reflection"] = std::move(internalReflection);

  Card strokeTheFadingEmbers("Stroke the fading embers", 0, 2, 0, 0, 'U');
  strokeTheFadingEmbers.setText("Add RR RR.");
  strokeTheFadingEmbers.addEffect(std::make_unique<AddManaEffect>(4, 0, 0));
  library["c_stroke_the_fading_embers"] = std::move(strokeTheFadingEmbers);
  
  //TODO:make GUI-ABLE
  Card radiantDecay("Radiant Decay", 0, 1, 0, 1, 'U');
  radiantDecay.setText("Exile a card from your graveyard. Add mana equal to its cost, the generic is red.");
  radiantDecay.addEffect(
      std::make_unique<LambdaEffect>([](RoundTracker &state) {
        auto &grave = state.getGraveyard();
        auto &graveCards = grave.getCards();
        if (graveCards.empty())return;
        std::string prompt = "";
        for (size_t i = 0; i < graveCards.size(); i++)
          prompt += "  [" + std::to_string(i + 1) + "] " +
                    graveCards[i]->getName() + "\n";
        prompt += "  Select card to decay: ";
        int index = 0;
        auto target = graveCards[index];
        state.addMana(target->getRedCost() + target->getGenericCost(),
                      target->getBlueCost(), target->getGreenCost());
        state.getExile().addCard(target);
        grave.removeCard(index);
      }));
  library["c_radiant_decay"] = std::move(radiantDecay);
  
  // --- Rares and Legendaries ---
  Card recallWisdom("Recall Wisdom", 0, 0, 1, 0, 'R');
  recallWisdom.setText("Draw 3.");
  recallWisdom.addEffect(std::make_unique<DrawCardEffect>(3));
  library["c_recallWisdom"] = recallWisdom;

  Card ponderTheSilence("Ponder the silence", 0, 0, 1, 0, 'R');
  ponderTheSilence.setText("Draw a card for each echo.");
  ponderTheSilence.addEffect(std::make_unique<StormEffect>((std::make_unique<DrawCardEffect>(1))));
  library["c_ponderTheSilence"] = ponderTheSilence;
  

  Card glimpseWCHB("Glimpse what could have been", 0, 0, 0, 1, 'C');
  glimpseWCHB.setText("Draw 2. Add one light of any color. Discard 3.");
  glimpseWCHB.addEffect(std::make_unique<DrawCardEffect>(2));
  glimpseWCHB.addEffect(std::make_unique<AddManaEffect>(0, 0, 0, 1));
  glimpseWCHB.addEffect(std::make_unique<DiscardEffect>(3));
  library["c_glimpseWCHB"] = glimpseWCHB;

  //TODO; refactor this? light, 5 minute job
  Card doomsday("Doomsday", 0, 0, 2, 1, 'L');
  doomsday.setText("Search your deck and graveyard for five cards and exile the rest. Put the chosen cards on top of your deck in any order.");
  doomsday.addEffect(std::make_unique<LambdaEffect>([](RoundTracker &state) {
    auto &grave = state.getGraveyard();
    auto &deck = state.getDeck();
    while (!grave.getCards().empty()) deck.addCard(grave.popTopCard());
    std::vector<std::shared_ptr<Card>> purgatory;
    while (!deck.getCards().empty())purgatory.push_back(deck.popTopCard());

    struct DoomsdayLoop {
      static void step(RoundTracker &s, std::vector<std::shared_ptr<Card>> purg,std::vector<std::shared_ptr<Card>> chosen, int needed) {
       if (chosen.size() >= static_cast<std::size_t>(needed) || purg.empty()) {
          for (auto &leftover : purg)
            s.getExile().addCard(leftover);
          for (auto &c : chosen)
            s.getDeck().addCard(c);
          return;
        }
        std::string promptMsg = "Pick " + std::to_string(chosen.size() + 1) +" of " + std::to_string(needed);
        s.requestCardSelection(
            promptMsg, purg,
            [purg, chosen, needed](int idx, RoundTracker &st) mutable {
              if (idx >= 0 && idx < (int)purg.size()) {
                chosen.push_back(purg[idx]);
                purg.erase(purg.begin() + idx);
              }
              step(st, purg, chosen, needed);
            });
      }
    };
    int cardsToPick = std::min(5, (int)purgatory.size());
    DoomsdayLoop::step(state, std::move(purgatory), {}, cardsToPick);
  }));
  library["c_doomsday"] = std::move(doomsday);

  Card symphonyOfDust("Symphony of Dust", 1, 1, 0, 0, 'R');
  symphonyOfDust.setText("Exile your graveyard. Score 5 for each exiled card.");
  symphonyOfDust.addEffect(std::make_unique<LambdaEffect>([](RoundTracker &state) {auto &grave = state.getGraveyard();auto &exile = state.getExile();int cardsToExile = grave.getCards().size();while (!grave.getCards().empty()) exile.addCard(grave.popTopCard());state.addScore(5 * cardsToExile);}));
  library["c_symphonyOfDust"] = std::move(symphonyOfDust);
  
  Card dreamingOfThePast("Dreaming of the past", 2, 0, 0, 1, 'R');
  dreamingOfThePast.setText("Move your graveyard to your hand. Until end of round, whenever a card goes to the graveyard, exile it.");
  dreamingOfThePast.addEffect(std::make_unique<LambdaEffect>([](RoundTracker &state) {auto &grave = state.getGraveyard();auto &hand = state.getHand();while (!grave.getCards().empty())hand.addCard(grave.popTopCard());state.addStatus(std::make_unique<GlobalExileStatus>());}));
  library["c_dreaming_of_the_past"] = std::move(dreamingOfThePast);

  Card totalInternalReflection("Total internal Reflection", 0, 2, 0, 1, 'L');
  totalInternalReflection.setText("Copy the next spell you cast for each echo you have");
  totalInternalReflection.addEffect(std::make_unique<LambdaEffect>([](RoundTracker &state) {state.addStatus(std::make_unique<EchoStatus>(1));}));
  library["c_total_internal_reflection"] = std::move(totalInternalReflection);

  Card lookUponMyWorks("Look Upon My Works", 2, 1, 0, 0, 'R');
  lookUponMyWorks.setText("Pay X light. Score 10 for each light paid.");
  lookUponMyWorks.addEffect(std::make_unique<LambdaEffect>([](RoundTracker &state) {state.requestXPrompt([](int x, RoundTracker &s) {if (s.getManaPool().canAfford(0, 0, 0, x)) {s.getManaPool().spendMana(0, 0, 0, x);s.addScore(x * 10);}});}));
  library["c_look_upon_my_works"] = std::move(lookUponMyWorks);

  Card incandescentWrath("Incandescent Wrath", 3, 0, 1, 0, 'R');
  incandescentWrath.setText("Discard your hand, then draw that many cards. Score 5 for each discarded card.");
  incandescentWrath.addEffect(std::make_unique<LambdaEffect>([](RoundTracker &state) {auto &hand = state.getHand();auto &grave = state.getGraveyard();int discardedCount = hand.getCards().size();for (int i = discardedCount - 1; i >= 0; --i)hand.moveCardTo(i, grave);state.addScore(5 * discardedCount);for (int i = 0; i < discardedCount; i++)state.drawCard();}));
  library["c_incandescent_wrath"] = std::move(incandescentWrath);

  Card absoluteKnowledge("Absolute knowledge", 0, 0, 3, 0, 'R');
  absoluteKnowledge.setText("If your deck is empty, score 200, otherwise draw 2.");
  absoluteKnowledge.addEffect(std::make_unique<LambdaEffect>([](RoundTracker &state) {if (state.getDeck().getCards().empty())state.addScore(200);else state.drawCards(2);}));
  library["c_absolute_knowledge"] = std::move(absoluteKnowledge);

  //TODO: refactor this for UI
  Card digTroughMemories("Dig through memories", 1, 0, 0, 1, 'U');
  digTroughMemories.addEffect(
      std::make_unique<LambdaEffect>([](RoundTracker &state) {
        auto &grave = state.getGraveyard();
        auto &exile = state.getExile();
        auto &hand = state.getHand();
        auto &deck = state.getDeck();
        int maxExile = grave.getCards().size();
        if (maxExile == 0) {
          return;
        }
        int numToExile = maxExile;
        if (numToExile == 0)
          return;
        for (int i = 0; i < numToExile; i++)
          exile.addCard(grave.popTopCard());
        std::vector<std::shared_ptr<Card>> revealed;
        int cardsC = std::min(numToExile, (int)deck.getCards().size());
        for (int i = 0; i < cardsC; i++)
          revealed.push_back(deck.popTopCard());
        int picks = (numToExile >= 5) ? 2 : 1;
        picks = std::min(picks, (int)revealed.size());
        for (int p = 0; p < picks; p++) {
          std::string prompt = "\n  --- MEMORY DREDGE (Pick " +
                               std::to_string(p + 1) + "/" +
                               std::to_string(picks) + ") ---\n";
          for (size_t i = 0; i < revealed.size(); i++)
            prompt += "  [" + std::to_string(i + 1) + "] " +
                      revealed[i]->getName() + "\n";
          prompt += "  Select card to keep: ";
          int index = 0;
          hand.addCard(revealed[index]);
          revealed.erase(revealed.begin() + index);
        }
        if (!revealed.empty())
          for (auto &leftover : revealed)
            deck.addCardToBottom(leftover);
      }));
  library["c_digTroughMemories"] = std::move(digTroughMemories);

  Card sparkOfGenesis("Spark of Genesis", 0, 1, 2, 0, 'R'); // 1 Red, 2 Blue
  sparkOfGenesis.setText("discard your hand. Draw 7 cards. Double your echo count.");
  sparkOfGenesis.addEffect(std::make_unique<LambdaEffect>([](RoundTracker &state) {auto &grave = state.getGraveyard();auto &deck = state.getDeck();int gySize = grave.getCards().size();for (int i = gySize - 1; i >= 0; i--)grave.moveCardTo(i, deck);deck.shuffle();state.drawCards(7);state.setStormCount(state.getStormCount() * 2);}));
  library["c_spark_of_genesis"] = std::move(sparkOfGenesis);

  Card fleurDuNeant("Fleur du neant", 0, 0, 0, 0, 'L');
  fleurDuNeant.setText("Add 3 light of any color");
  fleurDuNeant.addEffect(std::make_unique<AddManaEffect>(0,0,0,3));
  library["c_fleurDuNeant"] = fleurDuNeant;

}

/// @brief creates a card based on id
/// @param cardID the cardID
/// @return pointer to the card, to add to deck
std::shared_ptr<Card> CardDatabase::createCard(const std::string &cardID) {
  if (library.find(cardID) != library.end()) return std::make_shared<Card>(library[cardID]);
  std::cerr << "Error: Card ID " << cardID << " not found!\n";
  return nullptr;
}

std::shared_ptr<Card> CardDatabase::getTrueRandomCard() {
  if (library.empty()) return nullptr;
  int randomIndex = RNG::range(0, library.size() - 1);
  auto rCard = library.begin();
  std::advance(rCard, randomIndex);
  return std::make_shared<Card>(rCard->second);
}

std::shared_ptr<Card> CardDatabase::getRandomCard() {
  if (library.empty()) return nullptr;
  int totalWeight = 0;
  for (const auto &pair : library) {
    char r = pair.second.getRarity();

    if (r == 'B') {
      continue; // Basics = 0
    } else if (r == 'C') {
      totalWeight += 60; // Commons = 60%
    } else if (r == 'U') {
      totalWeight += 30; // Uncommons = 30%
    } else if (r == 'R') {
      totalWeight += 10; // Rares = 10%
    } else if (r == 'L') {
      totalWeight += 1; // Legendaries = 1%
    }
  }
  if (totalWeight == 0) {
    std::cerr << "  [!] Error: No draftable cards in the database!\n";
    return nullptr;
  }

  int winningTicket = RNG::range(1, totalWeight);
  int currentWeight = 0;
  for (const auto &pair : library) {
    char r = pair.second.getRarity();
    if (r == 'B') continue;
    if (r == 'C') currentWeight += 60;
    if (r == 'U') currentWeight += 30;
    if (r == 'R') currentWeight += 10;
    if (r == 'L') currentWeight += 1;
    if (currentWeight >= winningTicket) return std::make_shared<Card>(pair.second);
  }
  return nullptr; // Fallback, error handling
}
