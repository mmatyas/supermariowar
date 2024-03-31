#pragma once

#include "uimenu.h"

class MI_Button;
class MI_Image;
class MI_StoredPowerupResetButton;
class MI_Text;
template<typename T> class MI_SelectFieldDyn;

/*
    In this menu, you can change some powerup settings,
    like fire rate, reloading of [?] blocks and bonus items.
*/
class UI_PowerupSettingsMenu : public UI_Menu {
public:
    UI_PowerupSettingsMenu();

private:
    MI_SelectFieldDyn<short>* miStoredPowerupDelayField = nullptr;
    MI_SelectFieldDyn<short>* miItemRespawnField = nullptr;
    MI_SelectFieldDyn<short>* miSwapStyleField = nullptr;
    MI_SelectFieldDyn<short>* miBonusWheelField = nullptr;
    MI_SelectFieldDyn<bool>* miKeepPowerupField = nullptr;
    MI_SelectFieldDyn<short>* miHiddenBlockRespawnField = nullptr;
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
