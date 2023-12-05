#pragma once

#include "uimenu.h"

class MI_BonusWheel;


class UI_BonusWheelMenu : public UI_Menu {
public:
    UI_BonusWheelMenu();

    MI_BonusWheel* miBonusWheel = nullptr;
};
