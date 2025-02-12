#include "EditorPaths.h"

#include "path.h"
#include "ResourceManager.h"
#include "world.h"


namespace {
void autoSetPath(WorldMap& world, Vec2s center)
{
    const short iPathTypes[16] = { 11, 1, 2, 3, 2, 4, 2, 8, 1, 1, 6, 7, 5, 9, 10, 11 };

    short iPath = 0;
    short iNeighborIndex = 0;

    WorldMapTile& centerTile = world.getTiles().at(center.x, center.y);
    if (centerTile.iConnectionType == 0)
        return;

    for (short row = center.y - 1; row <= center.y + 1; row++) {
        for (short col = center.x - 1; col <= center.x + 1; col++) {
            if (col == center.x && row == center.y)
                continue;

            if ((col == center.x && row != center.y) || (col != center.x && row == center.y)) {
                if (0 <= row && row < world.h() && 0 <= col && col < world.w()) {
                    const WorldMapTile& tile = world.getTiles().at(col, row);
                    if (tile.iConnectionType > 0)
                        iPath += 1 << iNeighborIndex;
                }

                iNeighborIndex++;
            }
        }
    }

    // #1 == |  2 == -  3 == -!  4 == L  5 == ,-  6 == -,
    // #7 == -|  8 == -`-  9 == |-  10 == -,-  11 == +

    short iPathType = iPathTypes[iPath];
    short iForegroundSprite = centerTile.iForegroundSprite;

    if (iPathType == 2 && iForegroundSprite >= WORLD_BRIDGE_SPRITE_OFFSET && iForegroundSprite <= WORLD_BRIDGE_SPRITE_OFFSET + 1) {
        iPathType = iForegroundSprite - WORLD_BRIDGE_SPRITE_OFFSET + 12;
    } else if (iPathType == 1 && iForegroundSprite >= WORLD_BRIDGE_SPRITE_OFFSET + 2 && iForegroundSprite <= WORLD_BRIDGE_SPRITE_OFFSET + 3) {
        iPathType = iForegroundSprite - WORLD_BRIDGE_SPRITE_OFFSET + 12;
    }

    centerTile.iConnectionType = iPathType;
}


void updatePath(WorldMap& world, Vec2s center)
{
    for (short row = center.y - 1; row <= center.y + 1; row++) {
        for (short col = center.x - 1; col <= center.x + 1; col++) {
            if (0 <= row && row < world.h() && 0 <= col && col < world.w()) {
                autoSetPath(world, {col, row});
            }
        }
    }
}
}  // namespace


void EditorPaths::loadAssets()
{
    m_sprPath.init(convertPath("gfx/leveleditor/leveleditor_world_path.png"), colors::MAGENTA);
}


void EditorPaths::onSetupKeypress(const SDL_KeyboardEvent& event, WorldMap& world)
{
    switch (event.keysym.sym) {
    case SDLK_ESCAPE:
        newlyEntered = false;
        return;
    }
}


void EditorPaths::onSetupMouseClick(const SDL_MouseButtonEvent& event, WorldMap& world)
{
    if (event.button != SDL_BUTTON_LEFT)
        return;

    const short tileX = event.x / TILESIZE;
    const short tileY = event.y / TILESIZE;

    if (tileX >= 0 && tileX <= 15 && tileY == 0) {
        m_selectedTileId = tileX + 1;
        newlyEntered = false;
    }
}


void EditorPaths::renderSetup(CResourceManager& rm, const WorldMap& world)
{
    m_sprPath.draw(0, 0, 0, 0, 480, 32);
}


void EditorPaths::renderEdit(WorldMap& world, Vec2s offsetTile, Vec2s offsetPx, CResourceManager& rm)
{
    for (short row = offsetTile.y; row < offsetTile.y + 15 && row < world.h(); row++) {
        for (short col = offsetTile.x; col <= offsetTile.x + 20 && col < world.w(); col++) {
            const short iConnection = world.getTiles().at(col, row).iConnectionType;
            if (iConnection == 0)
                continue;

            const short drawX = (col - offsetTile.x) * TILESIZE + offsetPx.x;
            const short drawY = (row - offsetTile.y) * TILESIZE + offsetPx.y;
            m_sprPath.draw(drawX, drawY, (iConnection - 1) * 32, 0, TILESIZE, TILESIZE);
        }
    }
}


bool EditorPaths::onTileClicked(WorldMap& world, Vec2s pos, uint8_t button)
{
    WorldMapTile& tile = world.getTiles().at(pos.x, pos.y);
    const short newTileId = (button == SDL_BUTTON_LEFT) ? m_selectedTileId : 0;
    tile.iConnectionType = newTileId;

    if (m_autopaint)
        updatePath(world, pos);

    return false;
}
