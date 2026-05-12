#pragma once

#include <RmlUi/Core/EventListener.h>
#include <RmlUi/Core/ElementDocument.h>
#include <RmlUi/Core/Event.h>

enum class PileView { NONE, GRAVEYARD, VOID };

class CombatListener : public Rml::EventListener {
private:
    Rml::ElementDocument* combatDoc = nullptr;
    std::string cardTemplateRML;
    bool menuOpen = false;
    int pendingXValue = 0;
    PileView activePileView = PileView::NONE;
    std::string relicTemplateRML;

public:
    void startCombat(Rml::ElementDocument* doc);
    void refreshUI();
    void ProcessEvent(Rml::Event& event) override;

private:
    void checkWinCondition();
    void checkLossCondition();
    void toggleMenu();

};