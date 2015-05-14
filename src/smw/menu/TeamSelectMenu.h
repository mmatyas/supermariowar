#ifndef MENU_TEAMSELECT_H
#define MENU_TEAMSELECT_H

#include "uimenu.h"
#include "uicontrol.h"
#include "uicustomcontrol.h"

/*
    Description.
*/

class UI_TeamSelectMenu : public UI_Menu
{
public:
    UI_TeamSelectMenu();
    ~UI_TeamSelectMenu();

    MI_TeamSelect * miTeamSelect;

private:

    MI_Image * miTeamSelectLeftHeaderBar;
    MI_Image * miTeamSelectRightHeaderBar;
    MI_Text * miTeamSelectHeaderText;
};

#endif // MENU_TEAMSELECT_H
