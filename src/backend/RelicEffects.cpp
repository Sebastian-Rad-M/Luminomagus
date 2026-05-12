#include "RelicEffects.h"

//Q: would it be better to have non-virtual methods in IRelic?

// --- Constructors ---
OnManaRelic::OnManaRelic(const std::string& n, const std::string& desc, const char r,const int mult, std::unique_ptr<IEffect> effect): name(std::move(n)),description(std::move(desc)),rarity(r),multiplier(mult),triggerEffect(std::move(effect)) {}
OnCastTriggerRelic::OnCastTriggerRelic(const std::string& n, const std::string& desc, const char r,std::unique_ptr<IEffect> effect): name(std::move(n)),description(std::move(desc)),rarity(r),triggeredEffect(std::move(effect)) {}
OnDamageRelic::OnDamageRelic(std::string n, std::string desc, char r, int bonus,std::unique_ptr<IEffect> effect): name(n), description(desc), rarity(r), flatBonus(bonus), triggerEffect(std::move(effect)) {}
OnDrawRelic::OnDrawRelic(std::string n, std::string desc, char r, std::unique_ptr<IEffect> effect): name(n), description(desc), rarity(r), triggerEffect(std::move(effect)) {}
ActiveRelic::ActiveRelic(std::string n, std::string desc, char r, std::function<void(RoundTracker&)> action): name(std::move(n)), description(std::move(desc)), rarity(r), activeAction(std::move(action)) {}
OnRoundStartRelic::OnRoundStartRelic(std::string n, std::string desc, char r, std::unique_ptr<IEffect> effect): name(n), description(desc), rarity(r), triggerEffect(std::move(effect)) {}
CustomManaRelic::CustomManaRelic(std::string n, std::string desc, char r, std::function<void(int&, int&, int&, RoundTracker&)> action): name(std::move(n)), description(std::move(desc)), rarity(r), manaAction(std::move(action)) {}
OnRoundEndRelic::OnRoundEndRelic(std::string n, std::string desc, char r, std::unique_ptr<IEffect> effect): name(n), description(desc), rarity(r), triggerEffect(std::move(effect)) {}

// -- Implementation ---

// TODO: display relic triggers with Gui for all of these
void OnManaRelic::onManaAdded(int& r, int& b, int& g, RoundTracker& state) {
	bool triggered = false;
	if (r > 0 || b > 0 || g > 0) {
		r *= multiplier;
		b *= multiplier;
		g *= multiplier;
		triggered = true;
	}
	if (triggered && triggerEffect)triggerEffect->resolve(state);
}
void OnCastTriggerRelic::onCardPlayed(RoundTracker& state) {triggeredEffect->resolve(state);}
void OnDamageRelic::onDamageDealt(int& damage, RoundTracker& state) {if (damage > 0) {damage += flatBonus;if (triggerEffect) triggerEffect->resolve(state);}}
void OnDrawRelic::onCardDrawn(RoundTracker& state) {if (triggerEffect) triggerEffect->resolve(state);}
void OnRoundStartRelic::onRoundStart(RoundTracker& state) {if (triggerEffect) triggerEffect->resolve(state);}
void OnRoundEndRelic::onRoundEnd(RoundTracker& state) {if (triggerEffect)triggerEffect->resolve(state);}
void CustomManaRelic::onManaAdded(int& r, int& b, int& g, RoundTracker& state) {if (manaAction) manaAction(r, b, g, state);}
void ActiveRelic::activate(RoundTracker& state) {if (activeAction) activeAction(state);}


std::shared_ptr<IRelic> SympatheticLodestoneRelic::getTargetRelic(RoundTracker& state) const {
	const auto& relics = state.getRelicZone().getRelicZone();
	if (relics.empty()) return nullptr;
	auto target = relics[0];
	if (target.get() == this || target->getName() == "Sympathetic Lodestone") return nullptr;
	return target;
	//Probably the most fun relic i had to code, also insane code upkeep.
}
void SympatheticLodestoneRelic::onRoundStart(RoundTracker& state) {	if (auto target = getTargetRelic(state)) target->onRoundStart(state);}
void SympatheticLodestoneRelic::onRoundEnd(RoundTracker& state) {if (auto target = getTargetRelic(state)) target->onRoundEnd(state);}
void SympatheticLodestoneRelic::onCardPlayed(RoundTracker& state) {	if (auto target = getTargetRelic(state)) target->onCardPlayed(state);}
void SympatheticLodestoneRelic::onCardDrawn(RoundTracker& state) {if (auto target = getTargetRelic(state)) target->onCardDrawn(state);}
void SympatheticLodestoneRelic::onManaAdded(int& r, int& b, int& g, RoundTracker& state) {if (auto target = getTargetRelic(state)) target->onManaAdded(r, b, g, state);}
void SympatheticLodestoneRelic::onDamageDealt(int& damage, RoundTracker& state) {if (auto target = getTargetRelic(state)) target->onDamageDealt(damage, state);}
void SympatheticLodestoneRelic::activate(RoundTracker& state) {if (auto target = getTargetRelic(state)) target->activate(state);}


// --- Getters ---
std::string ActiveRelic::getName() const { return name; }
std::string CustomManaRelic::getName() const { return name; }
std::string OnRoundStartRelic::getName() const { return name; }
std::string OnRoundEndRelic::getName() const { return name; }
std::string OnManaRelic::getName() const { return name; }
std::string OnCastTriggerRelic::getName() const { return name; }
std::string OnDamageRelic::getName() const { return name; }
std::string OnDrawRelic::getName() const { return name; }
std::string SympatheticLodestoneRelic::getName() const { return "Sympathetic Lodestone"; }

std::string ActiveRelic::getDescription() const { return description; }
std::string SympatheticLodestoneRelic::getDescription() const { return "Copy the relic in slot 1"; }
std::string CustomManaRelic::getDescription() const { return description; }
std::string OnRoundEndRelic::getDescription() const { return description; }
std::string OnRoundStartRelic::getDescription() const { return description; }
std::string OnManaRelic::getDescription() const { return description; }
std::string OnCastTriggerRelic::getDescription() const { return description; }
std::string OnDamageRelic::getDescription() const { return description; }
std::string OnDrawRelic::getDescription() const { return description; }

char OnRoundEndRelic::getRarity() const { return rarity; }
char OnRoundStartRelic::getRarity() const { return rarity; }
char OnManaRelic::getRarity() const { return rarity; }
char OnCastTriggerRelic::getRarity() const { return rarity; }
char OnDamageRelic::getRarity() const { return rarity; }
char OnDrawRelic::getRarity() const { return rarity; }
char CustomManaRelic::getRarity() const { return rarity; }
char ActiveRelic::getRarity() const { return rarity; }
char SympatheticLodestoneRelic::getRarity() const { return 'R'; }


std::shared_ptr<IRelic> ActiveRelic::clone() const {return std::make_shared<ActiveRelic>(name, description, rarity, activeAction);}
std::shared_ptr<IRelic> SympatheticLodestoneRelic::clone() const {return std::make_shared<SympatheticLodestoneRelic>();}
std::shared_ptr<IRelic> OnRoundEndRelic::clone() const {return std::make_shared<OnRoundEndRelic>(name, description, rarity, triggerEffect->clone());}
std::shared_ptr<IRelic> CustomManaRelic::clone() const {return std::make_shared<CustomManaRelic>(name, description, rarity, manaAction);}
std::shared_ptr<IRelic> OnManaRelic::clone() const {auto clonedEffect = triggerEffect ? triggerEffect->clone() : nullptr;return std::make_shared<OnManaRelic>(name, description, rarity, multiplier,std::move(clonedEffect));}
std::shared_ptr<IRelic> OnDamageRelic::clone() const {auto clonedEffect = triggerEffect ? triggerEffect->clone() : nullptr;return std::make_shared<OnDamageRelic>(name, description, rarity, flatBonus,std::move(clonedEffect));}
std::shared_ptr<IRelic> OnCastTriggerRelic::clone() const {return std::make_shared<OnCastTriggerRelic>(name, description, rarity,triggeredEffect->clone());}
std::shared_ptr<IRelic> OnDrawRelic::clone() const {return std::make_shared<OnDrawRelic>(name, description, rarity, triggerEffect->clone());}
std::shared_ptr<IRelic> OnRoundStartRelic::clone() const {return std::make_shared<OnRoundStartRelic>(name, description, rarity, triggerEffect->clone());}

