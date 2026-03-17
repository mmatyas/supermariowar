#pragma once

#include "SDL.h"

#include <cassert>


class Graphics {
public:
    static Graphics& get() {
        assert(s_instance);
        return *s_instance;
    }

    void flipScreen() const;
    void changeFullScreen(bool fullcreen) const;
    void setTitle(const char*) const;
    void showErrorBox(const char*) const;
    void takeScreenshot() const;

private:
    friend struct Systems;
    Graphics(bool fullscreen);
    ~Graphics();
    inline static Graphics* s_instance = nullptr;

    // surface -> texture -> renderer -> window
    SDL_Window* sdl_window = nullptr;
    SDL_Renderer* sdl_renderer = nullptr;
    SDL_Surface* sdl_screen_surface = nullptr;
    SDL_Texture* sdl_screen_texture = nullptr;
};
