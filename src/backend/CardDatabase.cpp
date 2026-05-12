#include "CardDatabase.h"
#include "Effect.h"
#include "Exceptions.h"
#include "RNG.h"

CardDatabase &CardDatabase::getInstance() {
  static CardDatabase instance;
  return instance;
}

CardDatabase::CardDatabase() { loadAllCards(); }

// Card cardID("Card Name", genericCost, redCost, blueCost, greenCost,
// 'rarity(char)');
void CardDatabase::loadAllCards() {

  // --- BASIC CARDS ---
  Card basicRed("Basic Red", 0, 0, 0, 0, 'B');
  basicRed.setText("If echo is less than 3, add R.");
  basicRed.addEffect(std::make_unique<ConditionalEffect>(
      [](const RoundTracker &state) { return state.getStormCount() < 3; },
      std::make_unique<AddManaEffect>(1, 0, 0)));
  library["c_basic_red"] = basicRed;

  Card basicBlue("Basic Blue", 0, 0, 0, 0, 'B');
  basicBlue.setText("If echo is less than 3, add B.");
  basicBlue.addEffect(std::make_unique<ConditionalEffect>(
      [](const RoundTracker &state) { return state.getStormCount() < 3; },
      std::make_unique<AddManaEffect>(0, 1, 0)));
  library["c_basic_blue"] = basicBlue;

  Card basicGreen("Basic Green", 0, 0, 0, 0, 'B');
  basicGreen.setText("If echo is less than 3, add G.");
  basicGreen.addEffect(std::make_unique<ConditionalEffect>(
      [](const RoundTracker &state) { return state.getStormCount() < 3; },
      std::make_unique<AddManaEffect>(0, 0, 1)));
  library["c_basic_green"] = basicGreen;

  // --- COMMON CARDS ---

  Card gitaxianProbe("Gitaxian Probe", 0, 0, 0, 0, 'C');
  gitaxianProbe.setText("Add 2 to target score, draw a card");
  gitaxianProbe.addEffect(std::make_unique<DrawCardEffect>(1));
  gitaxianProbe.addEffect(std::make_unique<LambdaEffect>(
      [](RoundTracker &state) { state.addScoreToTarget(2); }));
  library["c_gitaxian_probe"] = gitaxianProbe;

  Card weightThePast("Weight the Past", 0, 0, 1, 0, 'C');
  weightThePast.setText("Draw a card. Draw an additional one for each Weight "
                        "the Past in your graveyard.");
  weightThePast.addEffect(std::make_unique<DrawCardEffect>(1));
  weightThePast.addEffect(std::make_unique<GraveyardScaleEffect>(
      "Weight the Past", std::make_unique<DrawCardEffect>(1)));
  library["c_weightThePast"] = weightThePast;

  Card kindle("Kindle", 0, 1, 0, 0, 'C');
  kindle.setText(
      "Add RR. Add an additional R for each Kindle in your graveyard.");
  kindle.addEffect(std::make_unique<AddManaEffect>(2, 0, 0));
  kindle.addEffect(std::make_unique<GraveyardScaleEffect>(
      "Kindle", std::make_unique<AddManaEffect>(1, 0, 0)));
  library["c_kindle"] = kindle;

  Card showcaseOfKnowlege("Showcase of knowlege", 0, 0, 1, 0, 'C');
  showcaseOfKnowlege.setText("Score 3 for each card in your hand.");
  showcaseOfKnowlege.addEffect(
      std::make_unique<LambdaEffect>([](RoundTracker &state) {
        state.addScore((int)state.getHand().getCards().size() * 3);
      }));
  library["c_showcase_of_knowlege"] = showcaseOfKnowlege;

  Card discernFromDarkness("Discern from Darkness", 0, 0, 0, 1, 'C');
  discernFromDarkness.setText("Shuffle your graveyard into your deck. Draw a card.");
  discernFromDarkness.addEffect(
      std::make_unique<LambdaEffect>([](RoundTracker &state) {
        auto &grave = state.getGraveyard();
        auto &deck = state.getDeck();
        while (!grave.getCards().empty())
          deck.addCard(grave.popTopCard());
        deck.shuffle();
        state.drawCard();
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
  library["c_appel_du_vide"] = appelDuVide;

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
  // ps:these are actually written by me, not ai, but i like being able to
  // ctrl+F "---" for each delimitator

  Card catalogue("Catalogue", 0, 0, 2, 0, 'U');
  catalogue.setText("Draw 3");
  catalogue.addEffect(std::make_unique<DrawCardEffect>(3));
  library["c_catalogue"] = catalogue;

  Card interfereWithThePattern("Interfere with the pattern", 0, 0, 1, 1, 'U');
  interfereWithThePattern.setText(
      "Shuffle one zone of your choice. Draw a card.");
  interfereWithThePattern.addEffect(
      std::make_unique<LambdaEffect>([](RoundTracker &state) {
        state.requestOptionSelection("Select a zone to shuffle",
                                     {"Deck", "Hand", "Graveyard", "Exile"},
                                     [](int choice, RoundTracker &st) {
                                       switch (choice) {
                                       case 0:
                                         st.getDeck().shuffle();
                                         break;
                                       case 1:
                                         st.getHand().shuffle();
                                         break;
                                       case 2:
                                         st.getGraveyard().shuffle();
                                         break;
                                       case 3:
                                         st.getExile().shuffle();
                                         break;
                                       default:
                                         break;
                                       }
                                     });
      }));
  interfereWithThePattern.addEffect(std::make_unique<DrawCardEffect>(1));
  library["c_interfere_with_the_pattern"] = interfereWithThePattern;

  Card reflect("Reflect", 0, 1, 1, 0, 'U');
  reflect.setText("Copy the next card you cast.");
  reflect.addEffect(std::make_unique<ApplyStatusEffect>(
      std::make_unique<OverchargeStatus>(1)));
  library["c_reflect"] = reflect;

  Card adNauseum("Ad nauseum", 2, 0, 1, 1, 'U');
  adNauseum.setText(
      "Draw a card. You may repeat this any number of times. if the total cost "
      "of the cards drawn is greater than 13, lose the game");
  adNauseum.addEffect(std::make_unique<LambdaEffect>([](RoundTracker &state) {
    struct AdLoop {
      static void step(RoundTracker &s, int totalCostRevealed) {
        auto &deck = s.getDeck();
        auto &hand = s.getHand();
        if (deck.getCards().empty())
          return;
        auto topCard = deck.popTopCard();
        int cardCost = topCard->totalCost();
        totalCostRevealed += cardCost;
        hand.addCard(topCard);
        if (totalCostRevealed > 13) {
          s.triggerLoss();
          return;
        }
        s.requestYesNoPrompt("Stop revealing?",
                             [totalCostRevealed](bool stop, RoundTracker &st) {
                               if (!stop)
                                 step(st, totalCostRevealed);
                             });
      }
    };
    AdLoop::step(state, 0);
  }));
  library["c_ad_nauseum"] = adNauseum;

  Card internalReflection("Internal Reflection", 0, 0, 0, 2, 'U');
  internalReflection.setText("Discard any number of cards. Draw that many cards. Add 1 Light of any color for each card discarded this way.");
  internalReflection.addEffect(
      std::make_unique<LambdaEffect>([](RoundTracker &state) {
        state.requestXPrompt([](int x, RoundTracker &s) {
          int maxDiscard = s.getHand().getSize();
          int toDiscard = std::min(x, maxDiscard);
          if (toDiscard <= 0) return;
          struct Loop {
            static void step(RoundTracker &st, int remaining, int total) {
              if (remaining <= 0) {
                st.drawCards(total);
                st.requestManaChoices(total);
                return;
              }
              st.promptDiscard([remaining, total](int idx, RoundTracker &sst) {
                if (idx >= 0) step(sst, remaining - 1, total);
                else step(sst, remaining, total);
              });
            }
          };
          Loop::step(s, toDiscard, toDiscard);
        });
      }));
  library["c_internal_reflection"] = internalReflection;

  Card strokeTheFadingEmbers("Stroke the fading embers", 0, 2, 0, 0, 'U');
  strokeTheFadingEmbers.setText("Add RR RR.");
  strokeTheFadingEmbers.addEffect(std::make_unique<AddManaEffect>(4, 0, 0));
  library["c_stroke_the_fading_embers"] = strokeTheFadingEmbers;

  Card radiantDecay("Radiant Decay", 0, 1, 0, 1, 'U');
  radiantDecay.setText("Exile a card from your graveyard. Add mana equal to "
                       "its cost, the generic is red.");
  radiantDecay.addEffect(
      std::make_unique<LambdaEffect>([](RoundTracker &state) {
        auto &grave = state.getGraveyard();
        if (grave.getCards().empty())
          return;
        state.requestCardSelection(
            "Select card to decay", grave.getCards(),
            [](int idx, RoundTracker &st) mutable {
              auto &grv = st.getGraveyard();
              auto &grvCards = grv.getCards();
              if (idx >= 0 && idx < (int)grvCards.size()) {
                auto target = grvCards[idx];
                st.addMana(target->getRedCost() + target->getGenericCost(),
                           target->getBlueCost(), target->getGreenCost());
                st.getExile().addCard(target);
                grv.removeCard(idx);
              }
            });
      }));
  library["c_radiant_decay"] = radiantDecay;

  // --- Rares and Legendaries ---
  Card recallWisdom("Recall Wisdom", 0, 0, 1, 0, 'R');
  recallWisdom.setText("Draw 3.");
  recallWisdom.addEffect(std::make_unique<DrawCardEffect>(3));
  library["c_recallWisdom"] = recallWisdom;

  Card ponderTheSilence("Ponder the silence", 0, 0, 1, 0, 'R');
  ponderTheSilence.setText("Draw a card for each echo.");
  ponderTheSilence.addEffect(
      std::make_unique<StormEffect>((std::make_unique<DrawCardEffect>(1))));
  library["c_ponderTheSilence"] = ponderTheSilence;

  Card glimpseWCHB("Glimpse what could have been", 0, 0, 0, 1, 'C');
  glimpseWCHB.setText("Draw 2. Add one light of any color. Discard 3.");
  glimpseWCHB.addEffect(std::make_unique<DrawCardEffect>(2));
  glimpseWCHB.addEffect(std::make_unique<AddManaEffect>(0, 0, 0, 1));
  glimpseWCHB.addEffect(std::make_unique<DiscardEffect>(3));
  library["c_glimpseWCHB"] = glimpseWCHB;

  Card doomsday("Doomsday", 0, 0, 2, 1, 'L');
  doomsday.setText(
      "Search your deck and graveyard for five cards and exile the rest. Put "
      "the chosen cards on top of your deck in any order.");
  doomsday.addEffect(std::make_unique<LambdaEffect>([](RoundTracker &state) {
    auto &grave = state.getGraveyard();
    auto &deck = state.getDeck();
    while (!grave.getCards().empty())
      deck.addCard(grave.popTopCard());
    std::vector<std::shared_ptr<Card>> purgatory;
    while (!deck.getCards().empty())
      purgatory.push_back(deck.popTopCard());

    struct DoomsdayLoop {
      static void step(RoundTracker &s, std::vector<std::shared_ptr<Card>> purg,
                       std::vector<std::shared_ptr<Card>> chosen, int needed) {
        if (chosen.size() >= static_cast<std::size_t>(needed) || purg.empty()) {
          for (auto &leftover : purg)
            s.getExile().addCard(leftover);
          for (auto &c : chosen)
            s.getDeck().addCard(c);
          return;
        }
        std::string promptMsg = "Pick " + std::to_string(chosen.size() + 1) +
                                " of " + std::to_string(needed);
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
  library["c_doomsday"] = doomsday;

  Card symphonyOfDust("Symphony of Dust", 1, 1, 0, 0, 'R');
  symphonyOfDust.setText("Exile your graveyard. Score 5 for each exiled card.");
  symphonyOfDust.addEffect(
      std::make_unique<LambdaEffect>([](RoundTracker &state) {
        auto &grave = state.getGraveyard();
        auto &exile = state.getExile();
        int cardsToExile = (int)grave.getCards().size();
        while (!grave.getCards().empty())
          exile.addCard(grave.popTopCard());
        state.addScore(5 * cardsToExile);
      }));
  library["c_symphonyOfDust"] = symphonyOfDust;

  Card dreamingOfThePast("Dreaming of the past", 2, 0, 0, 1, 'R');
  dreamingOfThePast.setText(
      "Move your graveyard to your hand. Until end of round, whenever a card "
      "goes to the graveyard, exile it.");
  dreamingOfThePast.addEffect(
      std::make_unique<LambdaEffect>([](RoundTracker &state) {
        auto &grave = state.getGraveyard();
        auto &hand = state.getHand();
        while (!grave.getCards().empty())
          hand.addCard(grave.popTopCard());
        state.addStatus(std::make_unique<GlobalExileStatus>());
      }));
  library["c_dreaming_of_the_past"] = dreamingOfThePast;

  Card totalInternalReflection("Total internal Reflection", 0, 2, 0, 1, 'L');
  totalInternalReflection.setText(
      "Copy the next spell you cast for each echo you have");
  totalInternalReflection.addEffect(
      std::make_unique<LambdaEffect>([](RoundTracker &state) {
        state.addStatus(std::make_unique<EchoStatus>(1));
      }));
  library["c_total_internal_reflection"] = totalInternalReflection;

  Card lookUponMyWorks("Look Upon My Works", 2, 1, 0, 0, 'R');
  lookUponMyWorks.setText("Pay X light. Score 10 for each light paid.");
  lookUponMyWorks.addEffect(
      std::make_unique<LambdaEffect>([](RoundTracker &state) {
        state.requestXPrompt([](int x, RoundTracker &s) {
          if (s.getManaPool().canAfford(0, 0, 0, x)) {
            s.getManaPool().spendMana(0, 0, 0, x);
            s.addScore(x * 10);
          }
        });
      }));
  library["c_look_upon_my_works"] = lookUponMyWorks;

  Card incandescentWrath("Incandescent Wrath", 3, 0, 1, 0, 'R');
  incandescentWrath.setText("Discard your hand, then draw that many cards. "
                            "Score 5 for each discarded card.");
  incandescentWrath.addEffect(
      std::make_unique<LambdaEffect>([](RoundTracker &state) {
        auto &hand = state.getHand();
        auto &grave = state.getGraveyard();
        int discardedCount = (int)hand.getCards().size();
        for (int i = discardedCount - 1; i >= 0; --i)
          hand.moveCardTo(i, grave);
        state.addScore(5 * discardedCount);
        for (int i = 0; i < discardedCount; i++)
          state.drawCard();
      }));
  library["c_incandescent_wrath"] = incandescentWrath;

  Card absoluteKnowledge("Absolute knowledge", 0, 0, 3, 0, 'R');
  absoluteKnowledge.setText(
      "If your deck is empty, score 200, otherwise draw 2.");
  absoluteKnowledge.addEffect(
      std::make_unique<LambdaEffect>([](RoundTracker &state) {
        if (state.getDeck().getCards().empty())
          state.addScore(200);
        else
          state.drawCards(2);
      }));
  library["c_absolute_knowledge"] = absoluteKnowledge;

  Card digTroughMemories("Dig through memories", 1, 0, 0, 1, 'U');
  digTroughMemories.setText("Look at the top 5 cards of your deck. Put 2 into your hand, and move the rest to your GY.");
  digTroughMemories.addEffect(std::make_unique<LambdaEffect>([](RoundTracker &state) {
    auto &deck = state.getDeck();
    std::vector<std::shared_ptr<Card>> purgatory;
    for (int i = 0; i < 5; ++i) {
        if (deck.getCards().empty()) break;
        purgatory.push_back(deck.popTopCard());
    }
    
    struct DigLoop {
        static void step(RoundTracker &s, std::vector<std::shared_ptr<Card>> purg, int needed) {
            if (purg.empty() || needed <= 0) {
                for (auto &leftover : purg) s.getGraveyard().addCard(leftover);
                return;
            }
            std::string promptMsg = "Pick a card for your hand (" + std::to_string(needed) + " left)";
            s.requestCardSelection(promptMsg, purg, [purg, needed](int idx, RoundTracker &st) mutable {
                if (idx >= 0 && idx < (int)purg.size()) {
                    st.getHand().addCard(purg[idx]);
                    purg.erase(purg.begin() + idx);
                    step(st, purg, needed - 1);
                } else {
                    step(st, purg, 0); 
                }
            });
        }
    };
    DigLoop::step(state, std::move(purgatory), 2);
  }));
  library["c_digTroughMemories"] = digTroughMemories;

  Card sparkOfGenesis("Spark of Genesis", 0, 1, 2, 0, 'R'); // 1 Red, 2 Blue
  sparkOfGenesis.setText(
      "discard your hand. Draw 7 cards. Double your echo count.");
  sparkOfGenesis.addEffect(
      std::make_unique<LambdaEffect>([](RoundTracker &state) {
        auto &grave = state.getGraveyard();
        auto &deck = state.getDeck();
        int gySize = (int)grave.getCards().size();
        for (int i = gySize - 1; i >= 0; i--)
          grave.moveCardTo(i, deck);
        deck.shuffle();
        state.drawCards(7);
        state.setStormCount(state.getStormCount() * 2);
      }));
  library["c_spark_of_genesis"] = sparkOfGenesis;

  Card fleurDuNeant("Fleur du neant", 0, 0, 0, 0, 'L');
  fleurDuNeant.setText("Add 3 light of any color");
  fleurDuNeant.addEffect(std::make_unique<AddManaEffect>(0, 0, 0, 3));
  library["c_fleurDuNeant"] = fleurDuNeant;
}

/// @brief creates a card based on id
/// @param cardID the cardID
/// @return pointer to the card, to add to deck
std::shared_ptr<Card> CardDatabase::createCard(const std::string &cardID) {
  if (library.find(cardID) != library.end())
    return std::make_shared<Card>(library[cardID]);
  throw DatabaseException("Card ID " + cardID + " not found!");
}

std::shared_ptr<Card> CardDatabase::getTrueRandomCard() {
  if (library.empty())
    throw DatabaseException("Attempted to get random card from empty database!");
  int randomIndex = RNG::range(0, (int)library.size() - 1);
  auto rCard = library.begin();
  std::advance(rCard, randomIndex);
  return std::make_shared<Card>(rCard->second);
}

std::shared_ptr<Card> CardDatabase::getRandomCard() {
  if (library.empty())
    throw DatabaseException("Attempted to get random card from empty database!");
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
    throw DatabaseException("No draftable cards in the database!");
  }

  int winningTicket = RNG::range(1, totalWeight);
  int currentWeight = 0;
  for (const auto &pair : library) {
    char r = pair.second.getRarity();
    if (r == 'B')
      continue;
    if (r == 'C')
      currentWeight += 60;
    if (r == 'U')
      currentWeight += 30;
    if (r == 'R')
      currentWeight += 10;
    if (r == 'L')
      currentWeight += 1;
    if (currentWeight >= winningTicket)
      return std::make_shared<Card>(pair.second);
  }
  return nullptr; // Fallback, error handling
}
