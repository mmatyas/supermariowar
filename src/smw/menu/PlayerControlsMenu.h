#pragma once

#include "uimenu.h"

class MI_InputControlContainer;

/*
    This is where the selected player (from PlayerControlsSelectMenu)
    can set the preferred control device and keys.
*/
class UI_PlayerControlsMenu : public UI_Menu {
public:
    UI_PlayerControlsMenu();

    void SetPlayer(short playerID);

private:
    MI_InputControlContainer* miInputContainer;
};
