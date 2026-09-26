#include "input.h"

#include "GameValues.h"
#include "GlobalConstants.h"
#ifdef __ANDROID__
#include "AndroidControllerMapping.h"
#endif

extern CGameValues game_values;

#ifdef __ANDROID__
namespace {
SDL_GameController* androidController = nullptr;
SDL_JoystickID androidControllerId = -1;
AndroidControllerSnapshot androidInput;

void resetAndroidControl(CPlayerInput& input)
{
    androidInput.clear();
    for (auto& key : input.outputControls[0].keys) {
        key.fDown = false;
        key.fPressed = false;
    }
}

void openAndroidController(int index)
{
    if (androidController || !SDL_IsGameController(index)) return;
    androidController = SDL_GameControllerOpen(index);
    if (androidController) {
        androidControllerId = SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(androidController));
        SDL_Log("Android controller: %s, instance %d", SDL_GameControllerName(androidController), androidControllerId);
    }
}

void applyAndroidControl(CPlayerInput& input, short state)
{
    const auto desired = AndroidDesiredControls(androidInput, state, game_values.playercontrol[0] == 1);
    for (int i = 0; i < NUM_KEYS; ++i) {
        CKeyState& key = input.outputControls[0].keys[i];
        if (desired[i] && !key.fDown) key.fPressed = true;
        key.fDown = desired[i];
    }
}

bool handleAndroidController(CPlayerInput& input, const SDL_Event& event, short state)
{
    if (event.type == SDL_APP_WILLENTERBACKGROUND || event.type == SDL_APP_DIDENTERFOREGROUND) {
        resetAndroidControl(input);
        return false;
    }
    if (event.type == SDL_CONTROLLERDEVICEADDED) {
        openAndroidController(event.cdevice.which); // device index at add time
        return true;
    }
    if (event.type == SDL_CONTROLLERDEVICEREMOVED && event.cdevice.which == androidControllerId) {
        SDL_GameControllerClose(androidController);
        androidController = nullptr;
        androidControllerId = -1;
        resetAndroidControl(input);
        for (int i = 0; i < SDL_NumJoysticks() && !androidController; ++i) openAndroidController(i);
        return true;
    }
    if (!androidController) return false;
    if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
        // Keep keyboard and pad sources independent so releasing one cannot
        // cancel an action still held by the other.
        for (int mode = 0; mode < 2; ++mode) {
            for (int key = 0; key < NUM_KEYS; ++key) {
                if (game_values.inputConfiguration[0][0].inputGameControls[mode].keys[key] == event.key.keysym.sym)
                    androidInput.keyboard[mode][key] = event.type == SDL_KEYDOWN;
            }
        }
        if (event.type == SDL_KEYDOWN) input.iPressedKey = event.key.keysym.sym;
        applyAndroidControl(input, state);
        return true;
    }
    if ((event.type == SDL_CONTROLLERBUTTONDOWN || event.type == SDL_CONTROLLERBUTTONUP)
            && event.cbutton.which == androidControllerId
            && event.cbutton.button < SDL_CONTROLLER_BUTTON_MAX) {
        androidInput.buttons[event.cbutton.button] = event.type == SDL_CONTROLLERBUTTONDOWN;
        applyAndroidControl(input, state);
        return true;
    }
    if (event.type == SDL_CONTROLLERAXISMOTION && event.caxis.which == androidControllerId
            && event.caxis.axis < SDL_CONTROLLER_AXIS_MAX) {
        androidInput.axes[event.caxis.axis] = event.caxis.value;
        applyAndroidControl(input, state);
        return true;
    }
    return false;
}
} // namespace

void InitAndroidController()
{
    if (SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_INPUT, "Controller init failed: %s", SDL_GetError());
        return;
    }
    for (int i = 0; i < SDL_NumJoysticks() && !androidController; ++i) openAndroidController(i);
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
#ifdef __ANDROID__
    if (androidController) applyAndroidControl(*this, iGameState);
#endif
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
#ifdef __ANDROID__
    // A button held while leaving a screen must be released and pressed again.
    resetAndroidControl(*this);
#endif
}

//Called during game loop to read input events and see if
//configured keys were pressed.  If they were, then turn on
//key flags to be used by game logic
//iGameState == 0 for in game and 1 for menu
void CPlayerInput::Update(SDL_Event event, short iGameState)
{
#ifdef __ANDROID__
    if (handleAndroidController(*this, event, iGameState)) return;
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
            if (SDL_KEYDOWN == event.type) {
                for (int iKey = 0; iKey < NUM_KEYS && !fFound; iKey++) {
                    if (inputControl->keys[iKey] == event.key.keysym.sym) {
						fFound = true;

						//Ignore input for cpu controlled players
						if (iGameState == 0 && game_values.playercontrol[iPlayer] != 1 && iKey < 6)
							continue;

						if (!outputControl->keys[iKey].fDown)
							outputControl->keys[iKey].fPressed = true;

						outputControl->keys[iKey].fDown = true;
					}
				}

				iPressedKey = event.key.keysym.sym;
            } else if (SDL_KEYUP == event.type) {
                for (int iKey = 0; iKey < NUM_KEYS && !fFound; iKey++) {
                    if (inputControl->keys[iKey] == event.key.keysym.sym) {
						fFound = true;

						//Ignore input for cpu controlled players
						if (iGameState == 0 && game_values.playercontrol[iPlayer] != 1 && iKey < 6)
							continue;

						outputControl->keys[iKey].fDown = false;
					}
				}
            } else if (SDL_MOUSEMOTION == event.type) {
                for (int iKey = 0; iKey < NUM_KEYS && !fFound; iKey++) {
                    if (inputControl->keys[iKey] >= MOUSE_UP) {
						if ((inputControl->keys[iKey] == MOUSE_UP && event.motion.yrel < -MOUSE_Y_DEAD_ZONE) ||
							(inputControl->keys[iKey] == MOUSE_DOWN && event.motion.yrel > MOUSE_Y_DEAD_ZONE) ||
							(inputControl->keys[iKey] == MOUSE_LEFT && event.motion.xrel < -MOUSE_X_DEAD_ZONE) ||
							(inputControl->keys[iKey] == MOUSE_RIGHT && event.motion.xrel > MOUSE_X_DEAD_ZONE) ||
                                (inputControl->keys[iKey] >= MOUSE_BUTTON_START && (event.motion.state & SDL_BUTTON(inputControl->keys[iKey] - MOUSE_BUTTON_START)))) {
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
            } else if (SDL_MOUSEBUTTONDOWN == event.type) {
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
            } else if (SDL_MOUSEBUTTONUP == event.type) {
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
            if (SDL_JOYHATMOTION == event.type) {
				if (iDeviceID != event.jhat.which)
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
            } else if (SDL_JOYBUTTONDOWN == event.type) {
				if (iDeviceID != event.jbutton.which)
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
            } else if (SDL_JOYBUTTONUP == event.type) {
				if (iDeviceID != event.jbutton.which)
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
            } else if (SDL_JOYAXISMOTION == event.type) {
				if (iDeviceID != event.jaxis.which)
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
