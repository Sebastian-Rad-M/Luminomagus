#pragma once
#include <memory>
#include <string>
class RoundTracker;
class Card;

class IRelic {
   public:
	virtual ~IRelic() = default;

	virtual std::string getName() const = 0;
	virtual std::string getDescription() const = 0;
	virtual char getRarity() const = 0;

	virtual std::shared_ptr<IRelic> clone() const = 0;
	// this is Ieffects resolve but for relics
	virtual void onCardPlayed(RoundTracker& /*state*/) {}
	virtual void onCardDrawn(RoundTracker& /*state*/) {}
	virtual void onManaAdded(int& /*r*/, int& /*b*/, int& /*g*/, RoundTracker& /*state*/) {}
	virtual void onDamageDealt(int& /*damage*/, RoundTracker& /*state*/) {}
	virtual void onRoundStart(RoundTracker& /*state*/) {}
	virtual void onRoundEnd(RoundTracker& /*state*/) {}
	virtual bool isActivatable() const { return false; }
	virtual void activate(RoundTracker& /*state*/) {}
};
