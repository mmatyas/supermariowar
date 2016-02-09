#ifndef MENU_TEAMSELECT_H
#define MENU_TEAMSELECT_H

#include "uimenu.h"
#include "uicontrol.h"
#include "ui/MI_TeamSelect.h"

/*
    Description.
*/

class UI_TeamSelectMenu : public UI_Menu
{
public:
    UI_TeamSelectMenu();
    ~UI_TeamSelectMenu();

    void ResetTeamSelect();
    short GetTeamCount();

private:
    MI_TeamSelect * miTeamSelect;

    MI_Image * miTeamSelectLeftHeaderBar;
    MI_Image * miTeamSelectRightHeaderBar;
    MI_Text * miTeamSelectHeaderText;
};

#endif // MENU_TEAMSELECT_H
