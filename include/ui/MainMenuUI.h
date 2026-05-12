#pragma once

#include <RmlUi/Core/EventListener.h>
#include <RmlUi/Core/Event.h>
#include <RmlUi/Core/Element.h>

class MainMenuListener : public Rml::EventListener {
public:
    void ProcessEvent(Rml::Event& event) override;
};