#pragma once

#include "uicontrol.h"


class MI_Text : public UI_Control {
public:
    MI_Text(std::string text, short x, short y, short w, bool use_large_font, TextAlignment align = TextAlignment::LEFT);

    void SetText(std::string text);
    void Draw() override;

private:
    std::string szText;
    short iw = 0;
    TextAlignment m_align = TextAlignment::LEFT;
    gfxFont* font = nullptr;
};


class MI_HeaderText: public MI_Text {
public:
    MI_HeaderText(std::string text, short x, short y)
        : MI_Text(std::move(text), x, y, 0, true, TextAlignment::CENTER)
    {}
};
