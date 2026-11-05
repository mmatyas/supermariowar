#pragma once

#include "GlobalConstants.h"
#include "uimenu.h"

#include <array>

class MI_Button;
class MI_Image;
class MI_Text;

/*
    This menu leads to the control settings of player 1-8.
*/
class UI_PlayerControlsSelectMenu : public UI_Menu {
public:
    UI_PlayerControlsSelectMenu();

private:
    std::array<MI_Button*, MAX_PLAYERS> miPlayerControlsButtons;

    MI_Button* miPlayerControlsBackButton;

    MI_Image* miPlayerControlsLeftHeaderBar;
    MI_Image* miPlayerControlsMenuRightHeaderBar;
    MI_Text* miPlayerControlsMenuHeaderText;
};
