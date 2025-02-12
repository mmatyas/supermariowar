#include "EditorStartItems.h"

#include "ResourceManager.h"
#include "world.h"


namespace {
bool isInsideRect(Vec2s point, const SDL_Rect& rect)
{
    return rect.x <= point.x && point.x < rect.w + rect.x
        && rect.y <= point.y && point.y < rect.h + rect.y;
}
} // namespace


EditorStartItems::EditorStartItems()
    : EditorBase()
{
    for (size_t idx = 0; idx < m_itemBoxes.size(); idx++) {
        const short col = idx % 12;
        const short row = idx / 12;

        m_itemBoxes[idx].x = 16 + col * 48;
        m_itemBoxes[idx].y = 16 + row * 48;
        m_itemBoxes[idx].w = 32;
        m_itemBoxes[idx].h = 32;
    }

    size_t idx = 0;
    for (short row = 0; row < 4; row++) {
        for (short col = 0; col < 8; col++) {
            m_pickedBoxes[idx].x = 122 + col * 52;
            m_pickedBoxes[idx].y = 240 + row * 64;
            m_pickedBoxes[idx].w = 32;
            m_pickedBoxes[idx].h = 32;
            idx++;
        }
    }
}


void EditorStartItems::onSetupKeypress(const SDL_KeyboardEvent& event, WorldMap& world)
{
    switch (event.keysym.sym) {
    case SDLK_ESCAPE:
        newlyEntered = false;
        return;
    }
}


void EditorStartItems::onSetupMouseClick(const SDL_MouseButtonEvent& event, WorldMap& world)
{
    if (event.state != SDL_PRESSED)
        return;

    const Vec2s clickPos {event.x, event.y};

    if (world.getInitialBonuses().size() < 32) {
        for (short idx = 0; idx < m_itemBoxes.size(); idx++) {
            if (isInsideRect(clickPos, m_itemBoxes[idx])) {
                world.getInitialBonuses().emplace_back(idx);
                break;
            }
        }
    }

    for (short idx = 0; idx < world.getInitialBonuses().size(); idx++) {
        if (isInsideRect(clickPos, m_pickedBoxes[idx])) {
            world.getInitialBonuses().erase(world.getInitialBonuses().begin() + idx);
            break;
        }
    }
}


void EditorStartItems::renderSetup(CResourceManager& rm, const WorldMap& world)
{
    for (short idx = 0; idx < NUM_POWERUPS; idx++) {
        rm.spr_storedpoweruplarge.draw(m_itemBoxes[idx].x, m_itemBoxes[idx].y, idx * 32, 0, 32, 32);
    }

    for (short iWorldItem = 0; iWorldItem < NUM_WORLD_POWERUPS; iWorldItem++) {
        rm.spr_worlditems.draw(m_itemBoxes[iWorldItem + NUM_POWERUPS].x, m_itemBoxes[iWorldItem + NUM_POWERUPS].y, iWorldItem * 32, 0, 32, 32);
    }

    for (short iPopup = 0; iPopup < 4; iPopup++) {
        rm.spr_worlditempopup.draw(0, 416 - (iPopup * 64), 0, 0, 320, 64);
        rm.spr_worlditempopup.draw(320, 416 - (iPopup * 64), 192, 0, 320, 64);
    }

    for (short idx = 0; idx < world.getInitialBonuses().size(); idx++) {
        short iPowerup = world.getInitialBonuses()[idx];
        if (iPowerup >= NUM_POWERUPS)
            rm.spr_worlditems.draw(m_pickedBoxes[idx].x, m_pickedBoxes[idx].y, (iPowerup - NUM_POWERUPS) * 32, 0, 32, 32);
        else
            rm.spr_storedpoweruplarge.draw(m_pickedBoxes[idx].x, m_pickedBoxes[idx].y, iPowerup * 32, 0, 32, 32);
    }
}
