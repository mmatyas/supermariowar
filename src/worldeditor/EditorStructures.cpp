#include "EditorStructures.h"

#include "Helpers.h"
#include "ResourceManager.h"
#include "world.h"


void EditorStructures::onSetupKeypress(const SDL_KeyboardEvent& event)
{
    switch (event.keysym.sym) {
    case SDLK_ESCAPE:
        newlyEntered = false;
        return;
    }
}


void EditorStructures::onSetupMouseClick(const SDL_MouseButtonEvent& event)
{
    if (event.button != SDL_BUTTON_LEFT)
        return;

    const short tileX = event.x / TILESIZE;
    const short tileY = event.y / TILESIZE;

    if (0 <= tileY && tileY < 15) {
        if (0 <= tileX && tileX < 12) {
            m_selectedTileId = WORLD_FOREGROUND_SPRITE_OFFSET + tileX + tileY * 12;
            newlyEntered = false;
        } else if (12 <= tileX && tileX < 14) {
            m_selectedTileId = WORLD_FOREGROUND_SPRITE_ANIMATED_OFFSET + tileY + (tileX - 12) * 15;
            newlyEntered = false;
        }
    }
}


void EditorStructures::renderSetup(CResourceManager& rm)
{
    rm.spr_worldforeground[0].draw(0, 0, 0, 0, 416, 480);
    rm.spr_worldforeground[0].draw(416, 0, 512, 0, 32, 480);
}


bool EditorStructures::onTileClicked(WorldMap& world, Vec2s pos, uint8_t button)
{
    WorldMapTile& tile = world.getTiles().at(pos.x, pos.y);
    const short newTileId = (button == SDL_BUTTON_LEFT) ? m_selectedTileId : 0;
    return setTileFgWithConnection(tile, newTileId);
}
