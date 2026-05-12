#include <memory>
#include "Effect.h"

void DrawCardEffect::do_resolve(RoundTracker& state) {
	for (int i = 0; i < amount; i++) state.drawCard();
}
// TODO: make it not break.
void DiscardEffect::do_resolve(RoundTracker& state) {
	struct Loop {
		static void step(RoundTracker& s, int remaining) {
			if (remaining <= 0 || s.getHand().getCards().empty()) return;
			s.promptDiscard([remaining](int idx, RoundTracker& st) {
				if (idx >= 0)
					step(st, remaining - 1);
				else
					step(st, remaining);
			});
		}
	};
	Loop::step(state, amount);
}

void AddManaEffect::do_resolve(RoundTracker& state) {
	state.addMana(red, blue, green);
	if (anycol) state.requestManaChoices(anycol);
}

void Score::do_resolve(RoundTracker& state) { state.addScore(baseScore); }

void StormEffect::do_resolve(RoundTracker& state) {
	const int storm = state.getStormCount();
	for (int i = 0; i < storm; i++) baseEffect->resolve(state);
}

void ConditionalEffect::do_resolve(RoundTracker& state) {
	if (condition(state)) effectToResolve->resolve(state);
}

void GraveyardScaleEffect::do_resolve(RoundTracker& state) {
	int count = 0;
	const auto& grave = state.getGraveyard().getCards();
	for (const auto& card : grave)
		if (card->getName() == searchedName) count++;
	if (count > 0)
		for (int i = 0; i < count; i++) baseEffect->resolve(state);
}

void ApplyStatusEffect::do_resolve(RoundTracker& state) { state.addStatus(statusToApply->clone()); }


std::unique_ptr<IEffect> DrawCardEffect::clone() const {return std::make_unique<DrawCardEffect>(*this);}
std::unique_ptr<IEffect> DiscardEffect::clone() const {return std::make_unique<DiscardEffect>(*this);}
std::unique_ptr<IEffect> AddManaEffect::clone() const {return std::make_unique<AddManaEffect>(*this);}
std::unique_ptr<IEffect> Score::clone() const { return std::make_unique<Score>(*this); }
std::unique_ptr<IEffect> StormEffect::clone() const {return std::make_unique<StormEffect>(baseEffect->clone());}
std::unique_ptr<IEffect> ConditionalEffect::clone() const {return std::make_unique< ConditionalEffect>(condition,effectToResolve->clone());}
std::unique_ptr<IEffect> LambdaEffect::clone() const {return std::make_unique<LambdaEffect>(action);}
std::unique_ptr<IEffect> GraveyardScaleEffect::clone() const {return std::make_unique<GraveyardScaleEffect>(searchedName, baseEffect->clone());}
std::unique_ptr<IEffect> ApplyStatusEffect::clone() const {return std::make_unique<ApplyStatusEffect>(statusToApply->clone());}
