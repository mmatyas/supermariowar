#pragma once

#include "uimenu.h"

class MI_Button;
class MI_Image;
class MI_Text;
template<typename T> class MI_SelectField;

/*
    This menu is where you can change various gameplay-related
    settings, such as respawn time, bot difficulty or warp locking.
*/
class UI_GameplayOptionsMenu : public UI_Menu {
public:
    UI_GameplayOptionsMenu();

private:
    MI_SelectField<short>* miRespawnField;
    MI_SelectField<short>* miShieldStyleField;
    MI_SelectField<short>* miShieldTimeField;
    MI_SelectField<short>* miBoundsTimeField;
    MI_SelectField<short>* miSuicideTimeField;
    MI_SelectField<short>* miWarpLockStyleField;
    MI_SelectField<short>* miWarpLockTimeField;
    MI_SelectField<short>* miBotsField;
    MI_SelectField<short>* miPointSpeedField;
    MI_SelectField<bool>* miSecretsField;
    MI_Button* miGameplayOptionsMenuBackButton;

    MI_Image* miGameplayOptionsMenuLeftHeaderBar;
    MI_Image* miGameplayOptionsMenuRightHeaderBar;
    MI_Text* miGameplayOptionsMenuHeaderText;
};
