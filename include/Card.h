#pragma once
#include <memory>
#include <string>
#include <vector>

#include "IEffect.h"
class modifier;
class RoundTracker;

class Card {
   private:
	std::string name;
	int genericCost, redCost, blueCost, greenCost;
	std::vector<std::unique_ptr<IEffect>> effects;
	char rarity;//CURL
	std::shared_ptr<modifier> mod = nullptr;

   public:
	Card();
	Card(const std::string& name, const int genericCost = 0, const int redCost = 0,
		 const int blueCost = 0, const int greenCost = 0, const char rarity = 'C',
		 const std::shared_ptr<modifier> mod = nullptr);
	Card(const Card& other);
	Card& operator=(const Card& other);
	~Card();
	friend std::ostream& operator<<(std::ostream& os, const Card& c);

	void play(RoundTracker& state);
	void addEffect(std::unique_ptr<IEffect>);
	const std::string& getName() const;
	int getGenericCost() const;
	int getRedCost() const;
	int getBlueCost() const;
	int getGreenCost() const;
	char getRarity() const;
	int totalCost() const;
};
