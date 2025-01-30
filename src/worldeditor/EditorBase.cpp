#include "EditorBase.h"

#include "SDL_events.h"


void InputHandler::handleEvent(const SDL_Event& ev)
{
    switch (ev.type) {
        case SDL_KEYUP:
        case SDL_KEYDOWN: {
            if (ev.key.repeat)
                break;

            const SDL_Keycode key = ev.key.keysym.sym;
            const bool isDown = ev.key.state == SDL_PRESSED;

            const auto it = m_keyboardCalls.find(key);
            if (it != m_keyboardCalls.cend())
                it->second(isDown);

            break;
        }

        case SDL_MOUSEBUTTONUP:
        case SDL_MOUSEBUTTONDOWN: {
            const uint8_t button = ev.button.button;
            const bool isDown = ev.key.state == SDL_PRESSED;

            const auto it = m_mouseClickCalls.find(button);
            if (it != m_mouseClickCalls.cend())
                it->second(ev.button.x, ev.button.y, isDown);

            break;
        }

        case SDL_MOUSEMOTION:
            m_mouseMotionCall(ev.motion.x, ev.motion.y, ev.motion.state);
            break;

        default:
            break;
    }
}

void InputHandler::setKeyCb(Keycode key, const KeyboardCall& cb) {
    m_keyboardCalls[key] = cb;
}
void InputHandler::setMouseClickCb(MouseButton key, const MouseClickCall& cb) {
    m_mouseClickCalls[key] = cb;
}
void InputHandler::setMouseMotionCb(const MouseMotionCall& cb) {
    m_mouseMotionCall = cb;
}


void EditorBase::onEnter()
{
    newlyEntered = true;
}
