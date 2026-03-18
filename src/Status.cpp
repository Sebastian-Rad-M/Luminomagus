#include "RoundTracker.h"
#include "Status.h"


void OverchargeStatus::onCardPlayed(Card& card, RoundTracker& state) {
    if (charges > 0) {
        std::cout << " ![Overcharge] Copy spell!\n";
        card.play(state); // Now the compiler knows what .play() is!
        charges--;
    }
}

void EchoStatus::onCardPlayed(Card& card, RoundTracker& state){
        if (charges > 0) {
            int copies = state.getStormCount();        
            if (copies > 0)  for (int i = 0; i < copies; i++) card.play(state);  
            charges--;
        }
    }

void GlobalExileStatus::modifyDestination(CardZone*& destination, RoundTracker& state){
        if (charges > 0) {
			destination = &state.getExile(); 
            charges--;       }
    }