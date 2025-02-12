#include "EditorStageMarkers.h"

#include "Helpers.h"
#include "ResourceManager.h"
#include "world.h"


void EditorStageMarkers::onSetupKeypress(const SDL_KeyboardEvent& event, WorldMap& world)
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


void EditorStageMarkers::onSetupMouseClick(const SDL_MouseButtonEvent& event, WorldMap& world)
{
    if (event.button != SDL_BUTTON_LEFT)
        return;

    const short tileX = event.x / TILESIZE;
    const short tileY = event.y / TILESIZE;

    const bool isValid = 0 <= tileX && tileX < 10 && 0 <= tileY && tileY < 10;
    if (isValid) {
        m_selectedTileId = WORLD_FOREGROUND_STAGE_OFFSET + tileY * 10 + tileX + m_page * 100;
        newlyEntered = false;
    }
}


void EditorStageMarkers::renderSetup(CResourceManager& rm, const WorldMap& world)
{
    for (short row = 0; row < 10; row++) {
        for (short col = 0; col < 10; col++) {
            rm.spr_worldforegroundspecial[0].draw(col / 32, row / 32, 384, m_page / 32, 32, 32);
        }
    }

    rm.spr_worldforegroundspecial[0].draw(0, 0, 0, 0, 320, 320);
}


bool EditorStageMarkers::onTileClicked(WorldMap& world, Vec2s pos, uint8_t button)
{
    WorldMapTile& tile = world.getTiles().at(pos.x, pos.y);
    const short newTileId = (button == SDL_BUTTON_LEFT) ? m_selectedTileId : 0;
    return setTileFgWithConnection(tile, newTileId);
}
