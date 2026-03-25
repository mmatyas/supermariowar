#pragma once

#include <SDL3/SDL.h>

#include <memory>


struct SdlDeleter {
    void operator()(SDL_Window* ptr) const noexcept;
    void operator()(SDL_Renderer* ptr) const noexcept;
    void operator()(SDL_Texture* ptr) const noexcept;
    void operator()(SDL_Surface* ptr) const noexcept;
};

using SdlWindowPtr = std::unique_ptr<SDL_Window, SdlDeleter>;
using SdlRendererPtr = std::unique_ptr<SDL_Renderer, SdlDeleter>;
using SdlTexturePtr = std::unique_ptr<SDL_Texture, SdlDeleter>;
using SdlSurfacePtr = std::unique_ptr<SDL_Surface, SdlDeleter>;
