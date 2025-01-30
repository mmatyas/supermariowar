#pragma once

#include "EditorBase.h"


class EditorWater : public EditorBase {
public:
    EditorWater();

    void onTileClicked(WorldMapTile& tile, uint8_t button, bool& changed) override;
    void render(CResourceManager& rm) override;

private:
    short selectedTileId = 4;

    void onMouseClick(int x, int y);
};
