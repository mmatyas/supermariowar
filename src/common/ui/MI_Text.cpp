#include "MI_Text.h"

#include "ResourceManager.h"

extern CResourceManager* rm;


MI_Text::MI_Text(std::string text, short x, short y, short w, bool use_large_font, TextAlign align)
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
    if (!m_visible)
        return;

    if (m_align == TextAlign::LEFT && iw == 0)
        font->draw(m_pos.x, m_pos.y, szText.c_str());
    else if (m_align == TextAlign::LEFT)
        font->drawChopRight(m_pos.x, m_pos.y, iw, szText.c_str());
    else if (m_align == TextAlign::CENTER)
        font->drawCentered(m_pos.x, m_pos.y, szText.c_str());
    else if (m_align == TextAlign::RIGHT)
        font->drawRightJustified(m_pos.x, m_pos.y, szText.c_str());
}
