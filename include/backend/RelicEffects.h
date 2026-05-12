#pragma once
#include <memory>

#include "Effect.h"	 
#include "Relic.h"

class OnManaRelic : public IRelic {
   private:
	std::string name;
	std::string description;
	char rarity;
	int multiplier;
	std::unique_ptr<IEffect> triggerEffect;	 
   public:
	OnManaRelic(const std::string& n, const std::string& desc, const char r, const int mult = 1,std::unique_ptr<IEffect> effect = nullptr);
	std::string getName() const override;
	std::string getDescription() const override;
	char getRarity() const override;
	void onManaAdded(int& r, int& b, int& g, RoundTracker& state) override;
	std::shared_ptr<IRelic> clone() const override;
};

class OnCastTriggerRelic : public IRelic {
   private:
	std::string name;
	std::string description;
	char rarity;
	std::unique_ptr<IEffect> triggeredEffect;

   public:
	OnCastTriggerRelic(const std::string& n, const std::string& desc, const char r,std::unique_ptr<IEffect> effect);
	std::string getName() const override;
	std::string getDescription() const override;
	char getRarity() const override;
	void onCardPlayed(RoundTracker& state) override;
	std::shared_ptr<IRelic> clone() const override;
};

class OnDamageRelic : public IRelic {
   private:
	std::string name, description;
	char rarity;
	int flatBonus;							 // The modifier
	std::unique_ptr<IEffect> triggerEffect;	 // The optional trigger

   public:
	OnDamageRelic(std::string n, std::string desc, char r, int bonus = 0,std::unique_ptr<IEffect> effect = nullptr);
	std::string getName() const override;
	std::string getDescription() const override;
	char getRarity() const override;
	void onDamageDealt(int& damage, RoundTracker& state) override;
	std::shared_ptr<IRelic> clone() const override;
};

class OnDrawRelic : public IRelic {
   private:
	std::string name, description;
	char rarity;
	std::unique_ptr<IEffect> triggerEffect;

   public:
	OnDrawRelic(std::string n, std::string desc, char r, std::unique_ptr<IEffect> effect);
	std::string getName() const override;
	std::string getDescription() const override;
	char getRarity() const override;
	void onCardDrawn(RoundTracker& state) override;
	std::shared_ptr<IRelic> clone() const override;
};

class OnRoundStartRelic : public IRelic {
   private:
	std::string name, description;
	char rarity;
	std::unique_ptr<IEffect> triggerEffect;

   public:
	OnRoundStartRelic(std::string n, std::string desc, char r, std::unique_ptr<IEffect> effect);
	std::string getName() const override;
	std::string getDescription() const override;
	char getRarity() const override;
	void onRoundStart(RoundTracker& state) override;
	std::shared_ptr<IRelic> clone() const override;
};

class OnRoundEndRelic : public IRelic {
   private:
	std::string name, description;
	char rarity;
	std::unique_ptr<IEffect> triggerEffect;

   public:
	OnRoundEndRelic(std::string n, std::string desc, char r, std::unique_ptr<IEffect> effect);
	std::string getName() const override;
	std::string getDescription() const override;
	char getRarity() const override;
	void onRoundEnd(RoundTracker& state) override;
	std::shared_ptr<IRelic> clone() const override;
};

class CustomManaRelic : public IRelic {
   private:
	std::string name, description;
	char rarity;
	std::function<void(int&, int&, int&, RoundTracker&)> manaAction;

   public:
	CustomManaRelic(std::string n, std::string desc, char r, std::function<void(int&, int&, int&, RoundTracker&)> action);
	std::string getName() const override;
	std::string getDescription() const override;
	char getRarity() const override;
	void onManaAdded(int& r, int& b, int& g, RoundTracker& state) override;
	std::shared_ptr<IRelic> clone() const override;
};

class SympatheticLodestoneRelic : public IRelic {
   private:
	std::shared_ptr<IRelic> getTargetRelic(RoundTracker& state) const;

   public:
	std::string getName() const override;
	std::string getDescription() const override;
	char getRarity() const override;
	void onRoundStart(RoundTracker& state) override;
	void onRoundEnd(RoundTracker& state) override;
	void onCardPlayed(RoundTracker& state) override;
	void onCardDrawn(RoundTracker& state) override;
	void onManaAdded(int& r, int& b, int& g, RoundTracker& state) override;
	void onDamageDealt(int& damage, RoundTracker& state) override;	
	void activate(RoundTracker& state) override;
	std::shared_ptr<IRelic> clone() const override;
}; //antipattern? yes
//can i figure out any other way? no

class ActiveRelic : public IRelic {
   private:
	std::string name, description;
	char rarity;
	std::function<void(RoundTracker&)> activeAction;

   public:
	ActiveRelic(std::string n, std::string desc, char r, std::function<void(RoundTracker&)> action);
	std::string getName() const override;
	std::string getDescription() const override;
	char getRarity() const override;
	bool isActivatable() const override { return true; }
	void activate(RoundTracker& state) override;
	std::shared_ptr<IRelic> clone() const override;
};