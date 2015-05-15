#ifndef MENU_SCREENSETTINGS_H
#define MENU_SCREENSETTINGS_H

#include "uimenu.h"
#include "uicontrol.h"

/*
    Description.
*/

class UI_ScreenSettingsMenu : public UI_Menu
{
public:
    UI_ScreenSettingsMenu();
    ~UI_ScreenSettingsMenu();

private:
    MI_Button * miScreenResizeButton;
    MI_SelectField * miScreenHardwareFilterField;
    MI_SelectField * miScreenFlickerFilterField;
    MI_SelectField * miScreenSoftFilterField;
    //MI_SelectField * miScreenAspectRatioField;

    MI_Button * miScreenSettingsMenuBackButton;

    MI_Image * miScreenSettingsMenuLeftHeaderBar;
    MI_Image * miScreenSettingsMenuRightHeaderBar;
    MI_Text * miScreenSettingsMenuHeaderText;
};

#endif // MENU_SCREENSETTINGS_H
