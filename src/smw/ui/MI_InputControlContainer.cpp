#include "MI_InputControlContainer.h"

#include "GameValues.h"
#include "gfx/gfxSprite.h"
#include "uimenu.h"
#include "ui/MI_Button.h"
#include "ui/MI_Image.h"
#include "ui/MI_SelectField.h"
#include "ui/MI_Text.h"
#include "ResourceManager.h"

#include <cstdlib>
#include <cstring>

extern CGameValues game_values;
extern CResourceManager* rm;

extern short joystickcount;


namespace {
const std::array<std::string, NUM_KEYS> GameInputNames {
    "Left",
    "Right",
    "Jump",
    "Down",
    "Turbo",
    "Use Item",
    "Pause",
    "Exit",
};
const std::array<std::string, NUM_KEYS> MenuInputNames {
    "Up",
    "Down",
    "Left",
    "Right",
    "Select",
    "Cancel",
    "Random",
    "Fast Map",
};
} // namespace


/**************************************
 * MI_InputControlField Class
 **************************************/

MI_InputControlField::MI_InputControlField(gfxSprite * nspr, short x, short y, std::string name, short width, short indent) :
    UI_Control(x, y),
    szName(std::move(name))
{
    spr = nspr;

    iWidth = width;
    iIndent = indent;
    fSelected = false;

    iDevice = DEVICE_KEYBOARD;
    iKey = NULL;
    iType = 0;
    iKeyIndex = 0;
    iPlayerIndex = 0;
}

MI_InputControlField::~MI_InputControlField()
{}

    const char * MI_InputControlField::Joynames[30] = {
        "Joystick Up", "Joystick Down", "Joystick Left", "Joystick Right", "Stick 2 Up", "Stick 2 Down", "Stick 2 Left", "Stick 2 Right", "Pad Up", "Pad Down",
        "Pad Left", "Pad Right", "Button 1", "Button 2", "Button 3", "Button 4", "Button 5", "Button 6", "Button 7", "Button 8",
        "Button 9", "Button 10", "Button 11", "Button 12", "Button 13", "Button 14", "Button 15", "Button 16", "Button 17", "Button 18"
    };

MenuCodeEnum MI_InputControlField::Modify(bool modify)
{
    fModifying = modify;
    return MENU_CODE_MODIFY_ACCEPTED;
}

MenuCodeEnum MI_InputControlField::SendInput(CPlayerInput *)
{
    SDL_Event event;
    bool done = false;

    while (!done) {
        #ifndef __EMSCRIPTEN__
        SDL_WaitEvent(&event);
        #endif

        /*
        if (event.type == SDL_KEYDOWN)
        {
            printf("*********** Key Down ***********\n");
            printf("Keysym: %d\n", event.key.keysym.sym);
            printf("State: %d\n", event.key.state);
            printf("Type: %d\n", event.key.type);
            printf("Which: %d\n\n", event.key.which);
        }
        else if (event.type == SDL_MOUSEMOTION)
        {
            printf("*********** Mouse Motion ***********\n");
            printf("X: %d\n", event.motion.x);
            printf("Y: %d\n", event.motion.y);
            printf("State: %d\n", event.motion.state);
            printf("Type: %d\n", event.motion.type);
            printf("Which: %d\n\n", event.motion.which);
        }
        else if (event.type == SDL_MOUSEBUTTONDOWN)
        {
            printf("*********** Mouse Button ***********\n");
            printf("Button: %d\n", event.button.button);
            printf("State: %d\n", event.button.state);
            printf("Type: %d\n", event.button.type);
            printf("Which: %d\n\n", event.button.which);
        }
        else if (event.type == SDL_JOYHATMOTION)
        {
            printf("*********** Joystick Hat ***********\n");
            printf("Value: %d\n", event.jhat.value);
            printf("Type: %d\n", event.jhat.type);
            printf("Which: %d\n\n", event.jhat.which);
        }
        else if (event.type == SDL_JOYAXISMOTION)
        {
            printf("*********** Joystick Motion ***********\n");
            printf("Value: %d\n", event.jaxis.value);
            printf("Axis: %d\n", event.jaxis.axis);
            printf("Type: %d\n", event.jaxis.type);
            printf("Which: %d\n\n", event.jaxis.which);
        }
        else if (event.type == SDL_JOYBUTTONDOWN)
        {
            printf("*********** Joystick Button ***********\n");
            printf("Button: %d\n", event.jbutton.button);
            printf("State: %d\n", event.jbutton.state);
            printf("Type: %d\n", event.jbutton.type);
            printf("Which: %d\n\n", event.jbutton.which);
        }
        */

        game_values.playerInput.Update(event, 1);

        //Check to see if user cancelled
        //This was causing trouble with the joystick.  If a user had the menu cancel button
        //configured to something that he wanted to use in game, it would cancel that action.
        //This happened a lot when configuring the in game exit button.
        /*
        for (int iPlayer = 0; iPlayer < 4; iPlayer++)
        {
            if (playerInput->outputControls[iPlayer].menu_cancel.fPressed)
            {
                playerInput->ResetKeys();

                fModifying = false;
                return MENU_CODE_UNSELECT_ITEM;
            }
        }
        */

        if (iDevice == DEVICE_KEYBOARD) {
            if (event.type == SDL_KEYDOWN) {
                const SDL_Keycode key = event.key.keysym.sym;

                SetKey(iKey, key, iDevice);
                done = true;
            } else if (event.type == SDL_MOUSEMOTION) {
                short xmag = (short)abs(event.motion.xrel);
                short ymag = (short)abs(event.motion.yrel);

                if (xmag < MOUSE_X_DEAD_ZONE && ymag < MOUSE_Y_DEAD_ZONE)
                    continue;

                SDL_Keycode key = KEY_NONE;
                if (xmag > ymag) {
                    if (event.motion.xrel < 0)
                        key = MOUSE_LEFT;
                    else
                        key = MOUSE_RIGHT;
                } else {
                    if (event.motion.yrel < 0)
                        key = MOUSE_UP;
                    else
                        key = MOUSE_DOWN;
                }

                if (key != KEY_NONE) {
                    SetKey(iKey, key, iDevice);
                    done = true;
                }
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                const SDL_Keycode key = event.button.button + MOUSE_BUTTON_START;
                SetKey(iKey, key, iDevice);
                done = true;
            }
        } else {
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    done = true;
                }
            } else if (event.type == SDL_JOYHATMOTION) {
                SDL_Keycode key = KEY_NONE;

                if (event.jhat.value & SDL_HAT_UP) {
                    key = JOY_HAT_UP;
                } else if (event.jhat.value & SDL_HAT_DOWN) {
                    key = JOY_HAT_DOWN;
                } else if (event.jhat.value & SDL_HAT_LEFT) {
                    key = JOY_HAT_LEFT;
                } else if (event.jhat.value & SDL_HAT_RIGHT) {
                    key = JOY_HAT_RIGHT;
                }

                if (key != KEY_NONE) {
                    SetKey(iKey, key, iDevice);
                    done = true;
                }
            } else if (event.type == SDL_JOYAXISMOTION) {
                SDL_Keycode key = KEY_NONE;

                if (event.jaxis.axis == 0) {
                    if (event.jaxis.value < -JOYSTICK_DEAD_ZONE) {
                        key = JOY_STICK_1_LEFT;
                    } else if (event.jaxis.value > JOYSTICK_DEAD_ZONE) {
                        key = JOY_STICK_1_RIGHT;
                    }
                } else if (event.jaxis.axis == 1) {
                    if (event.jaxis.value < -JOYSTICK_DEAD_ZONE) {
                        key = JOY_STICK_1_UP;
                    } else if (event.jaxis.value > JOYSTICK_DEAD_ZONE) {
                        key = JOY_STICK_1_DOWN;
                    }
                } else if (event.jaxis.axis == 2) {
                    if (event.jaxis.value < -JOYSTICK_DEAD_ZONE) {
                        key = JOY_STICK_2_LEFT;
                    } else if (event.jaxis.value > JOYSTICK_DEAD_ZONE) {
                        key = JOY_STICK_2_RIGHT;
                    }
                } else if (event.jaxis.axis == 3) {
                    if (event.jaxis.value < -JOYSTICK_DEAD_ZONE) {
                        key = JOY_STICK_2_UP;
                    } else if (event.jaxis.value > JOYSTICK_DEAD_ZONE) {
                        key = JOY_STICK_2_DOWN;
                    }
                }

                if (key != KEY_NONE) {
                    SetKey(iKey, key, iDevice);
                    done = true;
                }
            } else if (event.type == SDL_JOYBUTTONDOWN) {
                if (event.jbutton.state == SDL_PRESSED) {
                    const SDL_Keycode key = event.jbutton.button + JOY_BUTTON_START;
                    SetKey(iKey, key, iDevice);
                    done = true;
                }
            }
        }
    }

    //Need to clear down keys when coming into this (we'll ignore key up events in here)
    game_values.playerInput.ResetKeys();

    fModifying = false;
    return MENU_CODE_UNSELECT_ITEM;
}

void MI_InputControlField::SetKey(SDL_Keycode * iSetKey, SDL_Keycode key, short device)
{
    bool fNeedSwap = false;
    short iSwapPlayer, iSwapKey;

    for (int iPlayer = 0; iPlayer < 4; iPlayer++) {
        if (game_values.playerInput.inputControls[iPlayer]->iDevice != device)
            continue;

        for (int selKey = 0; selKey < NUM_KEYS; selKey++) {
            if (selKey == iKeyIndex && iPlayer == iPlayerIndex)
                continue;

            if (game_values.playerInput.inputControls[iPlayer]->inputGameControls[iType].keys[selKey] == key) {
                fNeedSwap = true;
                iSwapPlayer = iPlayer;
                iSwapKey = selKey;
                break;
            }
        }

        if (fNeedSwap)
            break;
    }

    if (fNeedSwap) {
        game_values.playerInput.inputControls[iSwapPlayer]->inputGameControls[iType].keys[iSwapKey] = *iSetKey;
    }

    *iSetKey = key;
}

void MI_InputControlField::Draw()
{
    if (!m_visible)
        return;

    spr->draw(m_pos.x, m_pos.y, {0, (fSelected ? 32 : 0), iIndent - 16, 32});
    spr->draw(m_pos.x + iIndent - 16, m_pos.y, {0, (fSelected ? 96 : 64), 32, 32});
    spr->draw(m_pos.x + iIndent + 16, m_pos.y, {528 - iWidth + iIndent, (fSelected ? 32 : 0), iWidth - iIndent - 16, 32});

    rm->menu_font_large.drawChopRight(m_pos.x + 16, m_pos.y + 5, iIndent - 8, szName.c_str());

    if (iKey == NULL)
        rm->menu_font_large.drawChopRight(m_pos.x + iIndent + 8, m_pos.y + 5, iWidth - iIndent - 16, "Unassigned");
    else if (fModifying)
        rm->menu_font_large.drawChopRight(m_pos.x + iIndent + 8, m_pos.y + 5, iWidth - iIndent - 16, "(Press Button)");
    else if (iDevice == DEVICE_KEYBOARD)
        rm->menu_font_large.drawChopRight(m_pos.x + iIndent + 8, m_pos.y + 5, iWidth - iIndent - 16, SDL_GetKeyName(*iKey));
    else
        rm->menu_font_large.drawChopRight(m_pos.x + iIndent + 8, m_pos.y + 5, iWidth - iIndent - 16, Joynames[*iKey]);
}


/**************************************
 * MI_InputControlContainer Class
 **************************************/

//call with x = 94, y = 19
MI_InputControlContainer::MI_InputControlContainer(gfxSprite * spr_button, short x, short y, short playerID) :
    UI_Control(x, y)
{
    iPlayerID = playerID;
    iDevice = game_values.playerInput.inputControls[iPlayerID]->iDevice;
    iSelectedInputType = 0;

    char szTitle[128];
    sprintf(szTitle, "Player %d Controls", iPlayerID + 1);
    miText = new MI_HeaderText(szTitle, 320, 5);

    miImage[0] = new MI_Image(spr_button, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miImage[1] = new MI_Image(spr_button, 320, 0, 192, 0, 320, 32, 1, 1, 0);

    miDeviceSelectField = new MI_SelectField<short>(spr_button, x + 16, y + 38, "Device", 420, 150);
    miDeviceSelectField->setItemChangedCode(MENU_CODE_INPUT_DEVICE_CHANGED);
    miDeviceSelectField->add("Keyboard", -1);

    for (short iJoystick = 0; iJoystick < joystickcount; iJoystick++) {
        miDeviceSelectField->add(SDL_JoystickNameForIndex(iJoystick), iJoystick, false);
    }

    //If the device is not found, default to the keyboard
    if (!miDeviceSelectField->setCurrentValue(iDevice)) {
        iDevice = DEVICE_KEYBOARD;
        miDeviceSelectField->setCurrentValue(iDevice);
    }

    miInputTypeButton = new MI_Button(spr_button, x + 336, y + 84, "Game", 100, TextAlign::CENTER);
    miInputTypeButton->SetCode(MENU_CODE_INPUT_TYPE_CHANGED);


    for (short iKey = 0; iKey < NUM_KEYS; iKey++) {
        miGameInputControlFields[iKey] = new MI_InputControlField(spr_button, x + 16, y + 118 + iKey * 34, GameInputNames[iKey], 420, 150);
        miGameInputControlFields[iKey]->SetDevice(iDevice);
        miGameInputControlFields[iKey]->SetType(0);
        miGameInputControlFields[iKey]->SetKeyIndex(iKey);
        miGameInputControlFields[iKey]->SetPlayerIndex(iPlayerID);
        miGameInputControlFields[iKey]->SetKey(&game_values.playerInput.inputControls[iPlayerID]->inputGameControls[0].keys[iKey]);
    }

    for (short iKey = 0; iKey < NUM_KEYS; iKey++) {
        miMenuInputControlFields[iKey] = new MI_InputControlField(spr_button, x + 16, y + 118 + iKey * 34, MenuInputNames[iKey], 420, 150);
        miMenuInputControlFields[iKey]->SetDevice(iDevice);
        miMenuInputControlFields[iKey]->SetType(1);
        miMenuInputControlFields[iKey]->SetKeyIndex(iKey);
        miMenuInputControlFields[iKey]->SetPlayerIndex(iPlayerID);
        miMenuInputControlFields[iKey]->SetKey(&game_values.playerInput.inputControls[iPlayerID]->inputGameControls[1].keys[iKey]);
    }

    miBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, TextAlign::CENTER);
    miBackButton->SetCode(MENU_CODE_BACK_TO_CONTROLS_MENU);

    mInputMenu = new UI_Menu();
    mInputMenu->SetCancelCode(MENU_CODE_BACK_TO_CONTROLS_MENU);
    mInputMenu->AddNonControl(miImage[0]);
    mInputMenu->AddNonControl(miImage[1]);
    mInputMenu->AddNonControl(miText);

    mInputMenu->AddControl(miDeviceSelectField, miBackButton, miInputTypeButton, NULL, miBackButton);
    mInputMenu->AddControl(miInputTypeButton, miDeviceSelectField, miGameInputControlFields[0], NULL, miBackButton);
    mInputMenu->AddControl(miGameInputControlFields[0], miInputTypeButton, miGameInputControlFields[1], NULL, miBackButton);
    mInputMenu->AddControl(miGameInputControlFields[1], miGameInputControlFields[0], miGameInputControlFields[2], NULL, miBackButton);
    mInputMenu->AddControl(miGameInputControlFields[2], miGameInputControlFields[1], miGameInputControlFields[3], NULL, miBackButton);
    mInputMenu->AddControl(miGameInputControlFields[3], miGameInputControlFields[2], miGameInputControlFields[4], NULL, miBackButton);
    mInputMenu->AddControl(miGameInputControlFields[4], miGameInputControlFields[3], miGameInputControlFields[5], NULL, miBackButton);
    mInputMenu->AddControl(miGameInputControlFields[5], miGameInputControlFields[4], miGameInputControlFields[6], NULL, miBackButton);
    mInputMenu->AddControl(miGameInputControlFields[6], miGameInputControlFields[5], miGameInputControlFields[7], NULL, miBackButton);
    mInputMenu->AddControl(miGameInputControlFields[7], miGameInputControlFields[6], miMenuInputControlFields[0], NULL, miBackButton);
    mInputMenu->AddControl(miMenuInputControlFields[0], miGameInputControlFields[7], miMenuInputControlFields[1], NULL, miBackButton);
    mInputMenu->AddControl(miMenuInputControlFields[1], miMenuInputControlFields[0], miMenuInputControlFields[2], NULL, miBackButton);
    mInputMenu->AddControl(miMenuInputControlFields[2], miMenuInputControlFields[1], miMenuInputControlFields[3], NULL, miBackButton);
    mInputMenu->AddControl(miMenuInputControlFields[3], miMenuInputControlFields[2], miMenuInputControlFields[4], NULL, miBackButton);
    mInputMenu->AddControl(miMenuInputControlFields[4], miMenuInputControlFields[3], miMenuInputControlFields[5], NULL, miBackButton);
    mInputMenu->AddControl(miMenuInputControlFields[5], miMenuInputControlFields[4], miMenuInputControlFields[6], NULL, miBackButton);
    mInputMenu->AddControl(miMenuInputControlFields[6], miMenuInputControlFields[5], miMenuInputControlFields[7], NULL, miBackButton);
    mInputMenu->AddControl(miMenuInputControlFields[7], miMenuInputControlFields[6], miBackButton, NULL, miBackButton);

    mInputMenu->AddControl(miBackButton, miMenuInputControlFields[7], miDeviceSelectField, miDeviceSelectField, NULL);

    mInputMenu->setInitialFocus(miDeviceSelectField);

    SetVisibleInputFields();
}

MI_InputControlContainer::~MI_InputControlContainer()
{
    //takes care of deleting all added controls
    delete mInputMenu;
}


void MI_InputControlContainer::Update()
{
    mInputMenu->Update();
}

void MI_InputControlContainer::Draw()
{
    if (!m_visible)
        return;

    mInputMenu->Draw();
}

MenuCodeEnum MI_InputControlContainer::SendInput(CPlayerInput * playerInput)
{
    MenuCodeEnum ret = mInputMenu->SendInput(playerInput);

    if (MENU_CODE_CANCEL_INPUT == ret) {
        fModifying = false;
        return MENU_CODE_UNSELECT_ITEM;
    } else if (MENU_CODE_INPUT_TYPE_CHANGED == ret) {
        if (0 == iSelectedInputType) {
            iSelectedInputType = 1;
            miInputTypeButton->SetName("Menu");
        } else {
            iSelectedInputType = 0;
            miInputTypeButton->SetName("Game");
        }

        SetVisibleInputFields();

        return MENU_CODE_NONE;
    } else if (MENU_CODE_INPUT_DEVICE_CHANGED == ret) {
        //TODO: Need to handle writing out input configurations, modifying input configs here
        //Need to handle case where reading in and using a device that is no longer
        //an option will crash the system (joystick unplugged or something)

        UpdateDeviceKeys(miDeviceSelectField->currentValue());
        playerInput->ResetKeys();

        return MENU_CODE_NONE;
    }

    return ret;
}

MenuCodeEnum MI_InputControlContainer::Modify(bool modify)
{
    fModifying = modify;
    return MENU_CODE_MODIFY_ACCEPTED;
}

void MI_InputControlContainer::SetVisibleInputFields()
{
    int selDevice = miDeviceSelectField->currentValue();

    for (int iKey = 0; iKey < NUM_KEYS; iKey++) {
        miGameInputControlFields[iKey]->setVisible(0 == iSelectedInputType && (iKey < 6 || DEVICE_KEYBOARD != selDevice || iPlayerID == 0));
    }

    for (int iKey = 0; iKey < NUM_KEYS; iKey++) {
        miMenuInputControlFields[iKey]->setVisible(1 == iSelectedInputType && (iKey < 6 || DEVICE_KEYBOARD != selDevice || iPlayerID == 0));
    }
}

void MI_InputControlContainer::SetPlayer(short playerID)
{
    //Hide input options that other players are using
    miDeviceSelectField->hideAllItems(false);

    for (short iPlayer = 0; iPlayer < 4; iPlayer++) {
        if (iPlayer == playerID)
            continue;

        if (game_values.playerInput.inputControls[iPlayer]->iDevice > -1)
            miDeviceSelectField->hideItem(game_values.playerInput.inputControls[iPlayer]->iDevice, true);
    }

    iPlayerID = playerID;
    iDevice = game_values.playerInput.inputControls[iPlayerID]->iDevice;
    miDeviceSelectField->setCurrentValue(iDevice);

    for (int iKey = 0; iKey < NUM_KEYS; iKey++) {
        miGameInputControlFields[iKey]->SetDevice(iDevice);
        miGameInputControlFields[iKey]->SetType(0);
        miGameInputControlFields[iKey]->SetKeyIndex(iKey);
        miGameInputControlFields[iKey]->SetPlayerIndex(iPlayerID);
        miGameInputControlFields[iKey]->SetKey(&game_values.playerInput.inputControls[iPlayerID]->inputGameControls[0].keys[iKey]);
    }

    for (int iKey = 0; iKey < NUM_KEYS; iKey++) {
        miMenuInputControlFields[iKey]->SetDevice(iDevice);
        miMenuInputControlFields[iKey]->SetType(1);
        miMenuInputControlFields[iKey]->SetKeyIndex(iKey);
        miMenuInputControlFields[iKey]->SetPlayerIndex(iPlayerID);
        miMenuInputControlFields[iKey]->SetKey(&game_values.playerInput.inputControls[iPlayerID]->inputGameControls[1].keys[iKey]);
    }

    SetVisibleInputFields();

    char szNewTitle[128];
    sprintf(szNewTitle, "Player %d Controls", iPlayerID + 1);
    miText->SetText(szNewTitle);
    mInputMenu->ResetMenu();
}

void MI_InputControlContainer::UpdateDeviceKeys(short lDevice)
{
    game_values.playerInput.inputControls[iPlayerID] = &game_values.inputConfiguration[iPlayerID][lDevice == DEVICE_KEYBOARD ? 0 : 1];
    game_values.playerInput.inputControls[iPlayerID]->iDevice = lDevice;

    for (int iKey = 0; iKey < NUM_KEYS; iKey++) {
        miGameInputControlFields[iKey]->SetDevice(lDevice);
        miGameInputControlFields[iKey]->SetKey(&game_values.playerInput.inputControls[iPlayerID]->inputGameControls[0].keys[iKey]);
    }

    for (int iKey = 0; iKey < NUM_KEYS; iKey++) {
        miMenuInputControlFields[iKey]->SetDevice(lDevice);
        miMenuInputControlFields[iKey]->SetKey(&game_values.playerInput.inputControls[iPlayerID]->inputGameControls[1].keys[iKey]);
    }

    SetVisibleInputFields();
}
