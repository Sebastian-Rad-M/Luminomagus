#pragma once

#include <RmlUi/Core/EventListener.h>
#include <RmlUi/Core/ElementDocument.h>
#include <RmlUi/Core/Event.h>
#include <RmlUi/Core/Element.h>
#include <vector>
#include <memory>
#include <string>

class Card; 

class DraftListener : public Rml::EventListener {
private:
    std::vector<std::shared_ptr<Card>> pool;
    int picksLeft = 5;
    Rml::ElementDocument* draftDoc = nullptr;
    std::string cardTemplateRML;

public:
    void startDraft(Rml::ElementDocument* doc);
    void refreshUI();
    void ProcessEvent(Rml::Event& event) override;
};