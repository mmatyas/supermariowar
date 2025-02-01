#include "EditorPathSprites.h"

#include "Helpers.h"
#include "ResourceManager.h"
#include "world.h"

#include <array>
#include <cstdint>


namespace {
std::array<short, 9> getForegroundTileValues(const WorldMap& world, Vec2s center)
{
    std::array<short, 9> tiles;
    tiles.fill(0);

    size_t idx = 0;
    for (short y = center.y - 1; y <= center.y + 1; y++) {
        for (short x = center.x - 1; x <= center.x + 1; x++) {
            if (0 <= y && y < world.h() && 0 <= x && x < world.w()) {
                const WorldMapTile& tile = world.getTiles().at(x, y);
                tiles[idx++] = tile.iForegroundSprite;
            }
        }
    }

    return tiles;
}


void autoSetPathSprite(WorldMap& world, Vec2s center)
{
    // TODO: Bounds check
    WorldMapTile& centerTile = world.getTiles().at(center.x, center.y);

    constexpr std::array<short, 16> PATH_TYPES { 6, 4, 3, 5, 6, 4, 1, 5, 6, 2, 3, 5, 6, 4, 3, 5 };

    short iPath = 0;
    short iNeighborIndex = 0;

    short iForegroundSprite = centerTile.iForegroundSprite;
    short iForegroundStyle = iForegroundSprite / WORLD_PATH_SPRITE_SET_SIZE;

    if (iForegroundSprite == 0 || iForegroundSprite >= WORLD_FOREGROUND_STAGE_OFFSET)
        return;

    for (short y = center.y - 1; y <= center.y + 1; y++) {
        for (short x = center.x - 1; x <= center.x + 1; x++) {
            if (x == center.x && y == center.y)
                continue;

            if ((x == center.x && y != center.y) || (x != center.x && y == center.y)) {
                if (0 <= y && y < world.h() && 0 <= x && x < world.w()) {
                    const WorldMapTile& tile = world.getTiles().at(x, y);
                    iForegroundSprite = tile.iForegroundSprite;

                    if ((iForegroundSprite >= WORLD_BRIDGE_SPRITE_OFFSET && iForegroundSprite <= WORLD_BRIDGE_SPRITE_OFFSET + 3) || (iForegroundSprite >= WORLD_FOREGROUND_STAGE_OFFSET && iForegroundSprite <= WORLD_FOREGROUND_STAGE_OFFSET + 399) ||
                        /*(iForegroundSprite >= WORLD_FOREGROUND_SPRITE_OFFSET && iForegroundSprite <= WORLD_FOREGROUND_SPRITE_OFFSET + 179) ||*/
                        /*(iForegroundSprite >= WORLD_FOREGROUND_SPRITE_ANIMATED_OFFSET && iForegroundSprite <= WORLD_FOREGROUND_SPRITE_ANIMATED_OFFSET + 29) ||*/
                        (iForegroundSprite >= WORLD_START_SPRITE_OFFSET && iForegroundSprite <= WORLD_START_SPRITE_OFFSET + 1)) {
                        iPath += 1 << iNeighborIndex;
                    } else if (0 <= iForegroundSprite && iForegroundSprite < WORLD_FOREGROUND_STAGE_OFFSET) {
                        short iPathSprite = iForegroundSprite % WORLD_PATH_SPRITE_SET_SIZE;

                        if (iPathSprite >= 1 && iPathSprite <= 18)
                            iPath += 1 << iNeighborIndex;
                    }
                }

                iNeighborIndex++;
            }
        }
    }

    // #1 == -  2 == |  3 == -o  4 == !  5 == -`  6 == o
    centerTile.iForegroundSprite = adjustedForeground(PATH_TYPES[iPath] + iForegroundStyle * WORLD_PATH_SPRITE_SET_SIZE, centerTile.iBackgroundSprite);
}


void updateRegion(WorldMap& world, Vec2s center)
{
    for (short y = center.y - 1; y <= center.y + 1; y++) {
        for (short x = center.x - 1; x <= center.x + 1; x++) {
            if (0 <= y && y < world.h() && 0 <= x && x < world.w()) {
                autoSetPathSprite(world, {x, y});
            }
        }
    }
}

bool checkforegroundTileValuesChanged(WorldMap& world, Vec2s center, const std::array<short, 9>& oldTiles)
{
    size_t idx = 0;
    for (short y = center.y - 1; y <= center.y + 1; y++) {
        for (short x = center.x - 1; x <= center.x + 1; x++) {
            if (0 <= y && y < world.h() && 0 <= x && x < world.w()) {
                const WorldMapTile& tile = world.getTiles().at(x, y);
                if (tile.iForegroundSprite != oldTiles[idx++])
                    return true;
            }
        }
    }

    return false;
}
}  // namespace


void EditorPathSprites::onSetupKeypress(const SDL_KeyboardEvent& event)
{
    switch (event.keysym.sym) {
        case SDLK_ESCAPE:
            newlyEntered = false;
            return;
    }
}


void EditorPathSprites::onSetupMouseClick(const SDL_MouseButtonEvent& event)
{
    if (event.button != SDL_BUTTON_LEFT)
        return;

    const short tileX = event.x / TILESIZE;
    const short tileY = event.y / TILESIZE;

    if (0 <= tileX && tileX < 8 && 0 <= tileY && tileY < 6) {
        m_selectedTileId = tileY + 1 + tileX * WORLD_PATH_SPRITE_SET_SIZE;
        newlyEntered = false;
    }
}


void EditorPathSprites::renderSetup(CResourceManager& rm)
{
    for (short iPath = 0; iPath < 8; iPath++) {
        rm.spr_worldpaths[0].draw(iPath * 32, 0, (iPath % 4) * 160, (iPath / 4) * 320, 32, 192);
    }
}


bool EditorPathSprites::onTileClicked(WorldMap& world, Vec2s pos, uint8_t button)
{
    WorldMapTile& tile = world.getTiles().at(pos.x, pos.y);
    bool changed = false;

    const short newTileId = (button == SDL_BUTTON_LEFT)
        ? adjustedForeground(m_selectedTileId, tile.iBackgroundSprite)
        : 0;

    if (!m_autopaint && tile.iForegroundSprite != newTileId) {
        tile.iForegroundSprite = newTileId;
        changed = true;
    }

    // Detect if there was a change so we can repaint the screen
    if (m_autopaint) {
        const std::array<short, 9> oldTiles = getForegroundTileValues(world, pos);
        tile.iForegroundSprite = newTileId;
        updateRegion(world, pos);
        changed |= checkforegroundTileValuesChanged(world, pos, oldTiles);
    }

    return changed;
}
