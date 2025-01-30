#include "EditorWater.h"

#include "ResourceManager.h"
#include "world.h"


namespace {
constexpr int TILE_COUNT = 3;
}  // namespace


void EditorWater::onSetupKeypress(const SDL_KeyboardEvent& event)
{
    switch (event.keysym.sym) {
        case SDLK_ESCAPE:
            newlyEntered = false;
            break;
    }
}


void EditorWater::onSetupMouseClick(const SDL_MouseButtonEvent& event)
{
    const int tileX = event.x / TILESIZE;
    const int tileY = event.y / TILESIZE;

    if (tileY == 0 && 0 <= tileX && tileX < TILE_COUNT) {
        m_selectedTileId = tileX + 4;
    }

    newlyEntered = false;
}


void EditorWater::renderSetup(CResourceManager& rm)
{
    for (int idx = 0; idx < TILE_COUNT; idx++)
        rm.spr_worldbackground[0].draw(idx * 32, 0, 512 + (idx * 128), 0, 32, 32);
}


bool EditorWater::onTileClicked(WorldMap& world, Vec2s pos, uint8_t button)
{
    WorldMapTile& tile = world.getTiles().at(pos.x, pos.y);
    const short newTileId = (button == SDL_BUTTON_LEFT) ? m_selectedTileId : 0;
    const bool changed = tile.iBackgroundWater != newTileId;
    tile.iBackgroundWater = newTileId;
    return changed;
}
