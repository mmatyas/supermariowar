#include "EditorBase.h"

#include "SDL_events.h"


void EditorBase::handleSetupInput(const SDL_Event& event)
{
    switch (event.type) {
        case SDL_KEYUP:
        case SDL_KEYDOWN:
            if (!event.key.repeat)
                onSetupKeypress(event.key);
            break;

        case SDL_MOUSEBUTTONUP:
        case SDL_MOUSEBUTTONDOWN:
            onSetupMouseClick(event.button);
            break;

        case SDL_MOUSEMOTION:
            onSetupMouseMotion(event.motion);
            break;

        default:
            break;
    }
}


void EditorBase::onEnter()
{
    newlyEntered = true;
}
