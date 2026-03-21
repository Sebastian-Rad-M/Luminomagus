#include "BossFactory.h"
#include "RNG.h"
#include "GameManager.h" 
#include <stdexcept>

std::unique_ptr<IStatus> BossFactory::createBoss(BossType boss) {
    auto generatedBoss = generateBoss(boss);
    if (generatedBoss) {
        generatedBoss->setIsBoss(true); 
    }
    return generatedBoss;
}

std::unique_ptr<IStatus> BossFactory::generateBoss(BossType boss) {
	switch (boss) {
        case BossType::ABSOLUTE_ZERO: {
			auto status = std::make_unique<LambdaStatus>("Absolute Zero", 1);
			status->setScoreAction([](int& targetScore, RoundTracker&) {
				targetScore *= 2; 
			});
			status->setAppliedAction([](RoundTracker& state) {
				for (const auto& relic : state.getRelicZone().getRelicZone()) if (relic) relic->setDisabled(true);
			});
			return status;
		}

		case BossType::LEVIATHAN: {
			auto status = std::make_unique<LambdaStatus>("Leviathan", 1);
			
			// Tripwire 1: Max Hand Size
			status->setDrawAction([](Card&, RoundTracker& state) {
				if (state.getHand().getSize() > 5) state.triggerLoss();
			});
			
			// Tripwire 2: Max Unspent Light
			status->setManaAction([](int, int, int, RoundTracker& state) {
				if (state.getManaPool().getTotalMana() > 5) state.triggerLoss();
			});
			
			return status;
		}

        case BossType::PUNCTUM_CAECUM: {
			auto status = std::make_unique<LambdaStatus>("Punctum Caecum", 1);
			status->setAppliedAction([](RoundTracker& state) {
				const auto& playerRelics = state.getRelicZone().getRelicZone();
				if (!playerRelics.empty() && playerRelics[0]) playerRelics[0]->setDisabled(true);
			});
			return status;
		}
        case BossType::ACCELERATED_DECAY: {
			auto status = std::make_unique<LambdaStatus>("Accelerated Decay", 1);
			status->setDrawAction([](Card&, RoundTracker& state) {
				for (int i = 0; i < 2; i++) {
					auto card = state.getDeck().popTopCard();
					if (card) state.getGraveyard().addCard(card);
					else state.triggerLoss();
				}
			});
			return status;
		}
		case BossType::BLEEDING_REALITY: {
			auto spellsCast = std::make_shared<int>(0);
			auto status = std::make_unique<LambdaStatus>("Bleeding Reality", 1);
			
			status->setPlayAction([spellsCast](Card&, RoundTracker& state) {
				(*spellsCast)++;
				if (*spellsCast % 3 == 0) state.promptDiscard();
			});
			return status;
		}


		case BossType::AMNESIC_VOID:
			return std::make_unique<GlobalExileStatus>(1);
        case BossType::OVER_RUNNETH_CUP: {
			auto status = std::make_unique<LambdaStatus>("Over-runneth Cup", 1);
			status->setAppliedAction([](RoundTracker& state) {
				state.getManaPool().setCapacity(4);
			});
			return status;
		}

		case BossType::BROKEN_WAVELENGTH: {
			struct LastCast { bool r = false, b = false, g = false, empty = true; };
			auto lastCast = std::make_shared<LastCast>();
			auto status = std::make_unique<LambdaStatus>("Broken Wavelength", 1);
			
			status->setPlayAction([lastCast](Card& card, RoundTracker&) {
				lastCast->r = card.getRedCost() > 0;
				lastCast->b = card.getBlueCost() > 0;
				lastCast->g = card.getGreenCost() > 0;
				lastCast->empty = false;
			});
			status->setCanPlayAction([lastCast](Card& card, RoundTracker&) -> bool {
				if (lastCast->empty) return true;
				if (lastCast->r && card.getRedCost() > 0) return false;
				if (lastCast->b && card.getBlueCost() > 0) return false;
				if (lastCast->g && card.getGreenCost() > 0) return false;
				return true;
			});
			return status;
		}

		case BossType::CROOKED_SCALE: {
			auto status = std::make_unique<LambdaStatus>("Crooked Scale", 1);
			status->setCanPlayAction([](Card& card, RoundTracker&) -> bool {
				int totalCost = card.getGenericCost() + card.getRedCost() + card.getBlueCost() + card.getGreenCost(); 
				if (totalCost > 0 && totalCost % 2 == 0) return false;
				return true;
			});
			return status;
		}

		case BossType::HISTORYS_WEIGHT: {
			auto status = std::make_unique<LambdaStatus>("History's Weight", 1);
			status->setScoreAction([](int& targetScore, RoundTracker& state) {
				int graveSize = state.getGraveyard().getSize(); 
				targetScore += (graveSize / 5) * 10;
			});
			return status;
		}


		case BossType::FOG_OF_BEING: {
            auto cardsPlayed = std::make_shared<int>(0); 
            auto status = std::make_unique<LambdaStatus>("Fog of Being", 1);
            status->setPlayAction([cardsPlayed](Card&, RoundTracker&) {
                (*cardsPlayed)++;
            });
            status->setCostAction([cardsPlayed](int& , int& , int& , int& generic, RoundTracker&) { 
                if (*cardsPlayed >= 3) generic += 1;
            });
            return status;
        }

        case BossType::BRITTLE_EXISTENCE: {
            auto status = std::make_unique<LambdaStatus>("Brittle Existence", 1);
            status->setPlayAction([](Card&, RoundTracker& state) { 
                if (state.getStormCount() >= 15) state.triggerLoss();
            });
            return status;
        }

        case BossType::FORCE_OF_GREED: {
            auto status = std::make_unique<LambdaStatus>("Force of Greed", 1);
            status->setPlayAction([](Card&, RoundTracker& state) {
                // Note: Ensure state.getRun().getPlayer().getGold() exists in your Player class
                int currentGold = state.getRun().getPlayer().getGold(); 
                if (state.getStormCount() > currentGold) state.triggerLoss();
            });
            return status;
        }
		default:
			throw std::invalid_argument("Unknown Boss Type");
	}
}

std::unique_ptr<IStatus> BossFactory::getRandomBoss(char rarity) {
	std::vector<BossType> pool;
	
	if (rarity == 'R') {
		pool = {BossType::OVER_RUNNETH_CUP, BossType::BROKEN_WAVELENGTH, BossType::HISTORYS_WEIGHT, BossType::ACCELERATED_DECAY,BossType::BLEEDING_REALITY,BossType::AMNESIC_VOID, BossType::FOG_OF_BEING, BossType::BRITTLE_EXISTENCE,BossType::PUNCTUM_CAECUM};
	} else if (rarity == 'E') {
		pool = { BossType::ABSOLUTE_ZERO, BossType::LEVIATHAN, BossType::CROOKED_SCALE, BossType::FORCE_OF_GREED};
	} else if (rarity == 'X') {
		pool = { };
	} else {
		throw std::invalid_argument("Unknown Boss Rarity");
	}

	std::uniform_int_distribution<size_t> dist(0, pool.size() - 1);
	return createBoss(pool[dist(RNG::engine())]);
}