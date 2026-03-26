#include "ui/ShopUI.h"
#include "ui/UIHelpers.h"
#include "GameManager.h"
#include "backend/ActiveRun.h"
#include "backend/Shop.h"
#include "backend/PlayerInfo.h"
#include "backend/RelicZone.h"
#include "backend/Relic.h"
void ShopListener::startShop(Rml::ElementDocument* doc) {
    shopDoc = doc;
    if (cardTemplateRML.empty()) cardTemplateRML = UIHelpers::LoadFileToString("assets/card.rml");
    if (relicTemplateRML.empty()) relicTemplateRML = UIHelpers::LoadFileToString("assets/relic.rml");
    refreshUI();
}

void ShopListener::refreshUI() {
    if (!shopDoc) return;
    auto& activeRun = GameManager::instance().getActiveRun();
    auto& shop = activeRun.getShop();
    auto& player = activeRun.getPlayer();
    
    if (auto el = shopDoc->GetElementById("player-gold")) {
        el->SetInnerRML("Gold: " + std::to_string(player.getGold()) + "G");
    }

    // --- Render Card Stock (Incantations) ---
    auto cardContainer = shopDoc->GetElementById("card-stock");
    if (cardContainer) {
        std::string cardHtml;
        const auto& cards = shop.getCards();
        for (size_t i = 0; i < cards.size(); i++) {
            if (cards[i].isSold) {
                cardHtml += "<div class=\"sold-out\">[ RECALLED ]</div>";
            } else {
                std::string cardVisuals = UIHelpers::GenerateCardRML(cardTemplateRML, *cards[i].item);
                cardHtml += "<div class=\"memory-fragment\" id=\"buy-card-" + std::to_string(i) + "\">\n"
                            + cardVisuals 
                            + "\n<div class=\"price-tag\">" + std::to_string(cards[i].price) + "G</div>\n"
                            + "</div>";
            }
        }
        cardContainer->SetInnerRML(cardHtml);
    }

    // --- Render Relic Stock (Using relic.rml!) ---
    auto relicContainer = shopDoc->GetElementById("relic-stock");
    if (relicContainer) {
        std::string relicHtml;
        const auto& relics = shop.getRelics();
        for (size_t i = 0; i < relics.size(); i++) {
            if (relics[i].isSold) {
                relicHtml += "<div class=\"sold-out\" style=\"width: 140px; height: 100px;\">[ RECALLED ]</div>";
            } else {
                std::string singleRelic = relicTemplateRML;
                singleRelic = UIHelpers::ReplaceAll(singleRelic, "[INDEX]", std::to_string(i));
                singleRelic = UIHelpers::ReplaceAll(singleRelic, "[NAME]", relics[i].item->getName());
                singleRelic = UIHelpers::ReplaceAll(singleRelic, "[DESC]", relics[i].item->getDescription());
                singleRelic = UIHelpers::ReplaceAll(singleRelic, "[ACTIVATABLE_CLASS]", "");
                
                // Hack the template: Swap 'sell' for 'buy' so your click event catches it
                singleRelic = UIHelpers::ReplaceAll(singleRelic, "sell-relic-", "buy-relic-");
                // Hack the template: Change "Shatter" to the price
                singleRelic = UIHelpers::ReplaceAll(singleRelic, "Shatter", "Recall (" + std::to_string(relics[i].price) + "G)");
                
                relicHtml += singleRelic;
            }
        }
        relicContainer->SetInnerRML(relicHtml);
    }

    // --- Render Player Relics for Forgetting (Using relic.rml!) ---
    auto sellContainer = shopDoc->GetElementById("player-relics-sell");
    if (sellContainer) {
        std::string sellHtml;
        const auto& pRelics = player.getRelicZone().getRelicZone();
        for (size_t i = 0; i < pRelics.size(); i++) {
            int price = Shop::calculatePrice(pRelics[i]->getRarity());
            
            std::string singleRelic = relicTemplateRML;
            singleRelic = UIHelpers::ReplaceAll(singleRelic, "[INDEX]", std::to_string(i));
            singleRelic = UIHelpers::ReplaceAll(singleRelic, "[NAME]", pRelics[i]->getName());
            singleRelic = UIHelpers::ReplaceAll(singleRelic, "[DESC]", pRelics[i]->getDescription());
            singleRelic = UIHelpers::ReplaceAll(singleRelic, "[ACTIVATABLE_CLASS]", "");
            
            // We keep the "sell-relic-" ID intact, but update the text!
            singleRelic = UIHelpers::ReplaceAll(singleRelic, "Shatter", "Forget (+" + std::to_string(price) + "G)");
            
            sellHtml += singleRelic;
        }
        sellContainer->SetInnerRML(sellHtml);
    }

    // Re-attach listeners
    attachItemListeners("buy-card-", shop.getCards().size());
    attachItemListeners("buy-relic-", shop.getRelics().size());
    attachItemListeners("sell-relic-", player.getRelicZone().getRelicZone().size());
}
void ShopListener::attachItemListeners(const std::string& prefix, size_t count) {
    for (size_t i = 0; i < count; i++) {
        auto btn = shopDoc->GetElementById(prefix + std::to_string(i));
        if (btn) btn->AddEventListener(Rml::EventId::Click, this);
    }
}

void ShopListener::ProcessEvent(Rml::Event& event) {
    std::string id = event.GetCurrentElement()->GetId();
    auto& activeRun = GameManager::instance().getActiveRun();
    auto& shop = activeRun.getShop();
    auto& player = activeRun.getPlayer();

    if (id == "btn-leave-shop") {
        GameManager::instance().setState(GameState::COMBAT);
    }
    else if (id.find("buy-card-") == 0) {
        shop.buyCard(std::stoi(id.substr(9)), player);
        refreshUI();
    }
    else if (id.find("buy-relic-") == 0) {
        shop.buyRelic(std::stoi(id.substr(10)), player);
        refreshUI();
    }
    else if (id.find("sell-relic-") == 0) {
        int idx = std::stoi(id.substr(11));
        const auto& pRelics = player.getRelicZone().getRelicZone();
        player.addGold(Shop::calculatePrice(pRelics[idx]->getRarity()));
        player.getRelicZone().removeRelic(idx);
        refreshUI();
    }
}