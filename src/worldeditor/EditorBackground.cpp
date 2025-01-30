#include "EditorBackground.h"

#include "Helpers.h"
#include "ResourceManager.h"
#include "world.h"


void EditorBackground::onSetupKeypress(const SDL_KeyboardEvent& event)
{
    switch (event.keysym.sym) {
        case SDLK_ESCAPE:
            newlyEntered = false;
            return;
    }

    if (event.state == SDL_PRESSED && SDLK_1 <= event.keysym.sym && event.keysym.sym <= SDLK_2) {
        m_page = event.keysym.sym - SDLK_1;
    }
}


void EditorBackground::onSetupMouseClick(const SDL_MouseButtonEvent& event)
{
    if (event.button != SDL_BUTTON_LEFT)
        return;

    short tileX = event.x / TILESIZE;
    short tileY = event.y / TILESIZE;

    short iTileStyleOffset = ((tileX / 4) + (m_page * 5)) * WORLD_BACKGROUND_SPRITE_SET_SIZE;

    tileX %= 4;

    if (tileX == 0) {
        if (tileY == 0)
            m_selectedTileId = 0;
        else if (tileY >= 1 && tileY < 15)
            m_selectedTileId = tileY + 1;
    } else if (tileX == 1) {
        if (tileY == 0)
            m_selectedTileId = 1;
        else if (tileY >= 1 && tileY < 15)
            m_selectedTileId = tileY + 15;
    } else if (tileX == 2) {
        if (tileY >= 0 && tileY < 15)
            m_selectedTileId = tileY + 30;
    } else if (tileX == 3) {
        if (tileY >= 0 && tileY < 15)
            m_selectedTileId = tileY + 45;
    }

    m_selectedTileId += iTileStyleOffset;
    newlyEntered = false;
}


void EditorBackground::renderSetup(CResourceManager& rm)
{
    rm.spr_worldbackground[0].draw(0, 0, m_page * 640, 32, 640, 480);
}


bool EditorBackground::onTileClicked(WorldMap& world, Vec2s pos, uint8_t button)
{
    WorldMapTile& tile = world.getTiles().at(pos.x, pos.y);
    const short newTileId = (button == SDL_BUTTON_LEFT) ? m_selectedTileId : 0;

    bool changed = false;
    if (tile.iBackgroundSprite != newTileId || m_autopaint) {
        if (tile.iBackgroundSprite != newTileId) {
            tile.iBackgroundSprite = newTileId;
            changed = true;
        }

        changed |= updateForeground(tile);

        if ((newTileId % WORLD_BACKGROUND_SPRITE_SET_SIZE) < 2 && m_autopaint)
            changed |= updateCoastline(world, pos);
    }

    return changed;
}
