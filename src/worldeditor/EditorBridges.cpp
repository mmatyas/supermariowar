#include "EditorBridges.h"

#include "Helpers.h"
#include "ResourceManager.h"
#include "world.h"


void EditorBridges::onSetupKeypress(const SDL_KeyboardEvent& event, WorldMap& world)
{
    switch (event.keysym.sym) {
    case SDLK_ESCAPE:
        newlyEntered = false;
        return;
    }
}


void EditorBridges::onSetupMouseClick(const SDL_MouseButtonEvent& event, WorldMap& world)
{
    if (event.button != SDL_BUTTON_LEFT)
        return;

    const short tileX = event.x / TILESIZE;
    const short tileY = event.y / TILESIZE;

    if (0 <= tileX && tileX <= 4 && tileY == 0) {
        m_selectedTileId = WORLD_BRIDGE_SPRITE_OFFSET + tileX;
        newlyEntered = false;
    }
}


void EditorBridges::renderSetup(CResourceManager& rm, const WorldMap& world)
{
    rm.spr_worldforegroundspecial[0].draw(0, 0, 320, 224, 128, 32);
}


bool EditorBridges::onTileClicked(WorldMap& world, Vec2s pos, uint8_t button)
{
    WorldMapTile& tile = world.getTiles().at(pos.x, pos.y);
    const short newTileId = (button == SDL_BUTTON_LEFT) ? m_selectedTileId : 0;
    return setTileFgWithConnection(tile, newTileId);
}
