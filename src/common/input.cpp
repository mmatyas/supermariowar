#include "input.h"

#include "GameValues.h"
#include "GlobalConstants.h"

#include <array>

extern CGameValues game_values;

#ifdef __ANDROID__
namespace {
std::array<SDL_JoystickID, MAX_PLAYERS> selectedGamepads {};
std::array<CInputPlayerControl*, MAX_PLAYERS> selectedConfigs {};
std::array<short, MAX_PLAYERS> selectedIndexes {};

void bindGamepads(CPlayerInput& input)
{
    int count = 0;
    SDL_JoystickID* ids = SDL_GetJoysticks(&count);
    for (int player = 0; player < MAX_PLAYERS; ++player) {
        CInputPlayerControl* config = input.inputControls[player];
        const short index = config ? config->iDevice : DEVICE_KEYBOARD;
        if (config != selectedConfigs[player] || index != selectedIndexes[player]) {
            selectedGamepads[player] = 0;
            selectedConfigs[player] = config;
            selectedIndexes[player] = index;
            for (auto& key : input.outputControls[player].keys)
                key = {};
        }
        if (selectedGamepads[player] || index < 0 || index >= count || !ids)
            continue;
        SDL_JoystickID candidate = ids[index];
        bool taken = false;
        for (int other = 0; other < MAX_PLAYERS; ++other)
            taken |= other != player && selectedGamepads[other] == candidate;
        if (taken) {
            candidate = 0;
            for (int otherIndex = 0; otherIndex < count; ++otherIndex) {
                bool used = false;
                for (SDL_JoystickID id : selectedGamepads)
                    used |= id == ids[otherIndex];
                if (!used && SDL_IsGamepad(ids[otherIndex])) {
                    candidate = ids[otherIndex];
                    break;
                }
            }
        }
        if (candidate && SDL_IsGamepad(candidate))
            selectedGamepads[player] = candidate;
    }
    SDL_free(ids);
}

bool gamepadControlDown(SDL_Gamepad* pad, SDL_Keycode binding, int mode, int key)
{
    const auto button = [&](SDL_GamepadButton code) { return SDL_GetGamepadButton(pad, code); };
    const auto axis = [&](SDL_GamepadAxis code) { return SDL_GetGamepadAxis(pad, code); };
    switch (binding) {
        case JOY_STICK_1_LEFT:  return axis(SDL_GAMEPAD_AXIS_LEFTX) < -JOYSTICK_DEAD_ZONE || button(SDL_GAMEPAD_BUTTON_DPAD_LEFT);
        case JOY_STICK_1_RIGHT: return axis(SDL_GAMEPAD_AXIS_LEFTX) > JOYSTICK_DEAD_ZONE || button(SDL_GAMEPAD_BUTTON_DPAD_RIGHT);
        case JOY_STICK_1_UP:    return axis(SDL_GAMEPAD_AXIS_LEFTY) < -JOYSTICK_DEAD_ZONE || button(SDL_GAMEPAD_BUTTON_DPAD_UP);
        case JOY_STICK_1_DOWN:  return axis(SDL_GAMEPAD_AXIS_LEFTY) > JOYSTICK_DEAD_ZONE || button(SDL_GAMEPAD_BUTTON_DPAD_DOWN);
        case JOY_STICK_2_LEFT:  return axis(SDL_GAMEPAD_AXIS_RIGHTX) < -JOYSTICK_DEAD_ZONE;
        case JOY_STICK_2_RIGHT: return axis(SDL_GAMEPAD_AXIS_RIGHTX) > JOYSTICK_DEAD_ZONE;
        case JOY_STICK_2_UP:    return axis(SDL_GAMEPAD_AXIS_RIGHTY) < -JOYSTICK_DEAD_ZONE;
        case JOY_STICK_2_DOWN:  return axis(SDL_GAMEPAD_AXIS_RIGHTY) > JOYSTICK_DEAD_ZONE;
        default: break;
    }
    if (binding < GAMEPAD_BUTTON_START || binding >= GAMEPAD_BUTTON_START + SDL_GAMEPAD_BUTTON_COUNT)
        return false;
    const auto code = static_cast<SDL_GamepadButton>(binding - GAMEPAD_BUTTON_START);
    bool down = button(code);
    if (mode == 0 && key == 2 && code == SDL_GAMEPAD_BUTTON_SOUTH)
        down |= button(SDL_GAMEPAD_BUTTON_EAST);
    if (mode == 0 && key == 5 && code == SDL_GAMEPAD_BUTTON_WEST)
        down |= button(SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER);
    if (mode == 1 && key == 4 && code == SDL_GAMEPAD_BUTTON_SOUTH)
        down |= button(SDL_GAMEPAD_BUTTON_START);
    if (mode == 1 && key == 5 && code == SDL_GAMEPAD_BUTTON_EAST)
        down |= button(SDL_GAMEPAD_BUTTON_BACK);
    return down;
}

void clearGamepadControls(CPlayerInput& input, SDL_JoystickID removed = 0)
{
    for (int player = 0; player < MAX_PLAYERS; ++player) {
        if (!selectedGamepads[player] || (removed && selectedGamepads[player] != removed))
            continue;
        for (auto& key : input.outputControls[player].keys)
            key = {};
        if (removed)
            selectedGamepads[player] = 0;
    }
}

bool updateGamepad(CPlayerInput& input, const SDL_Event& event, int mode)
{
    if (event.type == SDL_EVENT_WILL_ENTER_BACKGROUND || event.type == SDL_EVENT_DID_ENTER_FOREGROUND) {
        clearGamepadControls(input);
        return false;
    }
    if (event.type == SDL_EVENT_GAMEPAD_ADDED) {
        if (!SDL_GetGamepadFromID(event.gdevice.which))
            SDL_OpenGamepad(event.gdevice.which);
        bindGamepads(input);
        return true;
    }
    if (event.type == SDL_EVENT_GAMEPAD_REMOVED) {
        clearGamepadControls(input, event.gdevice.which);
        if (SDL_Gamepad* pad = SDL_GetGamepadFromID(event.gdevice.which))
            SDL_CloseGamepad(pad);
        return true;
    }
    if (event.type != SDL_EVENT_GAMEPAD_BUTTON_DOWN && event.type != SDL_EVENT_GAMEPAD_BUTTON_UP
            && event.type != SDL_EVENT_GAMEPAD_AXIS_MOTION)
        return false;

    const SDL_JoystickID instance = event.type == SDL_EVENT_GAMEPAD_AXIS_MOTION ? event.gaxis.which : event.gbutton.which;
    SDL_Gamepad* pad = SDL_GetGamepadFromID(instance);
    if (!pad) return true;
    bindGamepads(input);
    for (int player = 0; player < MAX_PLAYERS; ++player) {
        CInputPlayerControl* config = input.inputControls[player];
        if (!config || selectedGamepads[player] != instance)
            continue;
        for (int key = 0; key < NUM_KEYS; ++key) {
            if (mode == 0 && game_values.playercontrol[player] != 1 && key < 6)
                continue;
            CKeyState& output = input.outputControls[player].keys[key];
            const bool down = gamepadControlDown(pad, config->inputGameControls[mode].keys[key], mode, key);
            if (down && !output.fDown) output.fPressed = true;
            output.fDown = down;
        }
    }
    return true;
}
}

void ResetAndroidGamepadAssignments()
{
    selectedGamepads = {};
    selectedConfigs = {};
    selectedIndexes = {};
}
#endif

CPlayerInput::CPlayerInput()
{
    for (int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++) {
        for (int iKey = 0; iKey < NUM_KEYS; iKey++) {
			outputControls[iPlayer].keys[iKey].fPressed = false;
			outputControls[iPlayer].keys[iKey].fDown = false;
		}
	}

    iPressedKey = 0;
}

//Pass in 0 for game and 1 for menu
//Clear old button pushed states
void CPlayerInput::ClearPressedKeys(short iGameState)
{
    for (int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++) {
        COutputControl* outputControl = &outputControls[iPlayer];
        for (int iKey = 0; iKey < NUM_KEYS; iKey++) {
            outputControl->keys[iKey].fPressed = false;
        }
    }

    iPressedKey = 0;
}

void CPlayerInput::ClearGameActionKeys()
{
    for (int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++) {
		COutputControl * outputControl = &outputControls[iPlayer];

		// 0-3: direction keys, 4: turbo
        for (int iKey = 5; iKey < NUM_KEYS; iKey++) {
			outputControl->keys[iKey].fPressed = false;
			outputControl->keys[iKey].fDown = false;
		}
	}

	iPressedKey = 0;
}

//Clear all button pushed and down states
//Call this when switching from menu to game
void CPlayerInput::ResetKeys()
{
    for (int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++) {
        for (int iKey = 0; iKey < NUM_KEYS; iKey++) {
			outputControls[iPlayer].keys[iKey].fPressed = false;
			outputControls[iPlayer].keys[iKey].fDown = false;
		}
	}

	iPressedKey = 0;
}

//Called during game loop to read input events and see if
//configured keys were pressed.  If they were, then turn on
//key flags to be used by game logic
//iGameState == 0 for in game and 1 for menu
void CPlayerInput::Update(SDL_Event event, short iGameState)
{
#ifdef __ANDROID__
    if (updateGamepad(*this, event, iGameState)) return;
    if ((event.type == SDL_EVENT_JOYSTICK_HAT_MOTION || event.type == SDL_EVENT_JOYSTICK_BUTTON_DOWN
            || event.type == SDL_EVENT_JOYSTICK_BUTTON_UP || event.type == SDL_EVENT_JOYSTICK_AXIS_MOTION)
            && SDL_IsGamepad(event.jbutton.which)) return;
#endif
	bool fFound = false;
    for (short iPlayer = -1; iPlayer < MAX_PLAYERS; iPlayer++) {
		CInputControl * inputControl;
		COutputControl * outputControl;
		short iDeviceID = DEVICE_KEYBOARD;

		//Allow keyboard input from player 1 at all times (even when he is configured to use joystick)
        if (iPlayer == -1) {
            if (iGameState == 1 && inputControls[0]->iDevice != DEVICE_KEYBOARD) {
				inputControl = &game_values.inputConfiguration[0][0].inputGameControls[1];
				outputControl = &outputControls[0];
				iDeviceID = game_values.inputConfiguration[0][0].iDevice;
            } else {
				continue;
			}
        } else {
			if (!inputControls[iPlayer])
				continue;

			inputControl = &inputControls[iPlayer]->inputGameControls[iGameState];
			outputControl = &outputControls[iPlayer];
			iDeviceID = inputControls[iPlayer]->iDevice;
		}

        if (iDeviceID == DEVICE_KEYBOARD) {
            if (SDL_EVENT_KEY_DOWN == event.type) {
                for (int iKey = 0; iKey < NUM_KEYS && !fFound; iKey++) {
                    if (inputControl->keys[iKey] == event.key.key) {
						fFound = true;

						//Ignore input for cpu controlled players
						if (iGameState == 0 && game_values.playercontrol[iPlayer] != 1 && iKey < 6)
							continue;

						if (!outputControl->keys[iKey].fDown)
							outputControl->keys[iKey].fPressed = true;

						outputControl->keys[iKey].fDown = true;
					}
				}

				iPressedKey = event.key.key;
            } else if (SDL_EVENT_KEY_UP == event.type) {
                for (int iKey = 0; iKey < NUM_KEYS && !fFound; iKey++) {
                    if (inputControl->keys[iKey] == event.key.key) {
						fFound = true;

						//Ignore input for cpu controlled players
						if (iGameState == 0 && game_values.playercontrol[iPlayer] != 1 && iKey < 6)
							continue;

						outputControl->keys[iKey].fDown = false;
					}
				}
            } else if (SDL_EVENT_MOUSE_MOTION == event.type) {
                for (int iKey = 0; iKey < NUM_KEYS && !fFound; iKey++) {
                    if (inputControl->keys[iKey] >= MOUSE_UP) {
						if ((inputControl->keys[iKey] == MOUSE_UP && event.motion.yrel < -MOUSE_Y_DEAD_ZONE) ||
							(inputControl->keys[iKey] == MOUSE_DOWN && event.motion.yrel > MOUSE_Y_DEAD_ZONE) ||
							(inputControl->keys[iKey] == MOUSE_LEFT && event.motion.xrel < -MOUSE_X_DEAD_ZONE) ||
							(inputControl->keys[iKey] == MOUSE_RIGHT && event.motion.xrel > MOUSE_X_DEAD_ZONE) ||
                                (inputControl->keys[iKey] >= MOUSE_BUTTON_START && (event.motion.state & SDL_BUTTON_MASK(inputControl->keys[iKey] - MOUSE_BUTTON_START)))) {
							fFound = true;

							//Ignore input for cpu controlled players
							if (iGameState == 0 && game_values.playercontrol[iPlayer] != 1 && iKey < 6)
								continue;

							if (!outputControl->keys[iKey].fDown)
								outputControl->keys[iKey].fPressed = true;

							outputControl->keys[iKey].fDown = true;
                        } else {
							//Ignore input for cpu controlled players
							if (iGameState == 0 && game_values.playercontrol[iPlayer] != 1 && iKey < 6)
								continue;

							//Mouse scroll wheel up/down events happen on same frame so ignore up event (and clear it in the ClearPressedKeys() method)
							if (inputControl->keys[iKey] == MOUSE_BUTTON_START + 4 || inputControl->keys[iKey] == MOUSE_BUTTON_START + 5)
								continue;

							outputControl->keys[iKey].fDown = false;
						}
					}
				}
            } else if (SDL_EVENT_MOUSE_BUTTON_DOWN == event.type) {
                for (int iKey = 0; iKey < NUM_KEYS && !fFound; iKey++) {
                    if (inputControl->keys[iKey] == event.button.button + MOUSE_BUTTON_START) {
						fFound = true;

						//Ignore input for cpu controlled players
						if (iGameState == 0 && game_values.playercontrol[iPlayer] != 1 && iKey < 6)
							continue;

						if (!outputControl->keys[iKey].fDown)
							outputControl->keys[iKey].fPressed = true;

						outputControl->keys[iKey].fDown = true;
					}
				}
            } else if (SDL_EVENT_MOUSE_BUTTON_UP == event.type) {
                for (int iKey = 0; iKey < NUM_KEYS && !fFound; iKey++) {
                    if (inputControl->keys[iKey] == event.button.button + MOUSE_BUTTON_START) {
						fFound = true;

						//Mouse scroll wheel up/down events happen on same frame so ignore up event (and clear it in the ClearPressedKeys() method)
						if (inputControl->keys[iKey] == MOUSE_BUTTON_START + 4 || inputControl->keys[iKey] == MOUSE_BUTTON_START + 5)
							continue;

						//Ignore input for cpu controlled players
						if (iGameState == 0 && game_values.playercontrol[iPlayer] != 1 && iKey < 6)
							continue;

						outputControl->keys[iKey].fDown = false;
					}
				}
			}
        } else {
            int count = 0;
            SDL_JoystickID* ids = SDL_GetJoysticks(&count);
            const SDL_JoystickID instance = ids && iDeviceID >= 0 && iDeviceID < count ? ids[iDeviceID] : 0;
            SDL_free(ids);
            if (SDL_EVENT_JOYSTICK_HAT_MOTION == event.type) {
				if (instance != event.jhat.which)
					continue;

                for (int iKey = 0; iKey < NUM_KEYS; iKey++) {
                    if (inputControl->keys[iKey] >= JOY_HAT_UP && inputControl->keys[iKey] <= JOY_HAT_RIGHT) {
						if ((inputControl->keys[iKey] == JOY_HAT_UP && (event.jhat.value & SDL_HAT_UP)) ||
							(inputControl->keys[iKey] == JOY_HAT_DOWN && (event.jhat.value & SDL_HAT_DOWN)) ||
							(inputControl->keys[iKey] == JOY_HAT_LEFT && (event.jhat.value & SDL_HAT_LEFT)) ||
                                (inputControl->keys[iKey] == JOY_HAT_RIGHT && (event.jhat.value & SDL_HAT_RIGHT))) {
							fFound = true;

							//Ignore input for cpu controlled players
							if (iGameState == 0 && game_values.playercontrol[iPlayer] != 1 && iKey < 6)
								continue;

							if (!outputControl->keys[iKey].fDown)
								outputControl->keys[iKey].fPressed = true;

							outputControl->keys[iKey].fDown = true;
                        } else {
							//Ignore input for cpu controlled players
							if (iGameState == 0 && game_values.playercontrol[iPlayer] != 1 && iKey < 6)
								continue;

							outputControl->keys[iKey].fDown = false;
						}
					}
				}
            } else if (SDL_EVENT_JOYSTICK_BUTTON_DOWN == event.type) {
                if (instance != event.jbutton.which)
					continue;

                for (int iKey = 0; iKey < NUM_KEYS && !fFound; iKey++) {
                    if (inputControl->keys[iKey] == event.jbutton.button + JOY_BUTTON_START) {
						fFound = true;

						//Ignore input for cpu controlled players
						if (iGameState == 0 && game_values.playercontrol[iPlayer] != 1 && iKey < 6)
							continue;

						if (!outputControl->keys[iKey].fDown)
							outputControl->keys[iKey].fPressed = true;

						outputControl->keys[iKey].fDown = true;
					}
				}
            } else if (SDL_EVENT_JOYSTICK_BUTTON_UP == event.type) {
                if (instance != event.jbutton.which)
					continue;

                for (int iKey = 0; iKey < NUM_KEYS && !fFound; iKey++) {
                    if (inputControl->keys[iKey] == event.jbutton.button + JOY_BUTTON_START) {
						fFound = true;

						//Ignore input for cpu controlled players
						if (iGameState == 0 && game_values.playercontrol[iPlayer] != 1 && iKey < 6)
							continue;

						outputControl->keys[iKey].fDown = false;
					}
				}
            } else if (SDL_EVENT_JOYSTICK_AXIS_MOTION == event.type) {
                if (instance != event.jaxis.which)
					continue;

                for (int iKey = 0; iKey < NUM_KEYS; iKey++) {
					bool fUseJoystickInput = false;
					bool fJoystickDown = false;

                    if (event.jaxis.axis == 0 && inputControl->keys[iKey] == JOY_STICK_1_LEFT) {
						fUseJoystickInput = true;

						if (event.jaxis.value < -JOYSTICK_DEAD_ZONE)
							fJoystickDown = true;
                    } else if (event.jaxis.axis == 0 && inputControl->keys[iKey] == JOY_STICK_1_RIGHT) {
						fUseJoystickInput = true;

						if (event.jaxis.value > JOYSTICK_DEAD_ZONE)
							fJoystickDown = true;
                    } else if (event.jaxis.axis == 1 && inputControl->keys[iKey] == JOY_STICK_1_UP) {
						fUseJoystickInput = true;

						if (event.jaxis.value < -JOYSTICK_DEAD_ZONE)
							fJoystickDown = true;
                    } else if (event.jaxis.axis == 1 && inputControl->keys[iKey] == JOY_STICK_1_DOWN) {
						fUseJoystickInput = true;

						if (event.jaxis.value > JOYSTICK_DEAD_ZONE)
							fJoystickDown = true;
                    } else if (event.jaxis.axis == 2 && inputControl->keys[iKey] == JOY_STICK_2_LEFT) {
						fUseJoystickInput = true;

						if (event.jaxis.value < -JOYSTICK_DEAD_ZONE)
							fJoystickDown = true;
                    } else if (event.jaxis.axis == 2 && inputControl->keys[iKey] == JOY_STICK_2_RIGHT) {
						fUseJoystickInput = true;

						if (event.jaxis.value > JOYSTICK_DEAD_ZONE)
							fJoystickDown = true;
                    } else if (event.jaxis.axis == 3 && inputControl->keys[iKey] == JOY_STICK_2_UP) {
						fUseJoystickInput = true;

						if (event.jaxis.value < -JOYSTICK_DEAD_ZONE)
							fJoystickDown = true;
                    } else if (event.jaxis.axis == 3 && inputControl->keys[iKey] == JOY_STICK_2_DOWN) {
						fUseJoystickInput = true;

						if (event.jaxis.value > JOYSTICK_DEAD_ZONE)
							fJoystickDown = true;
					}

                    if (fUseJoystickInput) {
						//Ignore input for cpu controlled players
						if (iGameState == 0 && game_values.playercontrol[iPlayer] != 1 && iKey < 6)
							continue;

                        if (fJoystickDown) {
							fFound = true;

							if (!outputControl->keys[iKey].fDown)
								outputControl->keys[iKey].fPressed = true;

							outputControl->keys[iKey].fDown = true;
                        } else {
							outputControl->keys[iKey].fDown = false;
						}
					}
				}
			}
		}

		//This line might be causing input from some players not to be read
		//if (fFound)
			//break;
	}
}
