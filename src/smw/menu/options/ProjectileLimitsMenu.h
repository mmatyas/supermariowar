#pragma once

#include "uimenu.h"

class MI_Button;
class MI_Image;
class MI_Text;
template<typename T> class MI_SelectFieldDyn;

/*
    You can set the maximum number of uses of certain
    projectile weapons and special player states.
*/
class UI_ProjectileLimitsMenu : public UI_Menu {
public:
    UI_ProjectileLimitsMenu();

private:
    MI_SelectFieldDyn<short>* miFireballLimitField;
    MI_SelectFieldDyn<short>* miHammerLimitField;
    MI_SelectFieldDyn<short>* miBoomerangLimitField;
    MI_SelectFieldDyn<short>* miFeatherLimitField;
    MI_SelectFieldDyn<short>* miLeafLimitField;
    MI_SelectFieldDyn<short>* miPwingsLimitField;
    MI_SelectFieldDyn<short>* miTanookiLimitField;
    MI_SelectFieldDyn<short>* miBombLimitField;
    MI_SelectFieldDyn<short>* miWandLimitField;

    MI_Button* miProjectilesLimitsMenuBackButton;

    MI_Image* miProjectilesLimitsMenuLeftHeaderBar;
    MI_Image* miProjectilesLimitsMenuRightHeaderBar;
    MI_Text* miProjectilesLimitsMenuHeaderText;
};
