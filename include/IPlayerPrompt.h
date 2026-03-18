#pragma once
#include <string>

class IPlayerPrompt {
public:
    virtual ~IPlayerPrompt() = default;
    virtual int requestChoice(const std::string& promptText, int min, int max) = 0;
    virtual char requestManaColor() = 0;
   virtual void showMessage(const std::string& message) = 0;
};