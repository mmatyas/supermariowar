#pragma once

#include "ui/MI_Button.h"


class MI_StoredPowerupResetButton : public MI_Button {
public:
    MI_StoredPowerupResetButton(gfxSprite* nspr, short x, short y, std::string name, short width, TextAlign align = TextAlign::LEFT);

    void Draw() override;
};
