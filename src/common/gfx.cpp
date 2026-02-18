#include "gfx.h"

#include "gfx/Color.h"
#include "gfx/gfxSDL.h"

#include "SDL_image.h"
#include "sdl12wrapper.h"

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <iostream>
#include <string>

using namespace std;

extern SDL_Surface * blitdest;
extern SDL_Surface * screen;

extern short x_shake;
extern short y_shake;

GraphicsSDL gfx;


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
 * @param  colorKey     transparent color
 * @param  team         team index for color [0-3]
 * @param  allStates    render all player states
 * @param  mirrored     render horizontally mirrored
 * @return              team-colored surface
 */
SDL_Surface* createSkinSurface(
    SDL_Surface* source,
    size_t sourceFrame,
    const RGB& colorKey,
    size_t team,
    bool allStates,
    bool mirrored)
{
    //Take the loaded skin and colorize it for each state (normal, 3 frames of invincibility, shielded, tagged, ztarred, got shine, frozen)
    const size_t outFrameCount = allStates ? PlayerPalette::NUM_PALETTES : 1;

    SDL_Surface* out = SDL_CreateRGBSurface(
        screen->flags,
        32 * outFrameCount,
        32,
        screen->format->BitsPerPixel,
        screen->format->Rmask,
        screen->format->Gmask,
        screen->format->Bmask,
        screen->format->Amask);
#ifdef USE_SDL2
    SDL_SetSurfaceBlendMode(out, SDL_BLENDMODE_NONE);
#endif

    if (SDL_MUSTLOCK(out))
        SDL_LockSurface(out);

    if (SDL_MUSTLOCK(source))
        SDL_LockSurface(source);

    const int startX = sourceFrame * 32;

    for (int y = 0; y < 32; y++) {
        for (int srcX = 0; srcX < 32; srcX++) {
            const int dstX = mirrored ? (31 - srcX) : srcX;

            const RGB pixelColor = getRgb(source, startX + srcX, y);

            bool found = false;
            for (size_t keyIdx = 0; keyIdx < gfx.getPalette().colorCodes().size() && !found; keyIdx++) {
                const RGB& key = gfx.getPalette().colorCodes()[keyIdx];
                if (key == pixelColor) {
                    for (size_t outFrame = 0; outFrame < outFrameCount; outFrame++) {
                        const RGB& paletteColor = gfx.getPalette().colorScheme(team, outFrame, keyIdx);
                        setRgb(out, outFrame * 32 + dstX, y, paletteColor);
                    }
                    found = true;
                }
            }

            if (!found) {
                for (size_t outFrame = 0; outFrame < outFrameCount; outFrame++) {
                    setRgb(out, outFrame * 32 + dstX, y, pixelColor);
                }
            }
        }
    }

    SDL_UnlockSurface(source);
    SDL_UnlockSurface(out);

    if ( SDL_SETCOLORKEY(out, SDL_TRUE, SDL_MapRGB(out->format, colorKey.r, colorKey.g, colorKey.b)) < 0 ) {
        printf("\n ERROR: Couldn't set ColorKey + RLE for new skin surface: %s\n", SDL_GetError());
        return NULL;
    }

    return out;
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
    return gfx.Init(fullscreen);
}

void gfx_changefullscreen(bool fullscreen) {
    gfx.ChangeFullScreen(fullscreen);
}

void gfx_flipscreen() {
    gfx.FlipScreen();
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
bool gfx_loadpalette(const std::string& palette_path) {
    return gfx.getPalette().load(palette_path);
}

bool ValidSkinSurface(SDL_Surface* skin)
{
    return skin->w == 192 && skin->h == 32;
}


bool gfx_loadmenuskin(gfxSprite ** gSprite, const std::string& filename, const RGB& colorkey, short colorScheme, bool fLoadBothDirections)
{
    SDL_Surface * skin = IMG_Load(filename.c_str());
    if (!skin) {
        cout << endl << " ERROR: Couldn't load " << filename << ": "
             << SDL_GetError() << endl;
        return false;
    }

    if (!ValidSkinSurface(skin))
        return false;

    for (short iSprite = 0; iSprite < 2; iSprite++) {
        SDL_Surface * skinSurface = createSkinSurface(skin, iSprite, colorkey, colorScheme, true, false);

        if (skinSurface == NULL) {
            cout << endl << " ERROR: Couldn't create menu skin from " << filename
                 << ": " << SDL_GetError() << endl;

            return false;
        }

        gSprite[iSprite * 2]->setSurface(skinSurface);
    }

    if (fLoadBothDirections) {
        for (short iSprite = 0; iSprite < 2; iSprite++) {
            SDL_Surface * skinSurface = createSkinSurface(skin, iSprite, colorkey, colorScheme, true, true);

            if (skinSurface == NULL) {
                cout << endl << " ERROR: Couldn't create menu skin from " << filename
                     << ": " << SDL_GetError() << endl;

                return false;
            }

            gSprite[iSprite * 2 + 1]->setSurface(skinSurface);
        }
    }

    SDL_FreeSurface(skin);

    return true;
}


bool gfx_loadfullskin(gfxSprite ** gSprites, const std::string& filename, const RGB& colorkey, short colorScheme)
{
    // Load the BMP file into a surface
    SDL_Surface * skin = IMG_Load(filename.c_str());

    if (!skin) {
        cout << endl << " ERROR: Couldn't load " << filename
             << ": " << SDL_GetError() << endl;
        return false;
    }

    if (!ValidSkinSurface(skin))
        return false;

    for (short k = 0; k < 4; k++) {
        for (short j = 0; j < 2; j++) {
            SDL_Surface * skinSurface = createSkinSurface(skin, k, colorkey, colorScheme, true, j != 0);

            if (skinSurface == NULL) {
                cout << endl << " ERROR: Couldn't create menu skin from "
                     << filename << ": " << SDL_GetError() << endl;
                SDL_FreeSurface(skin);
                return false;
            }

            gSprites[(k * 2) + j]->setSurface(skinSurface);
        }
    }

    //Dead Flying Sprite
    SDL_Surface * skinSurface = createSkinSurface(skin, 4, colorkey, colorScheme, true, false);

    if (skinSurface == NULL) {
        cout << endl << " ERROR: Couldn't create menu skin from " << filename << ": " << SDL_GetError() << endl;
        SDL_FreeSurface(skin);
        return false;
    }

    gSprites[8]->setSurface(skinSurface);

    //Dead Stomped Sprite
    skinSurface = createSkinSurface(skin, 5, colorkey, colorScheme, true, false);

    if (skinSurface == NULL) {
        cout << endl << " ERROR: Couldn't create menu skin from "
             << filename << ": " << SDL_GetError() << endl;
        return false;
    }

    gSprites[9]->setSurface(skinSurface);

    SDL_FreeSurface(skin);

    return true;
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

bool gfx_adjusthiddenrects(SDL_Rect * srcRect, SDL_Rect * dstRect, short iHiddenDirection, short iHiddenValue)
{
    if (iHiddenDirection == 0) {
        if (dstRect->y <= iHiddenValue) {
            if (dstRect->y + srcRect->h <= iHiddenValue)
                return true;

            short iDiff = iHiddenValue - dstRect->y;
            srcRect->y += iDiff;
            srcRect->h -= iDiff;
            dstRect->y = iHiddenValue + y_shake;
            dstRect->h = srcRect->h;
        }
    } else if (iHiddenDirection == 1) {
        if (dstRect->x + srcRect->w >= iHiddenValue) {
            if (dstRect->x >= iHiddenValue)
                return true;

            srcRect->w = iHiddenValue - dstRect->x;
            dstRect->w = srcRect->w;
        }
    } else if (iHiddenDirection == 2) {
        if (dstRect->y + srcRect->h >= iHiddenValue) {
            if (dstRect->y >= iHiddenValue)
                return true;

            srcRect->h = iHiddenValue - dstRect->y;
            dstRect->h = srcRect->h;
        }
    } else if (iHiddenDirection == 3) {
        if (dstRect->x <= iHiddenValue) {
            if (dstRect->x + srcRect->w <= iHiddenValue)
                return true;

            short iDiff = iHiddenValue - dstRect->x;
            srcRect->x += iDiff;
            srcRect->w -= iDiff;
            dstRect->x = iHiddenValue + x_shake;
            dstRect->w = srcRect->w;
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
    short hiddenDirection, short hiddenPlane)
{
    //need to set source rect before each blit so it can be clipped correctly
    SDL_Rect rSrcRect = {srcX, srcY, iw, ih};
    SDL_Rect rDstRect = {dstX, dstY, iw, ih};

    gfx_cliprect(&rSrcRect, &rDstRect, clipX, clipY, clipW, clipH);

    if (hiddenDirection > -1)
        gfx_adjusthiddenrects(&rSrcRect, &rDstRect, hiddenDirection, hiddenPlane);

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

            if (hiddenDirection > -1)
                gfx_adjusthiddenrects(&rSrcRect, &rDstRect, hiddenDirection, hiddenPlane);

            if (SDL_BlitSurface(surface, &rSrcRect, blitdest, &rDstRect) < 0) {
                fprintf(stderr, "SDL_BlitSurface error: %s\n", SDL_GetError());
                return;
            }
        }
    }
}

bool gfx_loadimagenocolorkey(gfxSprite* sprite, const std::string& path)
{
    return sprite->init(path);
}

bool gfx_loadimage(gfxSprite& gSprite, const std::string& path, bool fWrap)
{
    return gfx_loadimage(gSprite, path, colors::MAGENTA, fWrap);
}

bool gfx_loadimage(gfxSprite& gSprite, const std::string& path, Uint8 alpha, bool fWrap)
{
    bool success = gSprite.init(path, colors::MAGENTA, alpha);

    if (success)
        gSprite.SetWrap(fWrap);

    return success;
}

bool gfx_loadimage(gfxSprite& gSprite, const std::string& path, const RGB& rgb, bool fWrap)
{
    bool success = gSprite.init(path, rgb);

    if (success)
        gSprite.SetWrap(fWrap);

    return success;
}

void gfx_setjoystickteamcolor(SDL_Joystick* joystick, short team, float brightness)
{
#if SDL_VERSION_ATLEAST(2, 0, 14)
    if (team < 0) {
        // A non-playing user has no team
        return;
    }

    brightness = max(0.f, min(1.f, brightness));
    const RGB& color = gfx.getPalette().colorScheme(team, 0, 5);
    SDL_JoystickSetLED(joystick, (Uint8)(brightness * color.r), (Uint8)(brightness * color.g), (Uint8)(brightness * color.b));
#endif
}
