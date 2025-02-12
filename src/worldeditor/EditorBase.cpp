#include "EditorBase.h"

#include "SDL_events.h"


void EditorBase::handleSetupInput(const SDL_Event& event, WorldMap& world)
{
    switch (event.type) {
        case SDL_KEYUP:
        case SDL_KEYDOWN:
            if (!event.key.repeat)
                onSetupKeypress(event.key, world);
            break;

        case SDL_MOUSEBUTTONUP:
        case SDL_MOUSEBUTTONDOWN:
            onSetupMouseClick(event.button, world);
            break;

        case SDL_MOUSEMOTION:
            onSetupMouseMotion(event.motion, world);
            break;

        default:
            break;
    }
}


void EditorBase::onEnter()
{
    newlyEntered = true;
}
