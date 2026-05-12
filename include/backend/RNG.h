#include <random>
#pragma once
class RNG {
   private:
	/// goated rng function
	static std::mt19937& getEngine() {
		static std::random_device rd;
		static std::mt19937 gen(rd());
		return gen;
	}

   public:
	static int range(const int min, const int max) {
		std::uniform_int_distribution<> dist(min, max);
		return dist(getEngine());
	}

	static std::mt19937& engine() { return getEngine(); }
};
