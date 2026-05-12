#include "ui/UIHelpers.h"
#include <fstream>
#include <sstream>
#include <algorithm>

namespace UIHelpers {

    std::string LoadFileToString(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {return "";}
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
        size_t start_pos = 0;
        while((start_pos = str.find(from, start_pos)) != std::string::npos) {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length();
        }
        return str;
    }

    std::string GenerateCardRML(const std::string& templateRML, const Card& card) {
        std::string result = templateRML;
        result = ReplaceAll(result, "[NAME]", card.getName());
        std::string cardText = card.getText();
            if (cardText.empty()) {
                cardText = "No description available for this card.";
            }
            result = ReplaceAll(result, "[TEXT]", cardText);
        std::string cardClass = "card-generic";
        if (card.getRedCost() > 0) cardClass = "card-red";
        else if (card.getBlueCost() > 0) cardClass = "card-blue";
        else if (card.getGreenCost() > 0) cardClass = "card-green";
        
        result = ReplaceAll(result, "[CARD_CLASS]", cardClass);
        std::string manaHtml = "";
        
       int generic = card.getGenericCost();
        if (generic > 0) {
            int displayGen = std::min(generic, 9); 
            manaHtml += "<div class=\"mana-icon icon-" + std::to_string(displayGen) + "\"></div>";        
        }

        for (int i = 0; i < card.getRedCost(); i++) manaHtml += "<div class=\"mana-icon icon-red\"></div>";
        for (int i = 0; i < card.getBlueCost(); i++)  manaHtml += "<div class=\"mana-icon icon-blue\"></div>";
        for (int i = 0; i < card.getGreenCost(); i++) manaHtml += "<div class=\"mana-icon icon-green\"></div>";
        if (manaHtml.empty())  manaHtml = "<div class=\"mana-icon icon-0\"></div>";
        result = ReplaceAll(result, "[MANA_COSTS]", manaHtml);

        return result;
    }

}