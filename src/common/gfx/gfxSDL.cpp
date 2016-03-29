#include "gfxSDL.h"

#include "SDL.h"
#include "SDL_image.h"

#include <cstdio>
#include <cstdlib>

extern SDL_Surface* screen;

#define GFX_BPP 16
#define GFX_SCREEN_W 640
#define GFX_SCREEN_H 480

enum SDL_Errors {
    E_INIT_SDL,
    E_INIT_SDL_IMG,
    E_CREATE_WINDOW
};
#ifdef USE_SDL2
enum SDL2_Errors {
    E_CREATE_RENDERER,
    E_CREATE_SCREEN_SURFACE,
    E_CREATE_SCREEN_TEX,
};
#endif

GraphicsSDL::GraphicsSDL() {}
GraphicsSDL::~GraphicsSDL() {
    Close();
}

bool GraphicsSDL::Init(bool fullscreen)
{
    try {
        init_sdl();
        init_sdl_img();
        create_game_window(fullscreen);
    }
    catch (...) {
        return false;
    }

    printf("[gfx] Game window initialized (%dx%d, %dbpp)\n",
        GFX_SCREEN_W, GFX_SCREEN_H, screen->format->BitsPerPixel);

    return true;
}

void GraphicsSDL::init_sdl()
{
    // init SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("[gfx] SDL error: %s\n", SDL_GetError());
        throw E_INIT_SDL;
    }

    // Clean up on exit
    atexit(SDL_Quit);

    print_sdl_version();
}

void GraphicsSDL::init_sdl_img()
{
    // init SDL_image
    int img_flags = IMG_INIT_PNG;
    if ((IMG_Init(img_flags) & img_flags) != img_flags) {
        printf("[gfx] SDL_image error: %s\n", IMG_GetError());
        throw E_INIT_SDL_IMG;
    }

    atexit(IMG_Quit);
    print_sdl_img_version();
}

void GraphicsSDL::print_sdl_version()
{
    SDL_version ver_current;

#ifdef USE_SDL2
    SDL_GetVersion(&ver_current);
#else
    const SDL_version * constptr_ver_current = SDL_Linked_Version(); // dyn. linked version
    ver_current = *constptr_ver_current;
#endif

    printf("[gfx] SDL %d.%d.%d loaded.\n",
        ver_current.major, ver_current.minor, ver_current.patch);
}

void GraphicsSDL::print_sdl_img_version()
{
    // show SDL_image version
    SDL_version ver_compiled;
    const SDL_version * ver_img_current = IMG_Linked_Version();
    SDL_IMAGE_VERSION(&ver_compiled);
    printf("[gfx] SDL_image %d.%d.%d loaded.\n",
        ver_img_current->major, ver_img_current->minor, ver_img_current->patch);
}

#ifdef USE_SDL2
// -----------------------
//  SDL2 specific methods
// -----------------------
void GraphicsSDL::create_game_window(bool fullscreen)
{
    RecreateWindow(fullscreen);
    create_renderer();
    print_renderer_info();
    create_screen_surface();
    create_screen_tex();
}

#ifdef SDL2_FORCE_GLES
    int GraphicsSDL::find_gles_driver_index() {
        int render_driver_count = SDL_GetNumRenderDrivers();
        if (render_driver_count < 1) {
            printf("[gfx][warning] Couldn't access renderers, fallback to default: %s\n", SDL_GetError());
            return -1;
        }

        for (int i = 0; i < render_driver_count; i++) {
            SDL_RendererInfo renderer_info;
            SDL_GetRenderDriverInfo(i, &renderer_info);
            if (strncmp(renderer_info.name, "opengles", strlen("opengles")) == 0)
                return i;
        }

        printf("[gfx][warning] No GLES renderer found, fallback to default");
        return -1;
    }
#endif

    void GraphicsSDL::create_renderer()
    {
#ifdef SDL2_FORCE_GLES
        sdl2_renderer = SDL_CreateRenderer(sdl2_window, find_gles_driver_index(),
#else
        sdl2_renderer = SDL_CreateRenderer(sdl2_window, -1,
#endif
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
        if (!sdl2_renderer) {
            fprintf(stderr, "[gfx] Couldn't create renderer: %s\n", SDL_GetError());
            throw E_CREATE_RENDERER;
        }

        SDL_SetRenderDrawColor(sdl2_renderer, 0, 0, 0, 255);
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
        SDL_RenderSetLogicalSize(sdl2_renderer, GFX_SCREEN_W, GFX_SCREEN_H);
    }

    void GraphicsSDL::create_screen_surface()
    {
        screen = SDL_CreateRGBSurface(0, GFX_SCREEN_W, GFX_SCREEN_H, 32,
            0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
        if (!screen) {
            fprintf(stderr, "[gfx] Couldn't create video buffer: %s\n", SDL_GetError());
            throw E_CREATE_SCREEN_SURFACE;
        }
    }

    void GraphicsSDL::create_screen_tex()
    {
        sdl2_screen_as_texture = SDL_CreateTexture(sdl2_renderer,
            SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, GFX_SCREEN_W, GFX_SCREEN_H);
        if (!sdl2_screen_as_texture) {
            fprintf(stderr, "[gfx] Couldn't create video texture: %s\n", SDL_GetError());
            throw E_CREATE_SCREEN_TEX;
        }
    }

    void GraphicsSDL::print_renderer_info()
    {
        SDL_RendererInfo renderer_info;
        SDL_GetRendererInfo(sdl2_renderer, &renderer_info);
        printf("[gfx] Renderer: %s, %s\n",
            renderer_info.name,
            renderer_info.flags & SDL_RENDERER_ACCELERATED ? "accelerated" : "software");
    }

void GraphicsSDL::setTitle(const char* title)
{
    SDL_SetWindowTitle(sdl2_window, title);
    //SDL_SetRelativeMouseMode(SDL_TRUE);
}

void GraphicsSDL::FlipScreen()
{
    SDL_UpdateTexture(sdl2_screen_as_texture, NULL, screen->pixels, screen->pitch);
    SDL_RenderClear(sdl2_renderer);
    SDL_RenderCopy(sdl2_renderer, sdl2_screen_as_texture, NULL, NULL);
    SDL_RenderPresent(sdl2_renderer);
}

void GraphicsSDL::RecreateWindow(bool fullscreen)
{
    if (sdl2_window)
        SDL_DestroyWindow(sdl2_window);

    sdl2_window = SDL_CreateWindow("smw",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        GFX_SCREEN_W, GFX_SCREEN_H,
        fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);

    if (!sdl2_window) {
        fprintf(stderr, "[gfx] Couldn't create window: %s\n", SDL_GetError());
        throw E_CREATE_WINDOW;
    }

    // on some systems there's a mouse input bug after re-creating the window
    /*if (SDL_GetRelativeMouseMode()) {
        SDL_SetRelativeMouseMode(SDL_FALSE);
        SDL_SetRelativeMouseMode(SDL_TRUE);
    }*/
}

void GraphicsSDL::ChangeFullScreen(bool fullscreen)
{
    Uint32 flags = SDL_GetWindowFlags(sdl2_window);
    if (fullscreen)
        flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    else
        flags &= ~SDL_WINDOW_FULLSCREEN_DESKTOP;

    if (SDL_SetWindowFullscreen(sdl2_window, flags) < 0) {
        fprintf(stderr, "[gfx] Couldn't toggle fullscreen mode: %s\n", SDL_GetError());
        return;
    }
    //SDL_SetWindowSize(sdl2_window, GFX_SCREEN_W, GFX_SCREEN_H);
}

void GraphicsSDL::Close()
{
    SDL_DestroyTexture(sdl2_screen_as_texture);
    SDL_FreeSurface(screen);
    SDL_DestroyRenderer(sdl2_renderer);
    SDL_DestroyWindow(sdl2_window);
}

#else
// ----------------------------
//  SDL 1.2.x specific methods
// ----------------------------
void GraphicsSDL::create_game_window(bool fullscreen)
{
    RecreateWindow(fullscreen);
}

void GraphicsSDL::setTitle(const char* title)
{
    SDL_WM_SetCaption(title, "smw.ico");
    SDL_ShowCursor(SDL_DISABLE);
}

void GraphicsSDL::FlipScreen()
{
    SDL_Flip(screen);
}

void GraphicsSDL::RecreateWindow(bool fullscreen)
{
    Uint32 flags = SDL_SWSURFACE | SDL_HWACCEL;
    if (fullscreen)
        flags |= SDL_FULLSCREEN;

#ifdef _XBOX
    if (game_values.aspectratio10x11)
        flags |= SDL_10X11PIXELASPECTRATIO;

    screen = SDL_SetVideoModeWithFlickerFilter(GFX_SCREEN_W, GFX_SCREEN_H, GFX_BPP,
        flags, game_values.flickerfilter, game_values.softfilter);

#else
    screen = SDL_SetVideoMode(GFX_SCREEN_W, GFX_SCREEN_H, GFX_BPP, flags);
#endif

    if (!screen) {
        printf("[gfx] Couldn't create window: %s\n", SDL_GetError());
        throw E_CREATE_WINDOW;
    }
}

void GraphicsSDL::ChangeFullScreen(bool fullscreen)
{
    RecreateWindow(fullscreen);
}

void GraphicsSDL::Close()
{
}

#endif
