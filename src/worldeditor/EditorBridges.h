#pragma once

#include "EditorBase.h"


class EditorBridges : public EditorBase {
protected:
    void renderSetup(CResourceManager& rm) override;
    void onSetupKeypress(const SDL_KeyboardEvent& event) override;
    void onSetupMouseClick(const SDL_MouseButtonEvent& event) override;
    bool onTileClicked(WorldMap& world, Vec2s pos, uint8_t button) override;

private:
    short m_selectedTileId = 0;
};
