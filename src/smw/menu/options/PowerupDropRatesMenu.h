#pragma once

#include "uimenu.h"

class MI_Image;
class MI_PowerupSelection;
class MI_Text;

/*
    In this menu you can set how frequently can various
    powerups pop out of a [?] block.
*/
class UI_PowerupDropRatesMenu : public UI_Menu {
public:
    UI_PowerupDropRatesMenu();

private:
    MI_PowerupSelection* miPowerupSelection;

    // MI_PowerupSlider * miPowerupSlider[NUM_POWERUPS];
    // MI_Button * miPowerupSelectionBackButton;
    // MI_Button * miPowerupSelectionRestoreDefaultsButton;

    MI_Image* miPowerupSelectionLeftHeaderBar;
    MI_Image* miPowerupSelectionMenuRightHeaderBar;
    MI_Text* miPowerupSelectionMenuHeaderText;
};
