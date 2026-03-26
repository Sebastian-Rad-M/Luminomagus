#include "ui/DraftMenuUI.h"
#include "ui/UIHelpers.h"
#include "GameManager.h"
#include "backend/CardDatabase.h"
#include "backend/ActiveRun.h"
#include "backend/PlayerInfo.h"
#include "backend/CardZone.h"
#include "backend/Card.h"

void DraftListener::startDraft(Rml::ElementDocument* doc) {
    draftDoc = doc;
    picksLeft = 5;
    pool.clear();
    if (cardTemplateRML.empty()) cardTemplateRML = UIHelpers::LoadFileToString("assets/card.rml");
    for (int i = 0; i < 10; i++) {
        auto card = CardDatabase::getInstance().getRandomCard();
        if (card) pool.push_back(card);
    }
    refreshUI();
}

void DraftListener::refreshUI() {
    if (!draftDoc) return;
    
    // Thematic terminology update
    if (auto title = draftDoc->GetElementById("draft-title")) {
        title->SetInnerRML("SCAVENGE THE ARCHIVES // Fragment " + std::to_string(6 - picksLeft) + " of 5");
    }
    
    auto container = draftDoc->GetElementById("card-pool");
    if (!container) return;
    std::string html = "";
    for (size_t i = 0; i < pool.size(); i++) {
        std::string cardVisuals = UIHelpers::GenerateCardRML(cardTemplateRML, *pool[i]);
        // Changed class to 'draft-choice' so it gets the grimdark hover effect
        html += "<div class=\"draft-choice\" id=\"draft-btn-" + std::to_string(i) + "\">\n" + cardVisuals + "\n</div>";
    }
    container->SetInnerRML(html);

    for (size_t i = 0; i < pool.size(); i++) {
        auto btn = draftDoc->GetElementById("draft-btn-" + std::to_string(i));
        if (btn) btn->AddEventListener(Rml::EventId::Click, this);
    }
}

void DraftListener::ProcessEvent(Rml::Event& event) {
    std::string id = event.GetCurrentElement()->GetId();
    if (id.find("draft-btn-") == 0) {
        int index = std::stoi(id.substr(10));
        if (index >= 0 && index < static_cast<int>(pool.size())) {
            GameManager::instance().getActiveRun().getPlayer().getDeck().addCard(pool[index]);
            pool.erase(pool.begin() + index);
            picksLeft--;
            
            if (picksLeft > 0) refreshUI();
            else GameManager::instance().setState(GameState::COMBAT);
        }
    }
}