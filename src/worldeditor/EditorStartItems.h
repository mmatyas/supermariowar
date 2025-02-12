#pragma once

#include "EditorBase.h"
#include "GlobalConstants.h"

#include <SDL.h>
#include <array>


class EditorStartItems : public EditorBase {
public:
    EditorStartItems();

protected:
    bool isSetupTransparent() const override { return true; }
    void renderSetup(CResourceManager& rm, const WorldMap& world) override;
    void onSetupKeypress(const SDL_KeyboardEvent& event, WorldMap& world) override;
    void onSetupMouseClick(const SDL_MouseButtonEvent& event, WorldMap& world) override;

private:
    std::array<SDL_Rect, NUM_POWERUPS + NUM_WORLD_POWERUPS> m_itemBoxes;
    std::array<SDL_Rect, 32> m_pickedBoxes;
};
