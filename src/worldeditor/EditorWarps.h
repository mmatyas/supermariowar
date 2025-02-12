#pragma once

#include "EditorBase.h"

#include "gfx/gfxSprite.h"

#include <array>


class EditorWarps : public EditorBase {
public:
    void renderScreenshot(const WorldMap& world, short screenshotSize);

protected:
    void loadAssets() override;

    bool isSetupTransparent() const override { return true; }
    void renderSetup(CResourceManager& rm, const WorldMap& world) override;
    void onSetupKeypress(const SDL_KeyboardEvent& event, WorldMap& world) override;
    void onSetupMouseClick(const SDL_MouseButtonEvent& event, WorldMap& world) override;

    void renderEdit(WorldMap& world, Vec2s offsetTile, Vec2s offsetPx, CResourceManager& rm) override;
    bool onTileClicked(WorldMap& world, Vec2s pos, uint8_t button) override;

private:
    std::array<gfxSprite, 3> m_sprWarps;
    short m_selectedTileId = 0;
};
