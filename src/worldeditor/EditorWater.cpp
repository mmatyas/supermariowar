#include "EditorWater.h"

#include "ResourceManager.h"
#include "world.h"


namespace {
constexpr int TILE_COUNT = 3;
}  // namespace


EditorWater::EditorWater()
    : EditorBase()
{
    input.setMouseClickCb(SDL_BUTTON_LEFT, [this](int x, int y, uint32_t state){
        onMouseClick(x, y);
    });
    input.setKeyCb(SDLK_ESCAPE, [this](bool pressed){
        newlyEntered = false;
    });
}


void EditorWater::onMouseClick(int x, int y)
{
    if (newlyEntered) {
        const int tileX = x / TILESIZE;
        const int tileY = y / TILESIZE;

        if (tileY == 0 && 0 <= tileX && tileX < TILE_COUNT) {
            selectedTileId = tileX + 4;
        }

        newlyEntered = false;
        return;
    }
}


void EditorWater::render(CResourceManager& rm)
{
    if (newlyEntered) {
        for (int idx = 0; idx < TILE_COUNT; idx++)
            rm.spr_worldbackground[0].draw(idx * 32, 0, 512 + (idx * 128), 0, 32, 32);
    }
}


void EditorWater::onTileClicked(WorldMapTile& tile, uint8_t button, bool& changed)
{
    const short newTileId = (button == SDL_BUTTON_LEFT) ? selectedTileId : 0;
    changed = tile.iBackgroundWater != newTileId;
    tile.iBackgroundWater = newTileId;
}
