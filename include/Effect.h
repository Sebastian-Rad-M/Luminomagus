#pragma once
#include "IEffect.h"
#include "RoundTracker.h"
#include <functional>
enum class CompareOp { LESS_THAN, GREATER_THAN, EQUALS, MODULO_EQUALS_ZERO };

class DrawCardEffect : public IEffect {
   private:
	int amount;

   public:
	explicit DrawCardEffect(int amt) : amount(amt) {}
	void resolve(RoundTracker& state) override;
	std::unique_ptr<IEffect> clone() const override;
};

class DiscardEffect : public IEffect {
   private:
	int amount;

   public:
	explicit DiscardEffect(int amt) : amount(amt) {}
	void resolve(RoundTracker& state) override;
	std::unique_ptr<IEffect> clone() const override;
};

class AddManaEffect : public IEffect {
   private:
	int red, blue, green,anycol;

   public:
	AddManaEffect(int r, int b, int g) : red(r), blue(b), green(g),anycol(0) {}
	AddManaEffect(int r, int b, int g,int ac) : red(r), blue(b), green(g),anycol(ac) {}
	void resolve(RoundTracker& state) override;
	std::unique_ptr<IEffect> clone() const override;
};

class Score : public IEffect {
   private:
	int baseScore;

   public:
	Score(int score) : baseScore(score) {}
	void resolve(RoundTracker& state) override;
	std::unique_ptr<IEffect> clone() const override;
};

class StormEffect : public IEffect {
   private:
	std::unique_ptr<IEffect> baseEffect; 
   public:
	StormEffect(std::unique_ptr<IEffect> effect) : baseEffect(std::move(effect)) {}
	void resolve(RoundTracker& state) override;
	std::unique_ptr<IEffect> clone() const override;
};

using ConditionFunc = std::function<bool(const RoundTracker&)>;
class ConditionalEffect : public IEffect {
private:
    ConditionFunc condition;
    std::unique_ptr<IEffect> effectToResolve; 

public:
    ConditionalEffect(ConditionFunc cond, std::unique_ptr<IEffect> effect) : condition(std::move(cond)), effectToResolve(std::move(effect)) {}
    void resolve(RoundTracker& state) override ;
	std::unique_ptr<IEffect> clone() const override;
};
class LambdaEffect : public IEffect {
private:
    std::function<void(RoundTracker&)> action;

public:
    LambdaEffect(std::function<void(RoundTracker&)> act) 
        : action(std::move(act)) {}

    void resolve(RoundTracker& state) override {
        action(state);
		}
	std::unique_ptr<IEffect> clone() const override;
    
};

class ApplyStatusEffect : public IEffect {
   private:
	std::unique_ptr<IStatus> statusToApply;

   public:
	ApplyStatusEffect(std::unique_ptr<IStatus> status) : statusToApply(std::move(status)) {}
	void resolve(RoundTracker& state) override;
	std::unique_ptr<IEffect> clone() const override;
};

class GraveyardScaleEffect : public IEffect {
   private:
	std::string searchedName;
	std::unique_ptr<IEffect> baseEffect;

   public:
	GraveyardScaleEffect(const std::string& name, std::unique_ptr<IEffect> effect)
		: searchedName(name), baseEffect(std::move(effect)) {}
	void resolve(RoundTracker& state) override;
	std::unique_ptr<IEffect> clone() const override;
};
