#pragma once

#include "input.h"

#include <array>

// Fixed first-pad profile for the Android build. SDL_GameController provides
// consistent buttons and axes across Android devices with different raw codes.
struct AndroidControllerSnapshot {
    std::array<bool, SDL_CONTROLLER_BUTTON_MAX> buttons {};
    std::array<Sint16, SDL_CONTROLLER_AXIS_MAX> axes {};
    std::array<std::array<bool, NUM_KEYS>, 2> keyboard {};

    void clear() { *this = {}; }
};

inline std::array<bool, NUM_KEYS> AndroidDesiredControls(const AndroidControllerSnapshot& source,
                                                         short mode, bool humanPlayer)
{
    const auto button = [&](SDL_GameControllerButton code) { return source.buttons[code]; };
    const bool left = button(SDL_CONTROLLER_BUTTON_DPAD_LEFT) || source.axes[SDL_CONTROLLER_AXIS_LEFTX] < -JOYSTICK_DEAD_ZONE;
    const bool right = button(SDL_CONTROLLER_BUTTON_DPAD_RIGHT) || source.axes[SDL_CONTROLLER_AXIS_LEFTX] > JOYSTICK_DEAD_ZONE;
    const bool up = button(SDL_CONTROLLER_BUTTON_DPAD_UP) || source.axes[SDL_CONTROLLER_AXIS_LEFTY] < -JOYSTICK_DEAD_ZONE;
    const bool down = button(SDL_CONTROLLER_BUTTON_DPAD_DOWN) || source.axes[SDL_CONTROLLER_AXIS_LEFTY] > JOYSTICK_DEAD_ZONE;
    std::array<bool, NUM_KEYS> desired {};
    if (mode == 1) {
        desired = {up, down, left, right,
                   button(SDL_CONTROLLER_BUTTON_A) || button(SDL_CONTROLLER_BUTTON_START),
                   button(SDL_CONTROLLER_BUTTON_B) || button(SDL_CONTROLLER_BUTTON_BACK),
                   button(SDL_CONTROLLER_BUTTON_X), button(SDL_CONTROLLER_BUTTON_RIGHTSHOULDER)};
    } else {
        desired = {left, right,
                   button(SDL_CONTROLLER_BUTTON_A) || button(SDL_CONTROLLER_BUTTON_B), down,
                   button(SDL_CONTROLLER_BUTTON_Y),
                   button(SDL_CONTROLLER_BUTTON_X) || button(SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) || button(SDL_CONTROLLER_BUTTON_LEFTSHOULDER),
                   button(SDL_CONTROLLER_BUTTON_START), button(SDL_CONTROLLER_BUTTON_BACK)};
    }
    for (int i = 0; i < NUM_KEYS; ++i) {
        desired[i] = desired[i] || source.keyboard[mode][i];
        if (mode == 0 && !humanPlayer && i < 6) desired[i] = false;
    }
    return desired;
}
