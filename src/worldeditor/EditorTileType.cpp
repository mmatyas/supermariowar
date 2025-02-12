#include "EditorTileType.h"

#include "FileList.h"
#include "ResourceManager.h"
#include "world.h"


void EditorTileType::onSetupKeypress(const SDL_KeyboardEvent& event, WorldMap& world)
{
    switch (event.keysym.sym) {
        case SDLK_ESCAPE:
            newlyEntered = false;
            return;
    }
}


void EditorTileType::onSetupMouseClick(const SDL_MouseButtonEvent& event, WorldMap& world)
{
    if (event.button != SDL_BUTTON_LEFT)
        return;

    const short tileX = event.x / TILESIZE;
    const short tileY = event.y / TILESIZE;

    if (0 <= tileX && tileX <= 5 && tileY == 0) {
        m_selectedTileId = tileX;
        newlyEntered = false;
    }
}


void EditorTileType::renderSetup(CResourceManager& rm, const WorldMap& world)
{
    rm.spr_worldforegroundspecial[0].draw(0, 0, 320, 128, 64, 32);
    rm.spr_worldforegroundspecial[0].draw(64, 0, 320, 192, 128, 32);
    rm.spr_worldforegroundspecial[0].draw(64, 0, 448, 64, 128, 32);
}


bool EditorTileType::onTileClicked(WorldMap& world, Vec2s pos, uint8_t button)
{
    WorldMapTile& tile = world.getTiles().at(pos.x, pos.y);
    bool changed = false;

    if (button == SDL_BUTTON_LEFT) {
        if (m_selectedTileId <= 1) {
            if (tile.iForegroundSprite != m_selectedTileId + WORLD_START_SPRITE_OFFSET) {
                tile.iType = 1;
                tile.iForegroundSprite = m_selectedTileId + WORLD_START_SPRITE_OFFSET;
                changed = true;
            }
        } else if (m_selectedTileId <= 5) {  // doors
            if (tile.iType != m_selectedTileId) {
                // if the door was placed on a start tile
                if (tile.iType == 1)
                    tile.iForegroundSprite = 0;

                tile.iType = m_selectedTileId;
                changed = true;
            }
        }
    } else if (button == SDL_BUTTON_RIGHT) {
        if (tile.iType == 1) {
            tile.iForegroundSprite = 0;
            changed = true;
        } else if (tile.iType <= 5) {
            tile.iType = 0;
            changed = true;
        }

        tile.iType = 0;
    }

    return changed;
}
