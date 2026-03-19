#include "GameSystems.h"

#include <format>


Sdl::Sdl(unsigned int flags)
{
    if (SDL_Init(flags) < 0)
        throw std::format("Could not initialize SDL: {}", SDL_GetError());

    SDL_version sdl_version;
    SDL_GetVersion(&sdl_version);
    printf("[gfx] SDL %d.%d.%d loaded.\n", sdl_version.major, sdl_version.minor, sdl_version.patch);
}

Sdl::~Sdl()
{
    SDL_Quit();
}
