#include "gfxSDL.h"

#include "path.h"

#include "SDL_image.h"

#include <chrono>
#include <iomanip>
#include <sstream>
#include <cstdio>
#include <cstdlib>

extern SDL_Surface* screen;

#define GFX_BPP 16
#define GFX_SCREEN_W 640
#define GFX_SCREEN_H 480

template<typename... Args>
[[noreturn]] void throw_error(std::format_string<Args...> fmt, Args&&... args) {
    throw std::runtime_error(std::format(fmt, std::forward<Args>(args)...));
}

namespace {
void initSdl()
{
    constexpr int IMG_FLAGS = IMG_INIT_PNG;
    if ((IMG_Init(IMG_FLAGS) & IMG_FLAGS) != IMG_FLAGS)
        throw_error("SDL_image error: {}", IMG_GetError());

    const SDL_version* img_version = IMG_Linked_Version();
    printf("[gfx] SDL_image %d.%d.%d loaded.\n", img_version->major, img_version->minor, img_version->patch);
}

void quitSdl()
{
    IMG_Quit();
}

SDL_Window* createWindow(bool fullscreen)
{
    Uint32 window_flags = SDL_WINDOW_RESIZABLE;
    if (fullscreen)
        window_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;

    SDL_Window* window = SDL_CreateWindow("smw",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        GFX_SCREEN_W, GFX_SCREEN_H,
        window_flags);
    if (!window)
        throw_error("Couldn't create window: {}", SDL_GetError());

    return window;
}

int findPreferredRendererIndex() {
#ifdef SDL2_FORCE_GLES
    const int render_driver_count = SDL_GetNumRenderDrivers();
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
#endif
    return -1;
}

SDL_Renderer* createRenderer(SDL_Window* window)
{
    constexpr Uint32 rendering_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE;

    SDL_Renderer* renderer = SDL_CreateRenderer(window, findPreferredRendererIndex(), rendering_flags);
    if (!renderer)
        throw_error("Couldn't create renderer: {}", SDL_GetError());

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
    SDL_RenderSetLogicalSize(renderer, GFX_SCREEN_W, GFX_SCREEN_H);

    SDL_RendererInfo renderer_info;
    SDL_GetRendererInfo(renderer, &renderer_info);
    printf("[gfx] Renderer: %s, %s\n",
        renderer_info.name,
        (renderer_info.flags & SDL_RENDERER_ACCELERATED) ? "accelerated" : "software");

    return renderer;
}

SDL_Surface* createScreenSurface()
{
    SDL_Surface* surface = SDL_CreateRGBSurface(0x0,
        GFX_SCREEN_W, GFX_SCREEN_H, 32,
        0x00FF0000,
        0x0000FF00,
        0x000000FF,
        0xFF000000);
    if (!surface)
        throw_error("Couldn't create video buffer: {}", SDL_GetError());

    return surface;
}

SDL_Texture* createScreenTexture(SDL_Renderer* renderer)
{
    SDL_Texture* texture = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        GFX_SCREEN_W, GFX_SCREEN_H);
    if (!texture)
        throw_error("Couldn't create video texture: {}", SDL_GetError());

    return texture;
}
} // namespace


Graphics::Graphics(bool fullscreen)
{
    s_instance = this;
    initSdl();

    sdl_window = createWindow(fullscreen);
    sdl_renderer = createRenderer(sdl_window);
    sdl_screen_surface = createScreenSurface();
    sdl_screen_texture = createScreenTexture(sdl_renderer);

    printf("[gfx] Game window initialized (%dx%d, %dbpp)\n",
        sdl_screen_surface->w,
        sdl_screen_surface->h,
        sdl_screen_surface->format->BitsPerPixel);

    screen = sdl_screen_surface;
}

Graphics::~Graphics()
{
    SDL_DestroyTexture(sdl_screen_texture);
    SDL_FreeSurface(sdl_screen_surface);
    SDL_DestroyRenderer(sdl_renderer);
    SDL_DestroyWindow(sdl_window);

    quitSdl();

    s_instance = nullptr;
}

void Graphics::showErrorBox(const char* message) const
{
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", message, sdl_window);
}

void Graphics::setTitle(const char* title) const
{
    SDL_SetWindowTitle(sdl_window, title);
}

void Graphics::flipScreen() const
{
    SDL_UpdateTexture(sdl_screen_texture, nullptr, sdl_screen_surface->pixels, sdl_screen_surface->pitch);
    SDL_RenderClear(sdl_renderer);
    SDL_RenderCopy(sdl_renderer, sdl_screen_texture, nullptr, nullptr);
    SDL_RenderPresent(sdl_renderer);
}

void Graphics::changeFullScreen(bool fullscreen) const
{
    Uint32 flags = SDL_GetWindowFlags(sdl_window);
    if (fullscreen) {
        flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    } else {
        flags &= ~SDL_WINDOW_FULLSCREEN_DESKTOP;
    }

    if (SDL_SetWindowFullscreen(sdl_window, flags) < 0) {
        fprintf(stderr, "[gfx] Couldn't toggle fullscreen mode: %s\n", SDL_GetError());
        return;
    }
}

void Graphics::takeScreenshot() const
{
    using std::chrono::system_clock;

    // NOTE: %F and %T don't work on Windows
    constexpr const char* path_format = "screenshots/%Y-%m-%d_%H%M%S.png";

    const std::time_t now = system_clock::to_time_t(system_clock::now());
    std::ostringstream path_ss;
    path_ss << std::put_time(std::localtime(&now), path_format);
    const std::string path = convertPath(path_ss.str());

    if (IMG_SavePNG(sdl_screen_surface, path.c_str()) != 0) {
        fprintf(stderr, "[gfx] Couldn't write the screenshot to file: %s\n", SDL_GetError());
        return;
    }

    printf("[gfx] Screenshot saved to file: %s\n", path.c_str());
}
