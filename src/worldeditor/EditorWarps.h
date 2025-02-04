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
    void renderSetup(CResourceManager& rm) override;
    void onSetupKeypress(const SDL_KeyboardEvent& event) override;
    void onSetupMouseClick(const SDL_MouseButtonEvent& event) override;

    void renderEdit(WorldMap& world, Vec2s offsetTile, Vec2s offsetPx) override;
    bool onTileClicked(WorldMap& world, Vec2s pos, uint8_t button) override;

private:
    std::array<gfxSprite, 3> m_sprWarps;
    short m_selectedTileId = 0;
};
