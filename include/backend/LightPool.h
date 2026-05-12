#include <iostream>
#include <stdexcept>
#pragma once
class ManaPool {
   private:
	int red;
	int blue;
	int green;
	int capacity = -1;

   public:
	explicit ManaPool(const int r = 0, const int b = 0, const int g = 0) : red(r), blue(b), green(g) {}

	int getRed() const { return red; }
	int getBlue() const { return blue; }
	int getGreen() const { return green; }

	void setCapacity(int c) { capacity = c; }
	int getTotalMana() const { return red + blue + green; }
	
	void addMana(const int r, const int b, const int g);
	void addManaByColor(char color, int amount = 1);
	void clearPool();

	bool canAfford(const int costR, const int costB, const int costG, const int costGeneric) const;

	void spendMana(const int costR, const int costB, const int costG, const int costGeneric);
	// check canAfford before
	// generic is now handled by the GUI — player picks which colors to drain
	int pendingGenericSpend = 0;
	bool hasPendingGenericSpend() const { return pendingGenericSpend > 0; }
	void spendGenericByColor(char color) {
		if (pendingGenericSpend <= 0) return;
		switch (color) {
			case 'R': case 'r': if (red > 0) { red--; pendingGenericSpend--; } break;
			case 'B': case 'b': if (blue > 0) { blue--; pendingGenericSpend--; } break;
			case 'G': case 'g': if (green > 0) { green--; pendingGenericSpend--; } break;
			default: break;
		}
	}
	// for generic We drain Green first, then Blue, then Red (arbitrary priority).

	friend std::ostream& operator<<(std::ostream& os, const ManaPool& pool) {
		os << "[Mana: " << pool.red << "R, " << pool.blue << "B, " << pool.green << "G]";
		return os;
	}
};
