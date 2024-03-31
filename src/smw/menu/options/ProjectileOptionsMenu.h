#pragma once

#include "uimenu.h"

class MI_Button;
class MI_Image;
class MI_Text;
template<typename T> class MI_SelectFieldDyn;


class UI_ProjectileOptionsMenu : public UI_Menu {
public:
    UI_ProjectileOptionsMenu();

private:
    MI_SelectFieldDyn<short>* miFireballLifeField;
    MI_SelectFieldDyn<short>* miHammerLifeField;
    MI_SelectFieldDyn<short>* miHammerDelayField;
    MI_SelectFieldDyn<bool>* miHammerOneKillField;
    MI_SelectFieldDyn<short>* miShellLifeField;
    MI_SelectFieldDyn<short>* miWandFreezeTimeField;

    MI_SelectFieldDyn<short>* miBlueBlockLifeField;
    MI_SelectFieldDyn<short>* miGrayBlockLifeField;
    MI_SelectFieldDyn<short>* miRedBlockLifeField;
    MI_SelectFieldDyn<short>* miBoomerangStyleField;
    MI_SelectFieldDyn<short>* miBoomerangLifeField;
    MI_SelectFieldDyn<short>* miFeatherJumpsField;

    MI_Button* miProjectileOptionsMenuBackButton;

    MI_Text* miFireballText;
    MI_Text* miHammerText;
    MI_Text* miBoomerangText;
    MI_Text* miFeatherText;
    MI_Text* miShellText;
    MI_Text* miWandText;
    MI_Text* miBlueBlockText;

    MI_Image* miProjectileOptionsMenuLeftHeaderBar;
    MI_Image* miProjectileOptionsMenuRightHeaderBar;
    MI_Text* miProjectileOptionsMenuHeaderText;
};
