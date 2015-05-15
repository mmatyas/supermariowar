#ifndef MENU_GAMEPLAYOPTIONS_H
#define MENU_GAMEPLAYOPTIONS_H

#include "uimenu.h"
#include "uicontrol.h"

/*
    This menu is where you can change various gameplay-related
    settings, such as respawn time, bot difficulty or warp locking.
*/

class UI_GameplayOptionsMenu : public UI_Menu
{
public:
    UI_GameplayOptionsMenu();
    ~UI_GameplayOptionsMenu();

private:
    MI_SelectField * miRespawnField;
    MI_SelectField * miShieldStyleField;
    MI_SelectField * miShieldTimeField;
    MI_SelectField * miBoundsTimeField;
    MI_SelectField * miSuicideTimeField;
    MI_SelectField * miWarpLockStyleField;
    MI_SelectField * miWarpLockTimeField;
    MI_SelectField * miBotsField;
    MI_SelectField * miPointSpeedField;
    MI_Button * miGameplayOptionsMenuBackButton;

    MI_Image * miGameplayOptionsMenuLeftHeaderBar;
    MI_Image * miGameplayOptionsMenuRightHeaderBar;
    MI_Text * miGameplayOptionsMenuHeaderText;
};

#endif // MENU_GAMEPLAYOPTIONS_H
