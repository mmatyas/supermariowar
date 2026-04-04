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
gfxSprite createSkinSurface(
    const gfxSprite& source,
    size_t sourceFrame,
    size_t team,
    bool allStates,
    bool mirrored)
{
    //Take the loaded skin and colorize it for each state (normal, 3 frames of invincibility, shielded, tagged, ztarred, got shine, frozen)
    const size_t outFrameCount = allStates ? PlayerPalette::COUNT : 1;

    auto out = gfxSprite::blank(32 * outFrameCount, 32);

    if (SDL_MUSTLOCK(out.getSurface()))
        SDL_LockSurface(out.getSurface());

    if (SDL_MUSTLOCK(source.getSurface()))
        SDL_LockSurface(source.getSurface());

    const int startX = sourceFrame * 32;

    for (int y = 0; y < 32; y++) {
        for (int srcX = 0; srcX < 32; srcX++) {
            const int dstX = mirrored ? (31 - srcX) : srcX;

            const RGB pixelColor = getRgb(source.getSurface(), startX + srcX, y);

            const auto it = gfx_palette.colorSheets().find(pixelColor);
            if (it != gfx_palette.colorSheets().cend()) {
                const ColorSheet& sheet = it->second;
                for (size_t outFrame = 0; outFrame < outFrameCount; outFrame++) {
                    const RGB paletteColor = sheet.replacementFor(team, static_cast<PlayerPalette>(outFrame));
                    setRgb(out.getSurface(), outFrame * 32 + dstX, y, paletteColor);
                }
            } else {
                for (size_t outFrame = 0; outFrame < outFrameCount; outFrame++) {
                    setRgb(out.getSurface(), outFrame * 32 + dstX, y, pixelColor);
                }
            }
        }
    }

    SDL_UnlockSurface(source.getSurface());
    SDL_UnlockSurface(out.getSurface());

    const auto color_key = SDL_MapRGB(out.getSurface()->format, colors::MAGENTA.r, colors::MAGENTA.g, colors::MAGENTA.b);
    if (SDL_SetColorKey(out.getSurface(), SDL_TRUE, color_key) < 0) {
        throw std::format("Couldn't set color key for new skin surface: {}", SDL_GetError());
    }
    if (SDL_SetSurfaceRLE(out.getSurface(), 1) < 0) {
        throw std::format("Couldn't set RLE acceleration for new skin surface: {}", SDL_GetError());
    }

    return out;
}

bool validSkinSurface(const gfxSprite& skin)
{
    return skin.getWidth() == 192 && skin.getHeight() == 32;
}
} // namespace

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

RGB getRgb(SDL_Surface* surf, int x, int y)
{
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
    const gfxSprite skin = ImageLoader(path).withoutColorKey().withoutOptimization().create();

    if (!validSkinSurface(skin))
        throw std::format("Invalid skin file: {} has incorrect dimensions", path.generic_string());

    SpriteStrip strip;

    for (size_t i = 0; i < 2; i++) {
        strip[i * 2] = createSkinSurface(skin, i, colorScheme, true, false);
    }

    if (fLoadBothDirections) {
        for (size_t i = 0; i < 2; i++) {
            strip[i * 2 + 1] = createSkinSurface(skin, i, colorScheme, true, true);
        }
    }

    return strip;
}

SpriteStrip gfx_loadfullskin(const fs::path& path, short colorScheme)
{
    const gfxSprite skin = ImageLoader(path).withoutColorKey().withoutOptimization().create();

    if (!validSkinSurface(skin))
        throw std::format("Invalid skin file: {} has incorrect dimensions", path.generic_string());

    SpriteStrip strip;

    for (size_t k = 0; k < 4; k++) {
        for (size_t j = 0; j < 2; j++) {
            strip[(k * 2) + j] = createSkinSurface(skin, k, colorScheme, true, j != 0);
        }
    }

    //Dead Flying Sprite
    strip[8] = createSkinSurface(skin, 4, colorScheme, true, false);

    //Dead Stomped Sprite
    strip[9] = createSkinSurface(skin, 5, colorScheme, true, false);

    return strip;
}

void gfx_cliprect(SDL_Rect& srcRect, SDL_Rect& dstRect, const SDL_Rect& clipArea)
{
    if (dstRect.x >= clipArea.x + clipArea.w
        || dstRect.x + dstRect.w < clipArea.x
        || dstRect.y >= clipArea.y + clipArea.h
        || dstRect.y + dstRect.h < clipArea.y)
    {
        srcRect.w = 0;
        srcRect.h = 0;
        return;
    }

    if (dstRect.x < clipArea.x) {
        int iDiffX = clipArea.x - dstRect.x;
        srcRect.x += iDiffX;
        srcRect.w -= iDiffX;
        dstRect.x = clipArea.x;
    }

    if (dstRect.x + dstRect.w >= clipArea.x + clipArea.w) {
        int iDiffX = dstRect.x + dstRect.w - clipArea.x - clipArea.w;
        srcRect.w -= iDiffX;
    }

    if (dstRect.y < clipArea.y) {
        int iDiffY = clipArea.y - dstRect.y;
        srcRect.y += iDiffY;
        srcRect.h -= iDiffY;
        dstRect.y = clipArea.y;
    }

    if (dstRect.y + dstRect.h >= clipArea.y + clipArea.h) {
        int iDiffY = dstRect.y + dstRect.h - clipArea.y - clipArea.h;
        srcRect.h -= iDiffY;
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
    gfxSprite& sprite,
    short dstX, short dstY,
    short srcX, short srcY,
    short iw, short ih,
    const SDL_Rect& clipRect,
    bool wrap,
    std::optional<std::pair<ClipEdge, int>> clip)
{
    //need to set source rect before each blit so it can be clipped correctly
    SDL_Rect rSrcRect = {srcX, srcY, iw, ih};
    SDL_Rect rDstRect = {dstX, dstY, iw, ih};

    gfx_cliprect(rSrcRect, rDstRect, clipRect);

    if (clip) {
        auto [edge, threshold] = *clip;
        if (gfx_adjusthiddenrects(rSrcRect, rDstRect, edge, threshold))
            return;
    }

    // Blit onto the screen surface
    sprite.draw(rSrcRect, blitdest, rDstRect);

    if (wrap) {
        //Deal with wrapping over sides of screen
        bool fBlitSide = false;
        if (dstX < clipRect.x) {
            rDstRect.x = dstX + 320; // TODO: Get it from a global setting.
            fBlitSide = true;
        } else if (dstX + iw >= clipRect.x + clipRect.w) {
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

            gfx_cliprect(rSrcRect, rDstRect, clipRect);

            if (clip) {
                auto [edge, threshold] = *clip;
                if (gfx_adjusthiddenrects(rSrcRect, rDstRect, edge, threshold))
                    return;
            }

            sprite.draw(rSrcRect, blitdest, rDstRect);
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
