#ifndef MENU_NETNEWLEVEL_H
#define MENU_NETNEWLEVEL_H

#include "uimenu.h"

#include <string>

class MI_Button;
class MI_Image;
class MI_ImageSelectField;
class MI_MapField;
class MI_SelectField;
class MI_Text;
class UI_GameSettingsMenu;

/*
    TODO: Description.
*/

class UI_NetNewRoomSettingsMenu : public UI_Menu
{
public:
    UI_NetNewRoomSettingsMenu(const UI_GameSettingsMenu*);
    ~UI_NetNewRoomSettingsMenu();

    std::string getCurrentMapPath();
    void RefreshGameModeButtons();

    short getSelectedGameModeID();

private:
    MI_ImageSelectField * miModeField;
    MI_SelectField * miGoalField[22];
    MI_Button * miModeSettingsButton;
    MI_MapField * miMapField;
    MI_Button * miContinueButton;

    MI_Image * miLeftHeaderBar;
    MI_Image * miRightHeaderBar;
    MI_Text * miHeaderText;
};

#endif // MENU_NETNEWLEVEL_H
