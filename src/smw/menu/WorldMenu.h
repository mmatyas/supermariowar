#ifndef MENU_WORLD_H
#define MENU_WORLD_H

#include "uimenu.h"

class MI_Button;
class MI_Image;
class MI_Text;
class MI_TourStop;
class MI_World;

/*
    Description.
*/

class UI_WorldMenu : public UI_Menu
{
public:
    UI_WorldMenu();
    ~UI_WorldMenu();

    void OpenStageStart();
    void CloseStageStart();
    void OpenExitDialog();
    void CloseExitDialog();

    MI_World * miWorld;
    MI_TourStop * miWorldStop;

private:
    MI_Image * miWorldExitDialogImage;
    MI_Text * miWorldExitDialogExitTourText;
    MI_Button * miWorldExitDialogYesButton;
    MI_Button * miWorldExitDialogNoButton;
};

#endif // MENU_WORLD_H
