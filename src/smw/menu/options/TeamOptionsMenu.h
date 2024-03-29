#pragma once

#include "uimenu.h"

class MI_Button;
class MI_Image;
class MI_SelectField;
class MI_Text;

/*
    In this menu, you can set team-relates preferences,
    such as friendly collision or team color drawing.
*/
class UI_TeamOptionsMenu : public UI_Menu {
public:
    UI_TeamOptionsMenu();

private:
    MI_SelectField* miTeamKillsField;
    MI_SelectField* miTeamColorsField;
    MI_SelectField* miTournamentControlField;
    MI_Button* miTeamOptionsMenuBackButton;

    MI_Image* miTeamOptionsMenuLeftHeaderBar;
    MI_Image* miTeamOptionsMenuRightHeaderBar;
    MI_Text* miTeamOptionsMenuHeaderText;
};
