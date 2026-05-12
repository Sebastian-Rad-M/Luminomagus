#pragma once
#include <string>
#include "backend/Card.h" 

namespace UIHelpers {
    std::string LoadFileToString(const std::string& filepath);
    std::string ReplaceAll(std::string str, const std::string& from, const std::string& to);
    std::string GenerateCardRML(const std::string& templateRML, const Card& card);
}