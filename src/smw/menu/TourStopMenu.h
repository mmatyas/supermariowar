#ifndef MENU_TOURSTOP_H
#define MENU_TOURSTOP_H

#include "uimenu.h"
#include "uicontrol.h"
#include "uicustomcontrol.h"

/*
    Description.
*/

class UI_TourStopMenu : public UI_Menu
{
public:
    UI_TourStopMenu();
    ~UI_TourStopMenu();

    void OpenExitDialog();
    void CloseExitDialog();

    MI_TourStop * miTourStop;

private:
    MI_Image * miTourStopExitDialogImage;
    MI_Text * miTourStopExitDialogExitTourText;
    MI_Button * miTourStopExitDialogYesButton;
    MI_Button * miTourStopExitDialogNoButton;
};

#endif // MENU_TOURSTOP_H
