#include "LightPool.h"
#include <cctype>

void ManaPool::addMana(int r, int b, int g) {
	//expect it to go trough here 99% of times
	if (capacity == -1) {red += r; blue += b; green += g;return;}
	int spaceLeft = capacity - getTotalMana();
	int addR = std::min(r, spaceLeft); spaceLeft -= addR;
	int addB = std::min(b, spaceLeft); spaceLeft -= addB;
	int addG = std::min(g, spaceLeft); spaceLeft -= addG;
	red += addR; blue += addB; green += addG;
}

void ManaPool::addManaByColor(char color, int amount) {
    color = std::toupper(color);
	if (capacity != -1) {
		int spaceLeft = capacity - getTotalMana();
		if (amount > spaceLeft) amount = spaceLeft;
		if (amount <= 0) return;
	}
    switch (color) {
        case 'R': this->red += amount; break;
        case 'B': this->blue += amount; break;
        case 'G': this->green += amount; break;
        default: break;
    }
}

void ManaPool::clearPool() {
	red = 0;
	blue = 0;
	green = 0;
}

bool ManaPool::canAfford(const int costR, const int costB, const int costG,const int costGeneric) const {
	if (red < costR || blue < costB || green < costG) return false;
	const int leftoverR = red - costR;
	const int leftoverB = blue - costB;
	const int leftoverG = green - costG;
	const int totalLeftover = leftoverR + leftoverB + leftoverG;
	return totalLeftover >= costGeneric;
}


void ManaPool::spendMana(const int costR, const int costB, const int costG, const int costGeneric) {
	red -= costR;
	blue -= costB;
	green -= costG;
	if (costGeneric > 0) {
		// If only one color has mana left, auto-spend it
		int nonZeroColors = (red > 0 ? 1 : 0) + (blue > 0 ? 1 : 0) + (green > 0 ? 1 : 0);
		if (nonZeroColors <= 1) {
			int genericRemaining = costGeneric;
			int* const vars[3] = {&green, &blue, &red};
			for (int i = 0; i < 3; i++) 
				while (genericRemaining > 0 && *vars[i] > 0) {(*vars[i])--;genericRemaining--;}
		} else {
			pendingGenericSpend = costGeneric;
		}
	}
}
