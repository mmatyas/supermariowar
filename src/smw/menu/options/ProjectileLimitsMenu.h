#pragma once

#include "uimenu.h"

class MI_Button;
class MI_Image;
class MI_Text;
template<typename T> class MI_SelectField;

/*
    You can set the maximum number of uses of certain
    projectile weapons and special player states.
*/
class UI_ProjectileLimitsMenu : public UI_Menu {
public:
    UI_ProjectileLimitsMenu();

private:
    MI_SelectField<short>* miFireballLimitField;
    MI_SelectField<short>* miHammerLimitField;
    MI_SelectField<short>* miBoomerangLimitField;
    MI_SelectField<short>* miFeatherLimitField;
    MI_SelectField<short>* miLeafLimitField;
    MI_SelectField<short>* miPwingsLimitField;
    MI_SelectField<short>* miTanookiLimitField;
    MI_SelectField<short>* miBombLimitField;
    MI_SelectField<short>* miWandLimitField;

    MI_Button* miProjectilesLimitsMenuBackButton;

    MI_Image* miProjectilesLimitsMenuLeftHeaderBar;
    MI_Image* miProjectilesLimitsMenuRightHeaderBar;
    MI_Text* miProjectilesLimitsMenuHeaderText;
};
