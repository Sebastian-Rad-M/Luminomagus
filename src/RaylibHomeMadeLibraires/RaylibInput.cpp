#include "RaylibInput.h"
#include <RmlUi/Core/Context.h>
#include <raylib.h>

void UpdateRmlInput(Rml::Context* context) {
    if (!context) return;

    Vector2 mousePos = GetMousePosition();
    context->ProcessMouseMove((int)mousePos.x, (int)mousePos.y, 0);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) context->ProcessMouseButtonDown(0, 0);
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) context->ProcessMouseButtonUp(0, 0);
    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) context->ProcessMouseButtonDown(1, 0);
    if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON)) context->ProcessMouseButtonUp(1, 0);
    
    float wheel = GetMouseWheelMove();
    if (wheel != 0.0f) {
        context->ProcessMouseWheel(-wheel, 0); 
    }
    
    int charPressed = GetCharPressed();
    while (charPressed > 0) {
        context->ProcessTextInput(charPressed);
        charPressed = GetCharPressed();
    }
}