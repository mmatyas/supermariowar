#pragma once

#include "EditorBase.h"


class EditorTileType : public EditorBase {
protected:
    bool isSetupTransparent() const override { return true; }

    bool onTileClicked(WorldMap& world, Vec2s pos, uint8_t button) override;
    void renderSetup(CResourceManager& rm, const WorldMap& world) override;

    void onSetupKeypress(const SDL_KeyboardEvent& event, WorldMap& world) override;
    void onSetupMouseClick(const SDL_MouseButtonEvent& event, WorldMap& world) override;

private:
    short m_selectedTileId = 0;
};
