#include "EditorVehicleBoundaries.h"

#include "ResourceManager.h"
#include "world.h"

extern SDL_Surface* blitdest;


void EditorVehicleBoundaries::onSetupKeypress(const SDL_KeyboardEvent& event, WorldMap& world)
{
    switch (event.keysym.sym) {
    case SDLK_ESCAPE:
        newlyEntered = false;
        return;
    }
}


void EditorVehicleBoundaries::onSetupMouseClick(const SDL_MouseButtonEvent& event, WorldMap& world)
{
    if (event.button != SDL_BUTTON_LEFT)
        return;

    const short tileX = event.x / TILESIZE;
    const short tileY = event.y / TILESIZE;

    if (0 <= tileX && tileX <= 9 && 0 <= tileY && tileY <= 9) {
        m_selectedTileId = 1 + 10 * tileY + tileX;
        newlyEntered = false;
    }
}


void EditorVehicleBoundaries::renderSetup(CResourceManager& rm, const WorldMap& world)
{
    const int magenta = SDL_MapRGB(blitdest->format, 255, 0, 255);
    SDL_Rect r = { 0, 0, 320, 320 };
    SDL_FillRect(blitdest, &r, magenta);

    rm.spr_worldforegroundspecial[0].draw(0, 0, 0, 0, 320, 320);
}


void EditorVehicleBoundaries::renderEdit(WorldMap& world, Vec2s offsetTile, Vec2s offsetPx, CResourceManager& rm)
{
    const int magenta = SDL_MapRGB(blitdest->format, 255, 0, 255);

    for (short row = offsetTile.y; row < offsetTile.y + 15 && row < world.h(); row++) {
        for (short col = offsetTile.x; col <= offsetTile.x + 20 && col < world.w(); col++) {
            const short boundary = world.getTiles().at(col, row).iVehicleBoundary - 1;
            if (boundary < 0)
                continue;

            short ix = (col - offsetTile.x) * TILESIZE + offsetPx.x;
            short iy = (row - offsetTile.y) * TILESIZE + offsetPx.y;
            SDL_Rect r = { ix, iy, 32, 32 };
            SDL_FillRect(blitdest, &r, magenta);

            rm.spr_worldforegroundspecial[0].draw(ix, iy, (boundary % 10) * 32, (boundary / 10) * 32, 32, 32);
        }
    }
}


bool EditorVehicleBoundaries::onTileClicked(WorldMap& world, Vec2s pos, uint8_t button)
{
    const short newTileId = (button == SDL_BUTTON_LEFT) ? m_selectedTileId : 0;
    WorldMapTile& tile = world.getTiles().at(pos.x, pos.y);
    tile.iVehicleBoundary = newTileId;
    return false;
}
