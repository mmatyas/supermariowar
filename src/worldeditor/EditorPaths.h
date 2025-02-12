#pragma once

#include "EditorBase.h"

#include "gfx/gfxSprite.h"


class EditorPaths : public EditorBase {
public:
    void setAutoPaint(bool value) {
        m_autopaint = value;
    }

protected:
    void loadAssets() override;

    void onSetupKeypress(const SDL_KeyboardEvent& event, WorldMap& world) override;
    void onSetupMouseClick(const SDL_MouseButtonEvent& event, WorldMap& world) override;
    void renderSetup(CResourceManager& rm, const WorldMap& world) override;

    bool onTileClicked(WorldMap& world, Vec2s pos, uint8_t button) override;
    void renderEdit(WorldMap& world, Vec2s offsetTile, Vec2s offsetPx, CResourceManager& rm) override;

private:
    gfxSprite m_sprPath;
    short m_selectedTileId = 0;
    bool m_autopaint = false;
};
