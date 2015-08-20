#ifndef MENU_POWERUPSETTINGS_H
#define MENU_POWERUPSETTINGS_H

#include "uimenu.h"
#include "uicontrol.h"
#include "uicustomcontrol.h"

/*
    In this menu, you can change some powerup settings,
    like fire rate, reloading of [?] blocks and bonus items.
*/

class UI_PowerupSettingsMenu : public UI_Menu
{
public:
    UI_PowerupSettingsMenu();
    ~UI_PowerupSettingsMenu();

private:
    MI_SelectField * miStoredPowerupDelayField;
    MI_SelectField * miItemRespawnField;
    MI_SelectField * miSwapStyleField;
    MI_SelectField * miBonusWheelField;
    MI_SelectField * miKeepPowerupField;
    MI_SelectField * miHiddenBlockRespawnField;
    MI_StoredPowerupResetButton * miStoredPowerupResetButton;
    MI_Button * miPowerupSettingsMenuBackButton;

    MI_Image * miPowerupSettingsMenuLeftHeaderBar;
    MI_Image * miPowerupSettingsMenuRightHeaderBar;
    MI_Text * miPowerupSettingsMenuHeaderText;

    /*
    MI_Image * miPowerupSelectionDialogImage;
    MI_Text * miPowerupSelectionDialogExitText;
    MI_Text * miPowerupSelectionDialogTournamentText;
    MI_Button * miPowerupSelectionDialogYesButton;
    MI_Button * miPowerupSelectionDialogNoButton;
    */
};

#endif // MENU_POWERUPSETTINGS_H
