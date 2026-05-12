#pragma once
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "Card.h"
#include "CardZone.h"
#include "LightPool.h"
#include "RelicZone.h"
#include "Status.h"
class ActiveRun;

class RoundTracker {
private:
  CardZone deck;
  RelicZone relics;
  CardZone hand, graveyard, exile;
  ManaPool manaPool;
  std::vector<std::unique_ptr<IStatus>> activeStatuses;
  const ActiveRun &currentRun;
  int currentScore;
  int targetScore;
  int stormCount;
  bool isLost = false;

  // Pending mana selections for GUI
  int pendingManaChoices = 0;

  // Pending X value prompt
  std::function<void(int, RoundTracker &)> pendingXCallback = nullptr;

  // Pending Yes/No prompt
  std::function<void(bool, RoundTracker &)> pendingYesNoCallback = nullptr;
  std::string activeYesNoMessage;

  // Pending Card Selection prompt
  std::function<void(int, RoundTracker &)> pendingCardSelectionCallback =
      nullptr;
  std::vector<std::shared_ptr<Card>> selectionChoices;
  std::string selectionPromptMessage;

  // Pending Option Selection prompt
  std::function<void(int, RoundTracker &)> pendingOptionCallback = nullptr;
  std::vector<std::string> optionChoices;
  std::string optionPromptMessage;

  // Pending Hand Click callback (click directly from hand, no modal)
  std::function<void(int, RoundTracker &)> pendingHandClickCallback = nullptr;
  std::string handClickMessage;

  // Status message for the UI prompt box
  std::string promptMessage;

public:
  explicit RoundTracker(const ActiveRun &runData);

  void drawCard();
  void drawCards(int amount);
  void promptDiscard(std::function<void(int, RoundTracker &)> cb = nullptr);

  std::string getPromptMessage() const { return promptMessage; }
  void setPromptMessage(const std::string &msg) { promptMessage = msg; }

  void addStatus(std::unique_ptr<IStatus> status);
  bool isRoundWon() const;
  std::string getActiveBossName() const;
  std::string getActiveBossDescription() const;
  void startNewRound();

  void addScore(int amount);
  void addScoreToTarget(int amount);
  void addMana(int r, int b, int g);
  void addManaByColor(char color);
  int getPendingManaChoices() const { return pendingManaChoices; }
  void requestManaChoices(int count) { pendingManaChoices += count; }

  bool isXPromptActive() const { return pendingXCallback != nullptr; }
  void requestXPrompt(std::function<void(int, RoundTracker &)> cb) {
    pendingXCallback = std::move(cb);
  }
  void resolveXPrompt(int x) {
    if (pendingXCallback) {
      auto cb = std::move(pendingXCallback);
      pendingXCallback = nullptr;
      cb(x, *this);
    }
  }

  bool isYesNoPromptActive() const { return pendingYesNoCallback != nullptr; }
  std::string getYesNoMessage() const { return activeYesNoMessage; }
  void requestYesNoPrompt(std::string msg,
                          std::function<void(bool, RoundTracker &)> cb) {
    activeYesNoMessage = std::move(msg);
    pendingYesNoCallback = std::move(cb);
  }
  void resolveYesNoPrompt(bool result) {
    if (pendingYesNoCallback) {
      auto cb = std::move(pendingYesNoCallback);
      pendingYesNoCallback = nullptr;
      cb(result, *this);
    }
  }

  bool isCardSelectionPromptActive() const {
    return pendingCardSelectionCallback != nullptr;
  }
  std::string getSelectionMessage() const { return selectionPromptMessage; }
  const std::vector<std::shared_ptr<Card>> &getSelectionChoices() const {
    return selectionChoices;
  }

  void requestCardSelection(std::string msg,
                            std::vector<std::shared_ptr<Card>> choices,
                            std::function<void(int, RoundTracker &)> cb) {
    selectionPromptMessage = std::move(msg);
    selectionChoices = std::move(choices);
    pendingCardSelectionCallback = std::move(cb);
  }

  void resolveCardSelection(int index) {
    if (pendingCardSelectionCallback) {
      auto cb = std::move(pendingCardSelectionCallback);
      pendingCardSelectionCallback = nullptr;
      // It's up to the caller to validate the index if needed, or we just pass
      // it blindly
      cb(index, *this);
    }
  }

  bool isOptionPromptActive() const { return pendingOptionCallback != nullptr; }
  std::string getOptionMessage() const { return optionPromptMessage; }
  const std::vector<std::string> &getOptionChoices() const {
    return optionChoices;
  }

  void requestOptionSelection(std::string msg, std::vector<std::string> choices,
                              std::function<void(int, RoundTracker &)> cb) {
    optionPromptMessage = std::move(msg);
    optionChoices = std::move(choices);
    pendingOptionCallback = std::move(cb);
  }

  void resolveOptionSelection(int index) {
    if (pendingOptionCallback) {
      auto cb = std::move(pendingOptionCallback);
      pendingOptionCallback = nullptr;
      cb(index, *this);
    }
  }

  bool isHandClickActive() const { return pendingHandClickCallback != nullptr; }
  std::string getHandClickMessage() const { return handClickMessage; }
  void requestHandClick(std::string msg,
                        std::function<void(int, RoundTracker &)> cb) {
    handClickMessage = std::move(msg);
    pendingHandClickCallback = std::move(cb);
  }
  void resolveHandClick(int index) {
    if (pendingHandClickCallback) {
      auto cb = std::move(pendingHandClickCallback);
      pendingHandClickCallback = nullptr;
      cb(index, *this);
    }
  }

  int getStormCount() const;
  CardZone &getDeck();
  ManaPool &getManaPool();
  CardZone &getGraveyard();
  CardZone &getExile();
  CardZone &getHand();
  RelicZone &getRelicZone();
  void moveHandCardToExile(int index) { hand.moveCardTo(index, exile); }
  void requestHandTarget(std::function<void(int, RoundTracker &)> cb);
  void setupDeck(const CardZone &library, const RelicZone &startingRelics);
  bool playCardFromHand(int index);
  int getCurrentScore() const;
  int getTargetScore() const;
  void setStormCount(int nr);
  bool hasLost() const { return isLost; }
  void triggerLoss() { isLost = true; }
  ActiveRun &getRun() { return const_cast<ActiveRun &>(currentRun); }
};
