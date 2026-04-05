#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "Card.h"
#include "Effect.h"
#include "RoundTracker.h"

Card::Card()
	: name(""), genericCost(0), redCost(0), blueCost(0), greenCost(0), rarity('C'), mod(nullptr) {}

Card::Card(const std::string& name, const int genericCost, const int redCost, const int blueCost,const int greenCost, const char rarity, const std::shared_ptr<modifier> mod)
	: name(std::move(name)),genericCost(genericCost),redCost(redCost),blueCost(blueCost),greenCost(greenCost),rarity(rarity),mod(mod) {}

Card::Card(const Card& other)
	: name(other.name), text(other.text),genericCost(other.genericCost),redCost(other.redCost),blueCost(other.blueCost),greenCost(other.greenCost),rarity(other.rarity),mod(other.mod) 
{for (const auto& effect : other.effects) effects.push_back(effect->clone());}

Card& Card::operator=(const Card& other) {
	if (this != &other) {
		name = other.name;
		text = other.text;
		genericCost = other.genericCost;
		redCost = other.redCost;
		blueCost = other.blueCost;
		greenCost = other.greenCost;
		rarity = other.rarity;
		mod = other.mod;
		effects.clear();
		for (const auto& effect : other.effects) effects.push_back(effect->clone());
	}
	return *this;
}

// we love smart pointers
Card::~Card() = default;  

std::ostream& operator<<(std::ostream& os, const Card& c) {
	os << "[" << c.rarity << "] " << c.name << " (Cost: " << c.genericCost << "G, " << c.redCost
	   << "R, " << c.blueCost << "B, " << c.greenCost << "G)";
	return os;
}
/// @brief on play iterate trough each effect
/// @param state is the round tracker usually 
void Card::play(RoundTracker& state) {
	for (const auto& effect : effects) effect->resolve(state);
}

/// @brief called for each effect, one at a time
/// @param effect the effect to be added
void Card::addEffect(std::unique_ptr<IEffect> effect) { effects.push_back(std::move(effect)); }

//getters
const std::string& Card::getName() const { return name; }
int Card::getGenericCost() const { return genericCost; }
int Card::getRedCost() const { return redCost; }
int Card::getBlueCost() const { return blueCost; }
int Card::getGreenCost() const { return greenCost; }
char Card::getRarity() const { return rarity; }
int Card::totalCost() const { return genericCost + redCost + blueCost + greenCost; }
void Card::setText(std::string t) {text=t;}
std::string Card::getText() const {return text;}
char Card::getFrame() const {return frame;}