#pragma once

#include "EditorBase.h"


class EditorBridges : public EditorBase {
protected:
    void renderSetup(CResourceManager& rm, const WorldMap& world) override;
    void onSetupKeypress(const SDL_KeyboardEvent& event, WorldMap& world) override;
    void onSetupMouseClick(const SDL_MouseButtonEvent& event, WorldMap& world) override;
    bool onTileClicked(WorldMap& world, Vec2s pos, uint8_t button) override;

private:
    short m_selectedTileId = 0;
};
