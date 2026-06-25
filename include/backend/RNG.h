#include <random>
#include <type_traits>
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
	template <typename T, typename U>
	static auto range(const T min, const U max) -> std::common_type_t<T, U> {
		using CommonT = std::common_type_t<T, U>;
		if constexpr (std::is_integral_v<CommonT>) {
			std::uniform_int_distribution<CommonT> dist(min, max);
			return dist(getEngine());
		} else {
			std::uniform_real_distribution<CommonT> dist(min, max);
			return dist(getEngine());
		}
	}

	static std::mt19937& engine() { return getEngine(); }
};
