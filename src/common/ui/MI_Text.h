#pragma once

#include "uicontrol.h"

#include <string>

class gfxFont;


class MI_Text : public UI_Control {
public:
    MI_Text(std::string text, short x, short y, short w, bool use_large_font, TextAlign align = TextAlign::LEFT);

    void SetText(std::string text);
    void Draw() override;

private:
    std::string szText;
    short iw = 0;
    TextAlign m_align = TextAlign::LEFT;
    gfxFont* font = nullptr;
};


class MI_HeaderText: public MI_Text {
public:
    MI_HeaderText(std::string text, short x, short y)
        : MI_Text(std::move(text), x, y, 0, true, TextAlign::CENTER)
    {}
};
