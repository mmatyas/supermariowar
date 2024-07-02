#include "MI_PowerupSlider.h"

#include "gfx/gfxSprite.h"
#include "ui/MI_Image.h"


MI_PowerupSlider::MI_PowerupSlider(gfxSprite* nspr, gfxSprite* nsprSlider, gfxSprite* nsprPowerup, short x, short y, short width, short powerupIndex)
    : MI_SliderField(nspr, nsprSlider, x, y, "", width, 0, 0)
    , m_sprPowerup(nsprPowerup)
    , m_powerupIndex(powerupIndex)
    , m_halfWidth((width - 38) / 2)
{
    miModifyImageLeft->SetPosition(m_pos.x + 25, m_pos.y + 4);
    miModifyImageRight->SetPosition(m_pos.x + m_width - 12, m_pos.y + 4);
}

void MI_PowerupSlider::Draw()
{
    if (!m_visible)
        return;

    m_spr->draw(m_pos.x + 38, m_pos.y, 0, (fSelected ? 32 : 0) + m_adjustmentY, m_halfWidth, 32);
    m_spr->draw(m_pos.x + 38 + m_halfWidth, m_pos.y, 550 - m_width + m_halfWidth, (fSelected ? 32 : 0) + m_adjustmentY, m_width - m_halfWidth - 38, 32);

    short iSpacing = (m_width - 100) / ((short)m_items.size() - 1);
    short iSpot = 0;

    for (unsigned int index = 0; index < m_items.size(); index++) {
        if (index < m_items.size() - 1)
            m_sprSlider->draw(m_pos.x + iSpot + 56, m_pos.y + 10, 0, 0, iSpacing, 13);
        else
            m_sprSlider->draw(m_pos.x + iSpot + 56, m_pos.y + 10, 164, 0, 4, 13);

        iSpot += iSpacing;
    }

    m_sprSlider->draw(m_pos.x + (m_index * iSpacing) + 54, m_pos.y + 8, 168, 0, 8, 16);
    m_sprSlider->draw(m_pos.x + m_width - 34, m_pos.y + 8, m_index * 16, 16, 16, 16);
    m_sprPowerup->draw(m_pos.x, m_pos.y, m_powerupIndex * 32, 0, 32, 32);

    const bool drawLeft = m_index > 0;
    if (m_wraps || drawLeft)
        miModifyImageLeft->Draw();

    const bool drawRight = (m_index + 1) < m_items.size();
    if (m_wraps || drawRight)
        miModifyImageRight->Draw();
}
