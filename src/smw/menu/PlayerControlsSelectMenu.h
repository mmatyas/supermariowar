#pragma once

#include "uimenu.h"

class MI_Button;
class MI_Image;
class MI_Text;

/*
    This menu leads to the control settings of player 1-4.
*/
class UI_PlayerControlsSelectMenu : public UI_Menu {
public:
    UI_PlayerControlsSelectMenu();

private:
    MI_Button* miPlayer1ControlsButton;
    MI_Button* miPlayer2ControlsButton;
    MI_Button* miPlayer3ControlsButton;
    MI_Button* miPlayer4ControlsButton;

    MI_Button* miPlayerControlsBackButton;

    MI_Image* miPlayerControlsLeftHeaderBar;
    MI_Image* miPlayerControlsMenuRightHeaderBar;
    MI_Text* miPlayerControlsMenuHeaderText;
};
