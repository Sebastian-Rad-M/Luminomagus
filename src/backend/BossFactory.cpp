#include "BossFactory.h"
#include "RNG.h"
#include "GameManager.h" 
#include <stdexcept>


std::unique_ptr<IStatus> BossFactory::createBoss(BossType boss) {
    auto generatedBoss = generateBoss(boss);
    if (generatedBoss) generatedBoss->setIsBoss(true); //boss is a status, we need to ensure it is made the boss 
    return generatedBoss;
}
/// @brief dont call this unless you are debuging 
std::unique_ptr<IStatus> BossFactory::generateBoss(BossType boss) {
	switch (boss) {
		//Elite boss: double score req and no relics lol
        case BossType::ABSOLUTE_ZERO: {
			auto status = std::make_unique<LambdaStatus>("Absolute Zero", 1);
			status->setDescription("Target score is doubled. All relics are disabled.");
			status->setScoreAction([](int& targetScore, RoundTracker&) {targetScore *= 2; });
			status->setAppliedAction([](RoundTracker& state) {for (const auto& relic : state.getRelicZone().getRelicZone()) if (relic) relic->setDisabled(true);});
			return status;
		}
		//Elite boss: if hand or light >5 lose lol
		case BossType::LEVIATHAN: {
			auto status = std::make_unique<LambdaStatus>("Leviathan", 1);
			status->setDescription("If you have more than 5 cards in hand or more than 5 mana, you lose.");
			status->setDrawAction([](Card&, RoundTracker& state) {if (state.getHand().getSize() > 5) state.triggerLoss();});
			status->setManaAction([](int, int, int, RoundTracker& state) {if (state.getManaPool().getTotalMana() > 5) state.triggerLoss();});
			return status;
		}
		// Disable first relic
        case BossType::PUNCTUM_CAECUM: {
			auto status = std::make_unique<LambdaStatus>("Punctum Caecum", 1);
			status->setDescription("Your first relic is disabled.");
			status->setAppliedAction([](RoundTracker& state) {
				const auto& playerRelics = state.getRelicZone().getRelicZone();
				if (!playerRelics.empty() && playerRelics[0]) playerRelics[0]->setDisabled(true);
			});
			return status;
		}
		//whenever you draw, mill 2
        case BossType::ACCELERATED_DECAY: {
			auto status = std::make_unique<LambdaStatus>("Accelerated Decay", 1);
			status->setDescription("Whenever you draw a card, mill 2 cards.");
			status->setDrawAction([](Card&, RoundTracker& state) {
				for (int i = 0; i < 2; i++) {
					auto card = state.getDeck().popTopCard();
					if (card) state.getGraveyard().addCard(card);
				}
			});
			return status;
		}
		//discard every 3rd spell
		case BossType::BLEEDING_REALITY: {
			auto spellsCast = std::make_shared<int>(0);
			auto status = std::make_unique<LambdaStatus>("Bleeding Reality", 1);
			status->setDescription("For every 3rd spell cast, you must discard a card.");
			status->setPlayAction([spellsCast](Card&, RoundTracker& state) {
				(*spellsCast)++;
				if (*spellsCast % 3 == 0) state.promptDiscard();
			});
			return status;
		}
		//graveyard goes to exile
		case BossType::AMNESIC_VOID:
			return std::make_unique<GlobalExileStatus>();
		//max mana is 4
        case BossType::OVER_RUNNETH_CUP: {
			auto status = std::make_unique<LambdaStatus>("Over-runneth Cup", 1);
			status->setDescription("Maximum mana capacity is capped at 4.");
			status->setAppliedAction([](RoundTracker& state) {state.getManaPool().setCapacity(4);});
			return status;
		}
		//cant cast 2 spells of the same color in a row
		case BossType::BROKEN_WAVELENGTH: {
			struct LastCast { bool r = false, b = false, g = false, empty = true; };
			auto lastCast = std::make_shared<LastCast>();
			auto status = std::make_unique<LambdaStatus>("Broken Wavelength", 1);
			status->setDescription("Cannot cast two spells of the same color in a row.");
			status->setPlayAction([lastCast](Card& card, RoundTracker&) {
				lastCast->r = card.getRedCost() > 0;
				lastCast->b = card.getBlueCost() > 0;
				lastCast->g = card.getGreenCost() > 0;
				lastCast->empty = false;
			});
			status->setCanPlayAction([lastCast](const Card& card, RoundTracker&) -> bool {
				if (lastCast->empty) return true;
				if (lastCast->r && card.getRedCost() > 0) return false;
				if (lastCast->b && card.getBlueCost() > 0) return false;
				if (lastCast->g && card.getGreenCost() > 0) return false;
				return true;
			});
			return status;
		}
		//elite boss, cant play even non zero costed cards 
		case BossType::CROOKED_SCALE: {
			auto status = std::make_unique<LambdaStatus>("Crooked Scale", 1);
			status->setDescription("Cannot play cards with an even total cost.");
			status->setCanPlayAction([](Card& card, RoundTracker&) -> bool {
				int totalCost = card.getGenericCost() + card.getRedCost() + card.getBlueCost() + card.getGreenCost(); 
				if (totalCost > 0 && totalCost % 2 == 0) return false;
				return true;
			});
			return status;
		}
		//increase score target by GYsize*2
		case BossType::HISTORYS_WEIGHT: {
			auto status = std::make_unique<LambdaStatus>("History's Weight");
			status->setDescription("Target score is increased by twice your graveyard size.");
			status->setScoreAction([](int& targetScore, RoundTracker& state) {
				int graveSize = state.getGraveyard().getSize(); 
				targetScore += graveSize * 2;
			});
			return status;
		}
		//all spells after 3 cost 1 more to cast
		case BossType::FOG_OF_BEING: {
            auto cardsPlayed = std::make_shared<int>(0); 
            auto status = std::make_unique<LambdaStatus>("Fog of Being");
			status->setDescription("Spells cost 1 generic mana more after playing 3 cards.");
            status->setPlayAction([cardsPlayed](Card&, RoundTracker&) {(*cardsPlayed)++;});
            status->setCostAction([cardsPlayed](int& , int& , int& , int& generic, RoundTracker&) { 
                if (*cardsPlayed >= 3) generic += 1;
            });
            return status;
        }
		//max 15 spells in a round
        case BossType::BRITTLE_EXISTENCE: {
            auto status = std::make_unique<LambdaStatus>("Brittle Existence", 1);
			status->setDescription("You lose if you cast 15 or more spells in a single round.");
            status->setPlayAction([](Card&, RoundTracker& state) { 
                if (state.getStormCount() >= 15) state.triggerLoss();
            });
            return status;
        }
		//Elite: if storm excedes money, lose
        case BossType::FORCE_OF_GREED: {
            auto status = std::make_unique<LambdaStatus>("Force of Greed", 1);
			status->setDescription("If your storm count exceeds your current gold, you lose.");
            status->setPlayAction([](Card&, RoundTracker& state) {
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
	
	if (rarity == 'R') 	pool = {BossType::OVER_RUNNETH_CUP, BossType::BROKEN_WAVELENGTH, BossType::HISTORYS_WEIGHT, BossType::ACCELERATED_DECAY,BossType::BLEEDING_REALITY,BossType::AMNESIC_VOID, BossType::FOG_OF_BEING, BossType::BRITTLE_EXISTENCE,BossType::PUNCTUM_CAECUM};
		else if (rarity == 'E') pool = { BossType::ABSOLUTE_ZERO, BossType::LEVIATHAN, BossType::CROOKED_SCALE, BossType::FORCE_OF_GREED};
			 else if (rarity == 'X') pool = { };
	 				else throw std::invalid_argument("Unknown Boss Rarity");
	std::uniform_int_distribution<size_t> dist(0, pool.size() - 1);
	return createBoss(pool[dist(RNG::engine())]);
}