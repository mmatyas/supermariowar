#pragma once

#include "gfx/gfxSDL.h"


struct Sdl {
    Sdl();
    ~Sdl();
};


struct Systems {
    Systems(bool fullscreen)
        : gfx(fullscreen)
    {}

    Systems(const Systems&) = delete;
    Systems& operator=(const Systems&) = delete;

    Sdl sdl;
    Graphics gfx;
};
