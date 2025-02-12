#include "EditorWarps.h"

#include "path.h"
#include "ResourceManager.h"
#include "world.h"

#include <algorithm>


namespace {
void addWarpToTile(WorldMap& world, Vec2s pos, short newWarpId)
{
    const auto it = std::find_if(
        world.getWarps().begin(),
        world.getWarps().end(),
        [newWarpId](const WorldWarp& warp){ return warp.id == newWarpId; });

    if (it != world.getWarps().cend()) {
        WorldWarp& warp = *it;
        if (warp.posA == WorldWarp::NULL_POS) {
            warp.posA = pos;
        } else if (warp.posA != pos) {
            warp.posB = pos;
        }
    } else {
        world.getWarps().emplace_back(newWarpId, pos, WorldWarp::NULL_POS);
    }
}

void removeWarpFromTile(WorldMap& world, Vec2s pos)
{
    for (WorldWarp& warp : world.getWarps()) {
        if (warp.posA == pos)
            warp.posA = WorldWarp::NULL_POS;
        if (warp.posB == pos)
            warp.posB = WorldWarp::NULL_POS;
    }

    const auto erase_from = std::remove_if(
        world.getWarps().begin(),
        world.getWarps().end(),
        [](const WorldWarp& warp) { return warp.posA == WorldWarp::NULL_POS && warp.posB == WorldWarp::NULL_POS; });
    world.getWarps().erase(erase_from, world.getWarps().end());
}
}  // namespace


void EditorWarps::loadAssets()
{
    m_sprWarps[0].init(convertPath("gfx/leveleditor/leveleditor_warp.png"), colors::MAGENTA);
    m_sprWarps[1].init(convertPath("gfx/leveleditor/leveleditor_warp_preview.png"), colors::MAGENTA);
    m_sprWarps[2].init(convertPath("gfx/leveleditor/leveleditor_warp_thumbnail.png"), colors::MAGENTA);
}


void EditorWarps::onSetupKeypress(const SDL_KeyboardEvent& event, WorldMap& world)
{
    switch (event.keysym.sym) {
    case SDLK_ESCAPE:
        newlyEntered = false;
        return;
    }
}


void EditorWarps::onSetupMouseClick(const SDL_MouseButtonEvent& event, WorldMap& world)
{
    if (event.button != SDL_BUTTON_LEFT)
        return;

    const short tileX = event.x / TILESIZE;
    const short tileY = event.y / TILESIZE;

    if (0 <= tileX && tileX <= 9 && tileY == 0) {
        m_selectedTileId = tileX;
        newlyEntered = false;
    }
}


void EditorWarps::renderSetup(CResourceManager& rm, const WorldMap& world)
{
    m_sprWarps[0].draw(0, 0, 0, 0, 320, 32);
}


void EditorWarps::renderEdit(WorldMap& world, Vec2s offsetTile, Vec2s offsetPx, CResourceManager& rm)
{
    for (const WorldWarp& warp : world.getWarps()) {
        if (warp.posA != WorldWarp::NULL_POS) {
            const Vec2s pos = (warp.posA - offsetTile) * TILESIZE + offsetPx;
            m_sprWarps[0].draw(pos.x, pos.y, warp.id * 32, 0, 32, 32);
        }
        if (warp.posB != WorldWarp::NULL_POS) {
            const Vec2s pos = (warp.posB - offsetTile) * TILESIZE + offsetPx;
            m_sprWarps[0].draw(pos.x, pos.y, warp.id * 32, 0, 32, 32);
        }
    }
}


void EditorWarps::renderScreenshot(const WorldMap& world, short screenshotSize)
{
    constexpr std::array<short, 3> TILE_SIZES { 32, 16, 8 };
    assert(0 <= screenshotSize && screenshotSize < 3);

    const short tileSize = TILE_SIZES[screenshotSize];
    for (const WorldWarp& warp : world.getWarps()) {
        if (warp.posA != WorldWarp::NULL_POS)
            m_sprWarps[screenshotSize].draw(warp.posA.x * tileSize, warp.posA.y * tileSize, warp.id * tileSize, 0, tileSize, tileSize);
        if (warp.posB != WorldWarp::NULL_POS)
            m_sprWarps[screenshotSize].draw(warp.posB.x * tileSize, warp.posB.y * tileSize, warp.id * tileSize, 0, tileSize, tileSize);
    }
}


bool EditorWarps::onTileClicked(WorldMap& world, Vec2s pos, uint8_t button)
{
    if (button == SDL_BUTTON_LEFT) {
        addWarpToTile(world, pos, m_selectedTileId);
    } else {
        removeWarpFromTile(world, pos);
    }
    return false;
}
