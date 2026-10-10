#pragma once

#include <SDL3/SDL.h>


class GraphicsSDL {
public:
    ~GraphicsSDL();

    bool init(bool fullscreen);

    void flipScreen() const;
    void changeFullScreen(bool fullcreen) const;
    void setTitle(const char*) const;
    void showErrorBox(const char*) const;
    void takeScreenshot() const;

private:
    // surface -> texture -> renderer -> window
    SDL_Window* sdl_window = nullptr;
    SDL_Renderer* sdl_renderer = nullptr;
    SDL_Surface* sdl_screen_surface = nullptr;
    SDL_Texture* sdl_screen_texture = nullptr;
};
