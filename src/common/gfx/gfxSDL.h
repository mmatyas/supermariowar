#ifndef GFX_SYS_SDL
#define GFX_SYS_SDL

#include "SDL.h"

#include "gfxPalette.h"

class GraphicsSDL
{
public:
    GraphicsSDL();
    ~GraphicsSDL();

    void FlipScreen();
    void RecreateWindow(bool fullscreen);
    void setTitle(const char*);

    bool Init(bool fullscreen);
    void Close();

    gfxPalette& getPalette() { return palette; }

private:
    void init_sdl();
    void init_sdl_img();
    void create_game_window(bool fullscreen);

    void print_sdl_version();
#ifndef __EMSCRIPTEN__
    void print_sdl_img_version();
#endif

#ifdef USE_SDL2
    void create_renderer();
    void create_screen_surface();
    void create_screen_tex();

    // screen -> texture -> renderer -> window
    SDL_Window*     sdl2_window;
    SDL_Renderer*   sdl2_renderer;
    SDL_Texture*    sdl2_screen_as_texture;
#endif

    gfxPalette palette;
};

#endif // GFX_SYS_SDL
