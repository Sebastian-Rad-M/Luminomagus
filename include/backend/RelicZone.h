#pragma once
#include <iostream>
#include <memory>
#include <vector>

#include "Relic.h"
class RoundTracker;	 // Forward declaration

class RelicZone {
   private:
	std::vector<std::shared_ptr<IRelic>> relics;
	int capacity;

   public:
	explicit RelicZone(int maxCapacity = 3);
	bool addRelic(std::shared_ptr<IRelic> relic);
	void removeRelic(int index);
	void clearRelicZone() { relics.clear(); }
	
	void triggerOnManaAdded(int& r, int& b, int& g, RoundTracker& state);
	void triggerOnCardPlayed(RoundTracker& state);
	void triggerOnCardDrawn(RoundTracker& state);
	void triggerOnDamageDealt(int& scoredPts, RoundTracker& state);
	void triggerOnRoundStart(RoundTracker& state);
	void triggerOnRoundEnd(RoundTracker& state);
	
	const std::vector<std::shared_ptr<IRelic>>& getRelicZone() const { return relics; }
	};
