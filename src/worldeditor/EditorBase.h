#pragma once

#include <cstdint>
#include <functional>
#include <unordered_map>

class CResourceManager;
class WorldMapTile;
union SDL_Event;


class InputHandler {
public:
    using Keycode = int32_t;
    using MouseButton = uint8_t;

    using KeyboardCall = std::function<void(bool /* pressed */)>;
    using MouseClickCall = std::function<void(int /*x */, int /* y */, bool /* pressed */)>;
    using MouseMotionCall = std::function<void(int /*x */, int /* y */, uint32_t /* button state */)>;

    void setKeyCb(Keycode key, const KeyboardCall& cb);
    void setMouseClickCb(MouseButton key, const MouseClickCall& cb);
    void setMouseMotionCb(const MouseMotionCall& cb);

    void handleEvent(const SDL_Event& ev);

private:
    std::unordered_map<Keycode, KeyboardCall> m_keyboardCalls;
    std::unordered_map<MouseButton, MouseClickCall> m_mouseClickCalls;
    MouseMotionCall m_mouseMotionCall = [](int, int, uint32_t){};
};


class EditorBase {
public:
    virtual ~EditorBase() = default;

    virtual void onEnter();
    virtual void onTileClicked(WorldMapTile& tile, uint8_t button, bool& changed) = 0;
    virtual void render(CResourceManager& rm) = 0;

    bool isReady() const { return !newlyEntered; }  /// Ready for editing the world map.

    InputHandler input;

protected:
    bool newlyEntered = true;
};
