#include "MI_StoredPowerupResetButton.h"

#include "GameValues.h"
#include "ResourceManager.h"

extern CResourceManager* rm;
extern CGameValues game_values;


MI_StoredPowerupResetButton::MI_StoredPowerupResetButton(gfxSprite* nspr, short x, short y, std::string name, short width, TextAlign align) :
    MI_Button(nspr, x, y, std::move(name), width, align)
{}


void MI_StoredPowerupResetButton::Draw()
{
    if (!m_visible)
        return;

    MI_Button::Draw();

    for (short iPowerup = 0; iPowerup < 4; iPowerup++) {
        rm->spr_selectfield.draw(m_pos.x + iWidth - 142 + iPowerup * 30, m_pos.y + 4, 188, 88, 24, 24);
        rm->spr_storedpowerupsmall.draw(m_pos.x + iWidth - 138 + iPowerup * 30, m_pos.y + 8, game_values.storedpowerups[iPowerup] * 16, 0, 16, 16);
    }
}
