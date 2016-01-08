#ifndef MENU_PROJECTILEOPTIONS_H
#define MENU_PROJECTILEOPTIONS_H

/*
    TODO: Description.
*/

#include "uimenu.h"

class MI_Button;
class MI_Image;
class MI_SelectField;
class MI_Text;

class UI_ProjectileOptionsMenu : public UI_Menu
{
public:
    UI_ProjectileOptionsMenu();
    ~UI_ProjectileOptionsMenu();

private:
    MI_SelectField * miFireballLifeField;
    MI_SelectField * miHammerLifeField;
    MI_SelectField * miHammerDelayField;
    MI_SelectField * miHammerOneKillField;
    MI_SelectField * miShellLifeField;
    MI_SelectField * miWandFreezeTimeField;

    MI_SelectField * miBlueBlockLifeField;
    MI_SelectField * miGrayBlockLifeField;
    MI_SelectField * miRedBlockLifeField;
    MI_SelectField * miBoomerangStyleField;
    MI_SelectField * miBoomerangLifeField;
    MI_SelectField * miFeatherJumpsField;

    MI_Button * miProjectileOptionsMenuBackButton;

    MI_Text * miFireballText;
    MI_Text * miHammerText;
    MI_Text * miBoomerangText;
    MI_Text * miFeatherText;
    MI_Text * miShellText;
    MI_Text * miWandText;
    MI_Text * miBlueBlockText;

    MI_Image * miProjectileOptionsMenuLeftHeaderBar;
    MI_Image * miProjectileOptionsMenuRightHeaderBar;
    MI_Text * miProjectileOptionsMenuHeaderText;
};

#endif // MENU_PROJECTILESOPTIONS_H
