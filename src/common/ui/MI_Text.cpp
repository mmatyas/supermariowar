#include "MI_Text.h"

#include "ResourceManager.h"

extern CResourceManager* rm;


MI_Text::MI_Text(std::string text, short x, short y, short w, bool use_large_font, TextAlignment align)
    : UI_Control(x, y)
    , szText(std::move(text))
    , iw(w)
    , m_align(align)
    , font(use_large_font ? &rm->menu_font_large : &rm->menu_font_small)
{}

void MI_Text::SetText(std::string text)
{
    szText = std::move(text);
}

void MI_Text::Draw()
{
    if (!fShow)
        return;

    if (m_align == TextAlignment::LEFT && iw == 0)
        font->draw(ix, iy, szText.c_str());
    else if (m_align == TextAlignment::LEFT)
        font->drawChopRight(ix, iy, iw, szText.c_str());
    else if (m_align == TextAlignment::CENTER)
        font->drawCentered(ix, iy, szText.c_str());
    else if (m_align == TextAlignment::RIGHT)
        font->drawRightJustified(ix, iy, szText.c_str());
}
