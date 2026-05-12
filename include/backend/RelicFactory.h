#pragma once
#include <iostream>
#include <map>
#include <memory>
#include <string>

#include "Relic.h"

// i lied its a database
class RelicDatabase {
   private:
	std::map<std::string, std::shared_ptr<IRelic>> registry;

	// Private constructor for Singleton
	// singleton slop
	RelicDatabase();

   public:
	static RelicDatabase& getInstance();

	RelicDatabase(const RelicDatabase&) = delete;
	RelicDatabase& operator=(const RelicDatabase&) = delete;

	void loadAllRelics();

	std::shared_ptr<IRelic> getRelic(const std::string& id);

	std::shared_ptr<IRelic> getRandomRelic();
	std::shared_ptr<IRelic> getRandomRareOrLegendaryRelic();
};
