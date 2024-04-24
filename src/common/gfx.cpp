#include "gfx.h"

#include "gfx/Color.h"
#include "gfx/gfxSDL.h"

#include "SDL_image.h"
#include "sdl12wrapper.h"

#include <algorithm>
#include <cstdio>
#include <iostream>
#include <string>

using namespace std;

extern SDL_Surface * blitdest;
extern SDL_Surface * screen;

extern short x_shake;
extern short y_shake;

GraphicsSDL gfx;

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
    return skin->w == 192 && skin->h == 32 && skin->format->BitsPerPixel == 24;
}

/**
 * Makes skin surface frame from a loaded sprite strip
 * @param  skin         sprite strip surface
 * @param  spriteindex  frame index [0-5]
 * @param  colorkey     transparent color
 * @param  colorScheme  player color [0-3]
 * @param  expand       wide frame?
 * @param  reverse
 * @return              skin frame surface
 */
SDL_Surface * gfx_createskinsurface(
    SDL_Surface * skin,
    short spriteindex,
    RGB colorkey,
    unsigned short colorScheme,
    bool expand, bool reverse)
{
    int loops = 1;
    if (expand)
        loops = PlayerPalette::NUM_PALETTES;

    //Blit over loaded skin into player image set
    SDL_Surface * temp = SDL_CreateRGBSurface(skin->flags, 32 * loops, 32, skin->format->BitsPerPixel, skin->format->Rmask, skin->format->Gmask, skin->format->Bmask, skin->format->Amask);

    //Take the loaded skin and colorize it for each state (normal, 3 frames of invincibility, shielded, tagged, ztarred, got shine, frozen)
    if (SDL_MUSTLOCK(temp))
        SDL_LockSurface(temp);

    if (SDL_MUSTLOCK(skin))
        SDL_LockSurface(skin);

    Uint8 byteperframe = 96;

    int skincounter = spriteindex * byteperframe;
    int tempcounter = 0;

    int reverseoffset = 0;

    Uint8 * pixels = (Uint8*)skin->pixels;
    Uint8 * temppixels = (Uint8*)temp->pixels;

    short iRedOffset = skin->format->Rshift >> 3;
    short iGreenOffset = skin->format->Gshift >> 3;
    short iBlueOffset = skin->format->Bshift >> 3;

    for (int j = 0; j < 32; j++) {
        for (int i = 0; i < 32; i++) {
            if (reverse)
                reverseoffset = (31 - (i * 2)) * 3;

            Uint8 iColorByte1 = pixels[skincounter + iRedOffset];
            Uint8 iColorByte2 = pixels[skincounter + iGreenOffset];
            Uint8 iColorByte3 = pixels[skincounter + iBlueOffset];

            bool fFoundColor = false;
            for (unsigned short m = 0; m < gfx.getPalette().colorCount(); m++) {
                if (gfx.getPalette().matchesColorAtID(m, iColorByte1, iColorByte2, iColorByte3)) {
                    for (unsigned short k = 0; k < loops; k++) {
                        unsigned short base = tempcounter + k * byteperframe + reverseoffset;
                        gfx.getPalette().copyColorSchemeTo(
                            colorScheme, k, m,
                            temppixels[base + iRedOffset],
                            temppixels[base + iGreenOffset],
                            temppixels[base + iBlueOffset]);
                    }

                    fFoundColor = true;
                    break;
                }
            }

            if (!fFoundColor) {
                for (int k = 0; k < loops; k++) {
                    temppixels[tempcounter + k * byteperframe + reverseoffset + iRedOffset] = iColorByte1;
                    temppixels[tempcounter + k * byteperframe + reverseoffset + iGreenOffset] = iColorByte2;
                    temppixels[tempcounter + k * byteperframe + reverseoffset + iBlueOffset] = iColorByte3;
                }
            }

            skincounter += 3;
            tempcounter += 3;
        }

        // 5 * 96 shall be replaced by a better variable expression
        skincounter += 5 * byteperframe + skin->pitch - (skin->w * 3);
        tempcounter += byteperframe * (loops - 1) + temp->pitch - (temp->w * 3);
    }

    SDL_UnlockSurface(skin);
    SDL_UnlockSurface(temp);

    if ( SDL_SETCOLORKEY(temp, SDL_TRUE, SDL_MapRGB(temp->format, colorkey.r, colorkey.g, colorkey.b)) < 0 ) {
        printf("\n ERROR: Couldn't set ColorKey + RLE for new skin surface: %s\n", SDL_GetError());
        return NULL;
    }

#ifdef USE_SDL2
    SDL_Surface * final = SDL_ConvertSurface(temp, screen->format, 0);
#else
    SDL_Surface * final = SDL_DisplayFormat(temp);
#endif
    if (!final) {
        printf("\n ERROR: Couldn't create new surface using SDL_DisplayFormat(): %s\n", SDL_GetError());
        return NULL;
    }

    SDL_FreeSurface(temp);

    return final;
}


bool gfx_loadmenuskin(gfxSprite ** gSprite, const std::string& filename, const RGB& colorkey, short colorScheme, bool fLoadBothDirections)
{
    // Load the BMP file into a surface
    SDL_Surface * skin = IMG_Load(filename.c_str());
    if (!skin) {
        cout << endl << " ERROR: Couldn't load " << filename << ": "
             << SDL_GetError() << endl;
        return false;
    }

    if (!ValidSkinSurface(skin))
        return false;

    for (short iSprite = 0; iSprite < 2; iSprite++) {
        SDL_Surface * skinSurface = gfx_createskinsurface(skin, iSprite, colorkey, colorScheme, true, false);

        if (skinSurface == NULL) {
            cout << endl << " ERROR: Couldn't create menu skin from " << filename
                 << ": " << SDL_GetError() << endl;

            return false;
        }

        gSprite[iSprite * 2]->setSurface(skinSurface);
    }

    if (fLoadBothDirections) {
        for (short iSprite = 0; iSprite < 2; iSprite++) {
            SDL_Surface * skinSurface = gfx_createskinsurface(skin, iSprite, colorkey, colorScheme, true, true);

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
            SDL_Surface * skinSurface = gfx_createskinsurface(skin, k, colorkey, colorScheme, true, j != 0);

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
    SDL_Surface * skinSurface = gfx_createskinsurface(skin, 4, colorkey, colorScheme, true, false);

    if (skinSurface == NULL) {
        cout << endl << " ERROR: Couldn't create menu skin from " << filename << ": " << SDL_GetError() << endl;
        SDL_FreeSurface(skin);
        return false;
    }

    gSprites[8]->setSurface(skinSurface);

    //Dead Stomped Sprite
    skinSurface = gfx_createskinsurface(skin, 5, colorkey, colorScheme, true, false);

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

void gfx_setjoystickteamcolor(SDL_Joystick* joystick, unsigned short team, float brightness)
{
#if SDL_VERSION_ATLEAST(2, 0, 14)
    uint8_t r = 0, g = 0, b = 0;
    brightness = max(0.f, min(1.f, brightness));
    gfx.getPalette().copyColorSchemeTo(team, 0, 5, r, g, b);
    SDL_JoystickSetLED(joystick, (Uint8)(brightness * r), (Uint8)(brightness * g), (Uint8)(brightness * b));
#endif
}
