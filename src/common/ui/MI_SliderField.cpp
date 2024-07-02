#include "MI_SliderField.h"

#include "input.h"
#include "ResourceManager.h"
#include "ui/MI_Image.h"

extern CResourceManager* rm;

/**************************************
 * MI_SliderField Class
 **************************************/

MI_SliderField::MI_SliderField(gfxSprite * nspr, gfxSprite * nsprSlider, short x, short y, const char * name, short width, short indent1, short indent2)
    : MI_SelectField<short>(nspr, x, y, name, width, indent1)
{
    m_indent2 = indent2;
    m_sprSlider = nsprSlider;

    SetPosition(x, y);
}

MI_SliderField::~MI_SliderField()
{}

void MI_SliderField::SetPosition(short x, short y)
{
    MI_SelectField<short>::SetPosition(x, y);
    miModifyImageLeft->SetPosition(m_pos.x + m_indent - 26, m_pos.y + 4);
    miModifyImageRight->SetPosition(m_pos.x + m_width - 16, m_pos.y + 4);
}

void MI_SliderField::Draw()
{
    if (!m_visible)
        return;

    m_spr->draw(m_pos.x, m_pos.y, 0, (fSelected ? 32 : 0) + m_adjustmentY, m_indent - 16, 32);
    m_spr->draw(m_pos.x + m_indent - 16, m_pos.y, 0, (fSelected ? 96 : 64), 32, 32);
    m_spr->draw(m_pos.x + m_indent + 16, m_pos.y, 528 - m_width + m_indent, (fSelected ? 32 : 0) + m_adjustmentY, m_width - m_indent - 16, 32);

    rm->menu_font_large.drawChopRight(m_pos.x + 16, m_pos.y + 5, m_indent - 8, m_name.c_str());

    if (!m_items.empty()) {
        rm->menu_font_large.drawChopRight(m_pos.x + m_indent2 + 16, m_pos.y + 5, m_width - m_indent2 - 24, currentItem().name.c_str());
    }

    short iSpacing = (m_indent2 - m_indent - 20) / ((short)m_items.size() - 1);
    short iSpot = 0;

    for (unsigned int index = 0; index < m_items.size(); index++) {
        if (index < m_items.size() - 1)
            m_sprSlider->draw(m_pos.x + m_indent + iSpot + 16, m_pos.y + 10, 0, 0, iSpacing, 13);
        else
            m_sprSlider->draw(m_pos.x + m_indent + iSpot + 16, m_pos.y + 10, 164, 0, 4, 13);

        iSpot += iSpacing;
    }

    m_sprSlider->draw(m_pos.x + m_indent + (m_index * iSpacing) + 14, m_pos.y + 8, 168, 0, 8, 16);

    const bool drawLeft = m_index > 0;
    if (m_wraps || drawLeft)
        miModifyImageLeft->Draw();

    const bool drawRight = (m_index + 1) < m_items.size();
    if (m_wraps || drawRight)
        miModifyImageRight->Draw();
}

MenuCodeEnum MI_SliderField::SendInput(CPlayerInput * playerInput)
{
    for (int iPlayer = 0; iPlayer < 4; iPlayer++) {
        if (playerInput->outputControls[iPlayer].menu_scrollfast.fPressed) {
            if (m_index == 0)
                while (moveNext());
            else
                while (movePrev());

            return mcItemChangedCode;
        }
    }

    return MI_SelectField<short>::SendInput(playerInput);
}
