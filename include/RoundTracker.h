#pragma once
#include <iostream>
#include <limits>
#include <memory>

#include "Card.h"
#include "CardZone.h"
#include "LightPool.h"
#include "RelicZone.h"
#include "Status.h"
#include "View.h"
#include "IPlayerPrompt.h"
class ActiveRun;

class RoundTracker {
   private:
	CardZone deck;
	RelicZone relics;
	CardZone hand, graveyard, exile;
	ManaPool manaPool;
	std::vector<std::unique_ptr<IStatus>> activeStatuses;
	const ActiveRun& currentRun;
	int currentScore;
	int targetScore;
	int stormCount;
	bool isLost = false;
	IPlayerPrompt* ui;

   public:
	explicit RoundTracker(const ActiveRun& runData, IPlayerPrompt* uiPrompt);
	IPlayerPrompt* getUI() { return ui; }
	void drawCard();
	void drawCards(int amount);
	bool promptDiscard();
	void addStatus(std::unique_ptr<IStatus> status);
	bool isRoundWon() const;

	void startNewRound();

	void addScore(int amount);
	void addScoreToTarget(int amount);
	void addMana(int r, int b, int g);
	static void promptForManaColor(ManaPool& manaPool,int nr=1);
	int getStormCount() const;
	CardZone& getDeck();
	ManaPool& getManaPool();
	CardZone& getGraveyard();
	CardZone& getExile();
	CardZone& getHand();
	RelicZone& getRelicZone();
	void moveHandCardToExile(int index) { hand.moveCardTo(index, exile); }
	int requestHandTarget();
	void setupDeck(const CardZone& library, const RelicZone& startingRelics);
	void printStatus() const;
	bool playCardFromHand(int index);
	int getCurrentScore() const;
	int getTargetScore() const;
	void setStormCount(int nr);
	bool hasLost() const { return isLost; }
	void triggerLoss() { isLost = true; }
};
