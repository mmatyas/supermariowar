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
    void ChangeFullScreen(bool fullcreen);
    void setTitle(const char*);

    bool Init(bool fullscreen);
    void Close();

    gfxPalette& getPalette() { return palette; }

private:
    void init_sdl();
    void init_sdl_img();
    void create_game_window(bool fullscreen);

    void print_sdl_version();
    void print_sdl_img_version();

#ifdef USE_SDL2
    void create_renderer();
    void create_screen_surface();
    void create_screen_tex();
    void print_renderer_info();
#ifdef SDL2_FORCE_GLES
    int find_gles_driver_index();
#endif

    // screen -> texture -> renderer -> window
    SDL_Window*     sdl2_window;
    SDL_Renderer*   sdl2_renderer;
    SDL_Texture*    sdl2_screen_as_texture;
#endif

    gfxPalette palette;
};

#endif // GFX_SYS_SDL
