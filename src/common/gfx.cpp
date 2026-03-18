#include "gfx.h"

#include "gfx/Color.h"
#include "gfx/gfxPalette.h"
#include "gfx/gfxSDL.h"

#include "SDL_image.h"

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <format>
#include <iostream>
#include <string>

using namespace std;

namespace fs = std::filesystem;

extern SDL_Surface * blitdest;
extern SDL_Surface * screen;

extern short x_shake;
extern short y_shake;

GraphicsSDL gfx;
gfxPalette gfx_palette;


namespace {
Uint32 getRawPixel(SDL_Surface* surf, int x, int y)
{
    assert(surf);
    assert(0 <= x && x < surf->w);
    assert(0 <= y && y < surf->h);

    const Uint8 bpp = surf->format->BytesPerPixel;
    const size_t idx = y * surf->pitch + x * bpp;
    const auto* pixel8 = static_cast<Uint8*>(surf->pixels) + idx;

    switch (bpp) {
        case 1: return *pixel8;
        case 2: return *reinterpret_cast<const Uint16*>(pixel8);
        case 3: return (SDL_BYTEORDER == SDL_BIG_ENDIAN)
            ? pixel8[0] << 16 | pixel8[1] << 8 | pixel8[2]
            : pixel8[0] | pixel8[1] << 8 | pixel8[2] << 16;
        case 4: return *reinterpret_cast<const Uint32*>(pixel8);
    }
    assert(false);
    return 0x0;
}

void setRawPixel(SDL_Surface* surf, int x, int y, Uint32 value)
{
    assert(surf);
    assert(0 <= x && x < surf->w);
    assert(0 <= y && y < surf->h);

    const Uint8 bpp = surf->format->BytesPerPixel;
    const size_t idx = y * surf->pitch + x * bpp;
    auto* pixel8 = static_cast<Uint8*>(surf->pixels) + idx;

    switch (bpp) {
        case 1:
            *pixel8 = value;
            break;
        case 2:
            *reinterpret_cast<Uint16*>(pixel8) = value;
            break;
        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                pixel8[0] = (value >> 16) & 0xFF;
                pixel8[1] = (value >>  8) & 0xFF;
                pixel8[2] = (value >>  0) & 0xFF;
            } else {
                pixel8[0] = (value >>  0) & 0xFF;
                pixel8[1] = (value >>  8) & 0xFF;
                pixel8[2] = (value >> 16) & 0xFF;
            }
            break;
        case 4:
            *reinterpret_cast<Uint32*>(pixel8) = value;
            break;
        default:
            assert(false);
            break;
    }
}

void setRgb(SDL_Surface* surf, int x, int y, const RGB& color)
{
    assert(surf);
    const Uint32 rawPixel = SDL_MapRGB(surf->format, color.r, color.g, color.b);
    setRawPixel(surf, x, y, rawPixel);
}

/**
 * Makes team-colored skin surface frame from a loaded sprite strip.
 * @param  source       skin sprite strip surface
 * @param  sourceFrame  frame index [0-5]
 * @param  team         team index for color [0-3]
 * @param  allStates    render all player states
 * @param  mirrored     render horizontally mirrored
 * @return              team-colored surface
 */
SdlSurfacePtr createSkinSurface(
    const SdlSurfacePtr& source,
    size_t sourceFrame,
    size_t team,
    bool allStates,
    bool mirrored)
{
    //Take the loaded skin and colorize it for each state (normal, 3 frames of invincibility, shielded, tagged, ztarred, got shine, frozen)
    const size_t outFrameCount = allStates ? PlayerPalette::COUNT : 1;

    auto out = SdlSurfacePtr(SDL_CreateRGBSurface(
        0x0,
        32 * outFrameCount,
        32,
        screen->format->BitsPerPixel,
        screen->format->Rmask,
        screen->format->Gmask,
        screen->format->Bmask,
        0x0 /* no alpha */));

    if (SDL_MUSTLOCK(out.get()))
        SDL_LockSurface(out.get());

    if (SDL_MUSTLOCK(source.get()))
        SDL_LockSurface(source.get());

    const int startX = sourceFrame * 32;

    for (int y = 0; y < 32; y++) {
        for (int srcX = 0; srcX < 32; srcX++) {
            const int dstX = mirrored ? (31 - srcX) : srcX;

            const RGB pixelColor = getRgb(source.get(), startX + srcX, y);

            const auto it = gfx_palette.colorSheets().find(pixelColor);
            if (it != gfx_palette.colorSheets().cend()) {
                const ColorSheet& sheet = it->second;
                for (size_t outFrame = 0; outFrame < outFrameCount; outFrame++) {
                    const RGB paletteColor = sheet.replacementFor(team, static_cast<PlayerPalette>(outFrame));
                    setRgb(out.get(), outFrame * 32 + dstX, y, paletteColor);
                }
            } else {
                for (size_t outFrame = 0; outFrame < outFrameCount; outFrame++) {
                    setRgb(out.get(), outFrame * 32 + dstX, y, pixelColor);
                }
            }
        }
    }

    SDL_UnlockSurface(source.get());
    SDL_UnlockSurface(out.get());

    const auto color_key = SDL_MapRGB(out->format, colors::MAGENTA.r, colors::MAGENTA.g, colors::MAGENTA.b);
    if (SDL_SetColorKey(out.get(), SDL_TRUE, color_key) < 0) {
        throw std::format("Couldn't set color key for new skin surface: {}", SDL_GetError());
    }
    if (SDL_SetSurfaceRLE(out.get(), 1) < 0) {
        throw std::format("Couldn't set RLE acceleration for new skin surface: {}", SDL_GetError());
    }

    return out;
}

bool validSkinSurface(const SdlSurfacePtr& skin)
{
    return skin->w == 192 && skin->h == 32;
}
} // namespace


RGB getRgb(SDL_Surface* surf, int x, int y)
{
    assert(surf);
    const Uint32 rawPixel = getRawPixel(surf, x, y);
    RGB color;
    SDL_GetRGB(rawPixel, surf->format, &color.r, &color.g, &color.b);
    return color;
}


bool gfx_init(int w, int h, bool fullscreen) {
    return gfx.init(fullscreen);
}

void gfx_changefullscreen(bool fullscreen) {
    gfx.changeFullScreen(fullscreen);
}

void gfx_flipscreen() {
    gfx.flipScreen();
}

void gfx_settitle(const char* title) {
    gfx.setTitle(title);
}

void gfx_show_error(const char* message) {
    gfx.showErrorBox(message);
}

void gfx_take_screenshot() {
    gfx.takeScreenshot();
}

void gfx_close() {}
bool gfx_loadpalette(const std::filesystem::path& palette_path) {
    return gfx_palette.load(palette_path);
}

SpriteStrip gfx_loadmenuskin(const fs::path& path, short colorScheme, bool fLoadBothDirections)
{
    auto skin = SdlSurfacePtr(IMG_Load(path.string().c_str()));
    if (!skin)
        throw std::format("Couldn't load {}: {}", path.string(), IMG_GetError());

    if (!validSkinSurface(skin))
        throw std::format("Invalid skin file: {} has incorrect dimensions", path.string());

    SpriteStrip strip;

    for (size_t i = 0; i < 2; i++) {
        SdlSurfacePtr skinSurface = createSkinSurface(skin, i, colorScheme, true, false);
        strip[i * 2] = gfxSprite(std::move(skinSurface));
    }

    if (fLoadBothDirections) {
        for (size_t i = 0; i < 2; i++) {
            SdlSurfacePtr skinSurface = createSkinSurface(skin, i, colorScheme, true, true);
            strip[i * 2 + 1] = gfxSprite(std::move(skinSurface));
        }
    }

    return strip;
}

SpriteStrip gfx_loadfullskin(const fs::path& path, short colorScheme)
{
    auto skin = SdlSurfacePtr(IMG_Load(path.string().c_str()));
    if (!skin)
        throw std::format("Couldn't load {}: {}", path.string(), IMG_GetError());

    if (!validSkinSurface(skin))
        throw std::format("Invalid skin file: {} has incorrect dimensions", path.string());

    SpriteStrip strip;

    for (size_t k = 0; k < 4; k++) {
        for (size_t j = 0; j < 2; j++) {
            SdlSurfacePtr skinSurface = createSkinSurface(skin, k, colorScheme, true, j != 0);
            strip[(k * 2) + j] = gfxSprite(std::move(skinSurface));
        }
    }

    //Dead Flying Sprite
    SdlSurfacePtr skinSurface = createSkinSurface(skin, 4, colorScheme, true, false);
    strip[8] = gfxSprite(std::move(skinSurface));

    //Dead Stomped Sprite
    skinSurface = createSkinSurface(skin, 5, colorScheme, true, false);
    strip[9] = gfxSprite(std::move(skinSurface));

    return strip;
}

void gfx_cliprect(SDL_Rect * srcRect, SDL_Rect * dstRect, short x, short y, short w, short h)
{
    if (dstRect->x >= x + w || dstRect->x + dstRect->w < x || dstRect->y >= y + h || dstRect->y + dstRect->h < y) {
        srcRect->w = 0;
        srcRect->h = 0;
        return;
    }

    if (dstRect->x < x) {
        short iDiffX = x - dstRect->x;
        srcRect->x += iDiffX;
        srcRect->w -= iDiffX;
        dstRect->x = x;
        //dstRect->w -= iDiffX;
    }

    if (dstRect->x + dstRect->w >= x + w) {
        short iDiffX = dstRect->x + dstRect->w - x - w;
        srcRect->w -= iDiffX;
        //dstRect->w -= iDiffX;
    }

    if (dstRect->y < y) {
        short iDiffY = y - dstRect->y;
        srcRect->y += iDiffY;
        srcRect->h -= iDiffY;
        dstRect->y = y;
        //dstRect->h -= iDiffY;
    }

    if (dstRect->y + dstRect->h >= y + h) {
        short iDiffY = dstRect->y + dstRect->h - y - h;
        srcRect->h -= iDiffY;
        //dstRect->h -= iDiffY;
    }
}

bool gfx_adjusthiddenrects(SDL_Rect& src, SDL_Rect& dst, ClipEdge edge, int threshold)
{
    switch (edge) {
        case ClipEdge::Top: {
            if (threshold < dst.y) // fully visible
                break;
            if (dst.y + dst.h <= threshold) // fully hidden
                return true;

            const int diff = threshold - dst.y;
            src.y += diff;
            src.h -= diff;
            dst.y = threshold + y_shake;
            dst.h = src.h;
            break;
        }
        case ClipEdge::Right: {
            if (dst.x + src.w < threshold) // fully visible
                break;
            if (threshold <= dst.x) // fully hidden
                return true;

            src.w = threshold - dst.x;
            dst.w = src.w;
            break;
        }
        case ClipEdge::Bottom: {
            if (dst.y + src.h < threshold) // fully visible
                break;
            if (threshold <= dst.y) // fully hidden
                return true;

            src.h = threshold - dst.y;
            dst.h = src.h;
            break;
        }
        case ClipEdge::Left: {
            if (threshold < dst.x) // fully visible
                break;
            if (dst.x + src.w <= threshold) // fully hidden
                return true;

            int diff = threshold - dst.x;
            src.x += diff;
            src.w -= diff;
            dst.x = threshold + x_shake;
            dst.w = src.w;
            break;
        }
    }

    return false;
}

void gfx_drawpreview(
    SDL_Surface * surface,
    short dstX, short dstY,
    short srcX, short srcY,
    short iw, short ih,
    short clipX, short clipY, short clipW, short clipH,
    bool wrap,
    std::optional<std::pair<ClipEdge, int>> clip)
{
    //need to set source rect before each blit so it can be clipped correctly
    SDL_Rect rSrcRect = {srcX, srcY, iw, ih};
    SDL_Rect rDstRect = {dstX, dstY, iw, ih};

    gfx_cliprect(&rSrcRect, &rDstRect, clipX, clipY, clipW, clipH);

    if (clip) {
        auto [edge, threshold] = *clip;
        if (gfx_adjusthiddenrects(rSrcRect, rDstRect, edge, threshold))
            return;
    }

    // Blit onto the screen surface
    if (SDL_BlitSurface(surface, &rSrcRect, blitdest, &rDstRect) < 0) {
        fprintf(stderr, "SDL_BlitSurface error: %s\n", SDL_GetError());
        return;
    }

    if (wrap) {
        //Deal with wrapping over sides of screen
        bool fBlitSide = false;
        if (dstX < clipX) {
            rDstRect.x = dstX + 320; // TODO: Get it from a global setting.
            fBlitSide = true;
        } else if (dstX + iw >= clipX + clipW) {
            rDstRect.x = dstX - 320; // TODO: Get it from a global setting.
            fBlitSide = true;
        }

        if (fBlitSide) {
            //need to set source rect before each blit so it can be clipped correctly
            rSrcRect.x = srcX;
            rSrcRect.y = srcY;
            rSrcRect.w = iw;
            rSrcRect.h = ih;

            rDstRect.y = dstY;
            rDstRect.w = iw;
            rDstRect.h = ih;

            gfx_cliprect(&rSrcRect, &rDstRect, clipX, clipY, clipW, clipH);

            if (clip) {
                auto [edge, threshold] = *clip;
                if (gfx_adjusthiddenrects(rSrcRect, rDstRect, edge, threshold))
                    return;
            }

            if (SDL_BlitSurface(surface, &rSrcRect, blitdest, &rDstRect) < 0) {
                fprintf(stderr, "SDL_BlitSurface error: %s\n", SDL_GetError());
                return;
            }
        }
    }
}


void gfx_setjoystickteamcolor(SDL_Joystick* joystick, short team, float brightness)
{
#if SDL_VERSION_ATLEAST(2, 0, 14)
    if (team < 0) {
        // A non-playing user has no team
        return;
    }
    brightness = max(0.f, min(1.f, brightness));
    if (std::optional<RGB> color = gfx_palette.replacementFor(RGB {0x80, 0x00, 0x00}, team, PlayerPalette::normal)) {
        SDL_JoystickSetLED(joystick, (Uint8)(brightness * color->r), (Uint8)(brightness * color->g), (Uint8)(brightness * color->b));
    }
#endif
}
