#pragma once
#include <memory>
#include "Status.h"

enum class BossType {
    PUNCTUM_CAECUM,
	AMNESIC_VOID,
	FOG_OF_BEING,
	BRITTLE_EXISTENCE,
	FORCE_OF_GREED,
    OVER_RUNNETH_CUP,
    BROKEN_WAVELENGTH,
    CROOKED_SCALE,
    HISTORYS_WEIGHT,
    ACCELERATED_DECAY,
    BLEEDING_REALITY,
    ABSOLUTE_ZERO,
    LEVIATHAN
};
class BossFactory {
    private:
    static std::unique_ptr<IStatus> generateBoss(BossType boss);

   public:
	BossFactory() = delete;
	static std::unique_ptr<IStatus> createBoss(BossType boss);
	
	static std::unique_ptr<IStatus> getRandomBoss(char rarity);
};