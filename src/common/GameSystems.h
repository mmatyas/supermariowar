#pragma once

#include "gfx/gfxSDL.h"
#include "sfx.h"


class Sdl {
    friend struct Systems;
    friend struct MinimalSystems;

    Sdl(unsigned int flags);
    ~Sdl();
};


struct Systems {
    Systems(bool fullscreen)
        : sdl(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK)
        , gfx(fullscreen)
    {}

    Systems(const Systems&) = delete;
    Systems& operator=(const Systems&) = delete;

    Sdl sdl;
    Graphics gfx;
    Audio sfx;
};


struct MinimalSystems {
    MinimalSystems(bool fullscreen)
        : sdl(SDL_INIT_VIDEO)
        , gfx(fullscreen)
    {}

    MinimalSystems(const MinimalSystems&) = delete;
    MinimalSystems& operator=(const MinimalSystems&) = delete;

    Sdl sdl;
    Graphics gfx;
};
