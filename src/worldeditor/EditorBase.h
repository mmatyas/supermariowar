#pragma once

#include "math/Vec2.h"

#include <cstdint>

class CResourceManager;
class WorldMap;
union SDL_Event;
struct SDL_KeyboardEvent;
struct SDL_MouseButtonEvent;
struct SDL_MouseMotionEvent;


class EditorSetupScreen {
public:
    virtual ~EditorSetupScreen() = default;
};


class EditorBase {
public:
    virtual ~EditorBase() = default;

    /// Input event during the setup stage of the editor mode.
    void handleSetupInput(const SDL_Event& ev);

    /// The editor is ready for editing the world map.
    bool isReady() const { return !newlyEntered; }

    /// If transparent, will show the map behind the setup screen,
    /// otherwise it is filled with black.
    virtual bool isSetupTransparent() const { return false; }

    virtual void loadAssets() {}
    virtual void onEnter();
    virtual bool onTileClicked(WorldMap& world, Vec2s pos, uint8_t button) { return false; }

    /// Render the setup screen of the editor mode.
    virtual void renderSetup(CResourceManager& rm) = 0;

    /// Render during map editing
    virtual void renderEdit(WorldMap& world, Vec2s offsetTile, Vec2s offsetPx, CResourceManager& rm) {}

protected:
    bool newlyEntered = true;

    /// Key press event during the setup stage of the editor mode.
    virtual void onSetupKeypress(const SDL_KeyboardEvent& event) {}
    /// Mouse click event during the setup stage of the editor mode.
    virtual void onSetupMouseClick(const SDL_MouseButtonEvent& event) {}
    /// Mouse motion event during the setup stage of the editor mode.
    virtual void onSetupMouseMotion(const SDL_MouseMotionEvent& event) {}
};
