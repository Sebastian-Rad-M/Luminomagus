#pragma once
#include "RoundTracker.h"
#include <functional>
#include <iostream>
#include <memory>
#include <string>

enum class CompareOp { LESS_THAN, GREATER_THAN, EQUALS, MODULO_EQUALS_ZERO };

class RoundTracker;
class IEffect {
   public:
	virtual ~IEffect() = default;

	// Non-Virtual Interface (NVI)
	void resolve(RoundTracker& state) { do_resolve(state); }

	// Virtual display
	virtual void print(std::ostream& os) const = 0;

	virtual std::unique_ptr<IEffect> clone() const = 0;

   protected:
	virtual void do_resolve(RoundTracker& state) = 0;
};

inline std::ostream& operator<<(std::ostream& os, const IEffect& effect) {
	effect.print(os);
	return os;
}

class DrawCardEffect : public IEffect {
   private:
	int amount;

   public:
	explicit DrawCardEffect(int amt) : amount(amt) {}
	void print(std::ostream& os) const override { os << "Draw " << amount << " cards"; }
	std::unique_ptr<IEffect> clone() const override;

   protected:
	void do_resolve(RoundTracker& state) override;
};

class DiscardEffect : public IEffect {
   private:
	int amount;

   public:
	explicit DiscardEffect(int amt) : amount(amt) {}
	void print(std::ostream& os) const override { os << "Discard " << amount << " cards"; }
	std::unique_ptr<IEffect> clone() const override;

   protected:
	void do_resolve(RoundTracker& state) override;
};

class AddManaEffect : public IEffect {
   private:
	int red, blue, green, anycol;

   public:
	AddManaEffect(int r, int b, int g) : red(r), blue(b), green(g), anycol(0) {}
	AddManaEffect(int r, int b, int g, int ac) : red(r), blue(b), green(g), anycol(ac) {}
	void print(std::ostream& os) const override {
		os << "Add " << red << "R, " << blue << "B, " << green << "G";
		if (anycol) os << ", " << anycol << " Any";
	}
	std::unique_ptr<IEffect> clone() const override;

   protected:
	void do_resolve(RoundTracker& state) override;
};

class Score : public IEffect {
   private:
	int baseScore;

   public:
	explicit Score(int score) : baseScore(score) {}
	void print(std::ostream& os) const override { os << "Gain " << baseScore << " score"; }
	std::unique_ptr<IEffect> clone() const override;

   protected:
	void do_resolve(RoundTracker& state) override;
};

class StormEffect : public IEffect {
   private:
	std::unique_ptr<IEffect> baseEffect;

   public:
	explicit StormEffect(std::unique_ptr<IEffect> effect) : baseEffect(std::move(effect)) {}
	void print(std::ostream& os) const override { os << "Storm: " << *baseEffect; }
	std::unique_ptr<IEffect> clone() const override;

   protected:
	void do_resolve(RoundTracker& state) override;
};

using ConditionFunc = std::function<bool(const RoundTracker&)>;
class ConditionalEffect : public IEffect {
   private:
	ConditionFunc condition;
	std::unique_ptr<IEffect> effectToResolve;

   public:
	ConditionalEffect(ConditionFunc cond, std::unique_ptr<IEffect> effect)
		: condition(std::move(cond)), effectToResolve(std::move(effect)) {}
	void print(std::ostream& os) const override { os << "If [condition], " << *effectToResolve; }
	std::unique_ptr<IEffect> clone() const override;

   protected:
	void do_resolve(RoundTracker& state) override;
};

class LambdaEffect : public IEffect {
   private:
	std::function<void(RoundTracker&)> action;

   public:
	explicit LambdaEffect(std::function<void(RoundTracker&)> act) : action(std::move(act)) {}

	void print(std::ostream& os) const override { os << "Special effect"; }
	std::unique_ptr<IEffect> clone() const override;

   protected:
	void do_resolve(RoundTracker& state) override { action(state); }
};

class ApplyStatusEffect : public IEffect {
   private:
	std::unique_ptr<IStatus> statusToApply;

   public:
	explicit ApplyStatusEffect(std::unique_ptr<IStatus> status)
		: statusToApply(std::move(status)) {}
	void print(std::ostream& os) const override { os << "Apply status: " << statusToApply->getName(); }
	std::unique_ptr<IEffect> clone() const override;

   protected:
	void do_resolve(RoundTracker& state) override;
};

class GraveyardScaleEffect : public IEffect {
   private:
	std::string searchedName;
	std::unique_ptr<IEffect> baseEffect;

   public:
	GraveyardScaleEffect(const std::string& name, std::unique_ptr<IEffect> effect)
		: searchedName(name), baseEffect(std::move(effect)) {}
	void print(std::ostream& os) const override {
		os << "For each " << searchedName << " in graveyard, " << *baseEffect;
	}
	std::unique_ptr<IEffect> clone() const override;

   protected:
	void do_resolve(RoundTracker& state) override;
};

