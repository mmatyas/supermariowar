#pragma once

#include "uimenu.h"

class MI_Button;
class MI_Image;
class MI_Text;
template<typename T> class MI_SelectFieldDyn;

/*
    This menu is where you can change various gameplay-related
    settings, such as respawn time, bot difficulty or warp locking.
*/
class UI_GameplayOptionsMenu : public UI_Menu {
public:
    UI_GameplayOptionsMenu();

private:
    MI_SelectFieldDyn<short>* miRespawnField;
    MI_SelectFieldDyn<short>* miShieldStyleField;
    MI_SelectFieldDyn<short>* miShieldTimeField;
    MI_SelectFieldDyn<short>* miBoundsTimeField;
    MI_SelectFieldDyn<short>* miSuicideTimeField;
    MI_SelectFieldDyn<short>* miWarpLockStyleField;
    MI_SelectFieldDyn<short>* miWarpLockTimeField;
    MI_SelectFieldDyn<short>* miBotsField;
    MI_SelectFieldDyn<short>* miPointSpeedField;
    MI_SelectFieldDyn<bool>* miSecretsField;
    MI_Button* miGameplayOptionsMenuBackButton;

    MI_Image* miGameplayOptionsMenuLeftHeaderBar;
    MI_Image* miGameplayOptionsMenuRightHeaderBar;
    MI_Text* miGameplayOptionsMenuHeaderText;
};
