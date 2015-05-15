#ifndef MENU_POWERUPDROPRATES_H
#define MENU_POWERUPDROPRATES_H

#include "uimenu.h"
#include "uicontrol.h"
#include "uicustomcontrol.h"

/*
    In this menu you can set how frequently can various
    powerups pop out of a [?] block.
*/

class UI_PowerupDropRatesMenu : public UI_Menu
{
public:
    UI_PowerupDropRatesMenu();
    ~UI_PowerupDropRatesMenu();

private:
    MI_PowerupSelection * miPowerupSelection;

    //MI_PowerupSlider * miPowerupSlider[NUM_POWERUPS];
    //MI_Button * miPowerupSelectionBackButton;
    //MI_Button * miPowerupSelectionRestoreDefaultsButton;

    MI_Image * miPowerupSelectionLeftHeaderBar;
    MI_Image * miPowerupSelectionMenuRightHeaderBar;
    MI_Text * miPowerupSelectionMenuHeaderText;
};

#endif // MENU_POWERUPDROPRATES_H
