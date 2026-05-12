#pragma once
#include <RmlUi/Core/SystemInterface.h>

class RaylibSystemInterface : public Rml::SystemInterface {
public:
    double GetElapsedTime() override;
};