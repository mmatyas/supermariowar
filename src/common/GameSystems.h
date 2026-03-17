#pragma once

#include "gfx/gfxSDL.h"


struct Systems {
    Systems(bool fullscreen)
        : gfx(fullscreen)
    {}

    Graphics gfx;
};
