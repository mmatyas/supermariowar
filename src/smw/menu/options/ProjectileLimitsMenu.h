#ifndef MENU_YYY_H
#define MENU_YYY_H

#include "uimenu.h"
#include "uicontrol.h"

/*
    You can set the maximum number of uses of certain
    projectile weapons and special player states.
*/

class UI_ProjectileLimitsMenu : public UI_Menu
{
public:
    UI_ProjectileLimitsMenu();
    ~UI_ProjectileLimitsMenu();

private:
    MI_SelectField * miFireballLimitField;
    MI_SelectField * miHammerLimitField;
    MI_SelectField * miBoomerangLimitField;
    MI_SelectField * miFeatherLimitField;
    MI_SelectField * miLeafLimitField;
    MI_SelectField * miPwingsLimitField;
    MI_SelectField * miTanookiLimitField;
    MI_SelectField * miBombLimitField;
    MI_SelectField * miWandLimitField;

    MI_Button * miProjectilesLimitsMenuBackButton;

    MI_Image * miProjectilesLimitsMenuLeftHeaderBar;
    MI_Image * miProjectilesLimitsMenuRightHeaderBar;
    MI_Text * miProjectilesLimitsMenuHeaderText;
};

#endif // MENU_YYY_H
