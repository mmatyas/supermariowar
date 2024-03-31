#pragma once

#include "uimenu.h"

class MI_Button;
class MI_Image;
class MI_Text;
template<typename T> class MI_SelectFieldDyn;

/*
    In this menu, you can set team-relates preferences,
    such as friendly collision or team color drawing.
*/
class UI_TeamOptionsMenu : public UI_Menu {
public:
    UI_TeamOptionsMenu();

private:
    MI_SelectFieldDyn<short>* miTeamKillsField;
    MI_SelectFieldDyn<bool>* miTeamColorsField;
    MI_SelectFieldDyn<short>* miTournamentControlField;
    MI_Button* miTeamOptionsMenuBackButton;

    MI_Image* miTeamOptionsMenuLeftHeaderBar;
    MI_Image* miTeamOptionsMenuRightHeaderBar;
    MI_Text* miTeamOptionsMenuHeaderText;
};
