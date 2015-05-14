#include "BonusWheelMenu.h"

UI_BonusWheelMenu::UI_BonusWheelMenu() : UI_Menu()
{
    miBonusWheel = new MI_BonusWheel(144, 38);

    AddControl(miBonusWheel, NULL, NULL, NULL, NULL);
    SetHeadControl(miBonusWheel);
    SetCancelCode(MENU_CODE_BONUS_DONE);
};

UI_BonusWheelMenu::~UI_BonusWheelMenu() {
}
