#pragma once

#include "uimenu.h"

class MI_Button;
class MI_Image;
class MI_Text;
template<typename T> class MI_SelectField;


class UI_ProjectileOptionsMenu : public UI_Menu {
public:
    UI_ProjectileOptionsMenu();

private:
    MI_SelectField<short>* miFireballLifeField;
    MI_SelectField<short>* miHammerLifeField;
    MI_SelectField<short>* miHammerDelayField;
    MI_SelectField<bool>* miHammerOneKillField;
    MI_SelectField<short>* miShellLifeField;
    MI_SelectField<short>* miWandFreezeTimeField;

    MI_SelectField<short>* miBlueBlockLifeField;
    MI_SelectField<short>* miGrayBlockLifeField;
    MI_SelectField<short>* miRedBlockLifeField;
    MI_SelectField<short>* miBoomerangStyleField;
    MI_SelectField<short>* miBoomerangLifeField;
    MI_SelectField<short>* miFeatherJumpsField;

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
