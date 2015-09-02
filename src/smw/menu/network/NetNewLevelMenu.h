#ifndef MENU_NETNEWLEVEL_H
#define MENU_NETNEWLEVEL_H

#include "uimenu.h"
#include "uicontrol.h"

#include <string>

/*
    Description.
*/

class UI_NetNewLevelMenu : public UI_Menu
{
public:
    UI_NetNewLevelMenu();
    ~UI_NetNewLevelMenu();

    std::string getCurrentMapPath();
    void RefreshGameModeButtons();

private:
    MI_ImageSelectField * miNetNewLevelModeField;
    MI_SelectField * miNetNewLevelGoalField[22];
    MI_MapField * miNetNewLevelMapField;
    MI_Button * miNetNewLevelContinueButton;

    MI_Image * miNetNewLevelLeftHeaderBar;
    MI_Image * miNetNewLevelRightHeaderBar;
    MI_Text * miNetNewLevelHeaderText;
};

#endif // MENU_NETNEWLEVEL_H
