#ifndef MENU_BONUSWHEEL_H
#define MENU_BONUSWHEEL_H

#include "uimenu.h"
#include "uicustomcontrol.h"

/*
    Description.
*/

class UI_BonusWheelMenu : public UI_Menu
{
public:
    UI_BonusWheelMenu();
    ~UI_BonusWheelMenu();

    MI_BonusWheel * miBonusWheel;
};

#endif // MENU_BONUSWHEEL_H
