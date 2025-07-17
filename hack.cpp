#include "hack.hpp"
#include "imgui.h"
#include <iostream>

bool espEnabled = true;
bool aimbotEnabled = false;

void RenderOverlay() {
    ImGui::Begin("Cheat Menu");
    ImGui::Checkbox("ESP", &espEnabled);
    ImGui::Checkbox("Aimbot", &aimbotEnabled);
    ImGui::Text("Press END to unload.");
    ImGui::End();
}

void RunHack() {
    if (espEnabled) {
        // Draw boxes, lines etc.
    }
    if (aimbotEnabled) {
        // Get closest enemy, compute aim, set rotation
    }
}