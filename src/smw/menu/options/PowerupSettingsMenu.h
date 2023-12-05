#pragma once

#include "uimenu.h"

class MI_Button;
class MI_Image;
class MI_SelectField;
class MI_StoredPowerupResetButton;
class MI_Text;

/*
    In this menu, you can change some powerup settings,
    like fire rate, reloading of [?] blocks and bonus items.
*/

class UI_PowerupSettingsMenu : public UI_Menu {
public:
    UI_PowerupSettingsMenu();
    ~UI_PowerupSettingsMenu();

private:
    MI_SelectField* miStoredPowerupDelayField = nullptr;
    MI_SelectField* miItemRespawnField = nullptr;
    MI_SelectField* miSwapStyleField = nullptr;
    MI_SelectField* miBonusWheelField = nullptr;
    MI_SelectField* miKeepPowerupField = nullptr;
    MI_SelectField* miHiddenBlockRespawnField = nullptr;
    MI_StoredPowerupResetButton* miStoredPowerupResetButton = nullptr;
    MI_Button* miPowerupSettingsMenuBackButton = nullptr;

    MI_Image* miPowerupSettingsMenuLeftHeaderBar = nullptr;
    MI_Image* miPowerupSettingsMenuRightHeaderBar = nullptr;
    MI_Text* miPowerupSettingsMenuHeaderText = nullptr;

    /*
    MI_Image* miPowerupSelectionDialogImage = nullptr;
    MI_Text* miPowerupSelectionDialogExitText = nullptr;
    MI_Text* miPowerupSelectionDialogTournamentText = nullptr;
    MI_Button* miPowerupSelectionDialogYesButton = nullptr;
    MI_Button* miPowerupSelectionDialogNoButton = nullptr;
    */
};
