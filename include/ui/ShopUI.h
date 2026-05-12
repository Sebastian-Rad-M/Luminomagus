#pragma once

#include <RmlUi/Core/EventListener.h>
#include <RmlUi/Core/ElementDocument.h>
#include <RmlUi/Core/Event.h>
#include <RmlUi/Core/Element.h>
#include <string>

class ShopListener : public Rml::EventListener {
private:
    Rml::ElementDocument* shopDoc = nullptr;
    
    // NO GLOBALS! Store the card template specifically for the shop instance.
    std::string cardTemplateRML;
    std::string relicTemplateRML;

public:
    void startShop(Rml::ElementDocument* doc);
    void refreshUI();
    void ProcessEvent(Rml::Event& event) override;

private:
    void attachItemListeners(const std::string& prefix, size_t count);
};