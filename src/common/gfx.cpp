#include "gfx.h"

#include "gfx/gfxSDL.h"

#include "SDL_image.h"
#include "sdl12wrapper.h"

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

void gfx_close() {}
bool gfx_loadpalette(const std::string& palette_path) {
    return gfx.getPalette().load(palette_path.c_str());
}

bool ValidSkinSurface(SDL_Surface * skin)
{
    if (skin->w == 192 && skin->h == 32 && skin->format->BitsPerPixel == 24)
        return true;

    return false;
}

/**
 * Makes skin surface frame from a loaded sprite strip
 * @param  skin         sprite strip surface
 * @param  spriteindex  frame index [0-5]
 * @param  r            transparent color red value
 * @param  g            transparent color green value
 * @param  b            transparent color blue value
 * @param  colorScheme  player color [0-3]
 * @param  expand       wide frame?
 * @param  reverse
 * @return              skin frame surface
 */
SDL_Surface * gfx_createskinsurface(
    SDL_Surface * skin,
    short spriteindex,
    Uint8 r, Uint8 g, Uint8 b,
    unsigned short colorScheme,
    bool expand, bool reverse)
{
    int loops = 1;
    if (expand)
        loops = NUM_SCHEMES;

    //Blit over loaded skin into player image set
    SDL_Surface * temp = SDL_CreateRGBSurface(skin->flags, 32 * loops, 32, skin->format->BitsPerPixel, skin->format->Rmask, skin->format->Gmask, skin->format->Bmask, skin->format->Amask);

    //Take the loaded skin and colorize it for each state (normal, 3 frames of invincibiliy, shielded, tagged, ztarred. got shine)
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

    if ( SDL_SETCOLORKEY(temp, SDL_TRUE, SDL_MapRGB(temp->format, r, g, b)) < 0 ) {
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


bool gfx_loadmenuskin(gfxSprite ** gSprite, const std::string& filename, Uint8 r, Uint8 g, Uint8 b, short colorScheme, bool fLoadBothDirections)
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
        SDL_Surface * skinSurface = gfx_createskinsurface(skin, iSprite, r, g, b, colorScheme, true, false);

        if (skinSurface == NULL) {
            cout << endl << " ERROR: Couldn't create menu skin from " << filename
                 << ": " << SDL_GetError() << endl;

            return false;
        }

        gSprite[iSprite * 2]->setSurface(skinSurface);
    }

    if (fLoadBothDirections) {
        for (short iSprite = 0; iSprite < 2; iSprite++) {
            SDL_Surface * skinSurface = gfx_createskinsurface(skin, iSprite, r, g, b, colorScheme, true, true);

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


bool gfx_loadfullskin(gfxSprite ** gSprites, const std::string& filename, Uint8 r, Uint8 g, Uint8 b, short colorScheme)
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
            SDL_Surface * skinSurface = gfx_createskinsurface(skin, k, r, g, b, colorScheme, true, j != 0);

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
    SDL_Surface * skinSurface = gfx_createskinsurface(skin, 4, r, g, b, colorScheme, true, false);

    if (skinSurface == NULL) {
        cout << endl << " ERROR: Couldn't create menu skin from " << filename << ": " << SDL_GetError() << endl;
        SDL_FreeSurface(skin);
        return false;
    }

    gSprites[8]->setSurface(skinSurface);

    //Dead Stomped Sprite
    skinSurface = gfx_createskinsurface(skin, 5, r, g, b, colorScheme, true, false);

    if (skinSurface == NULL) {
        cout << endl << " ERROR: Couldn't create menu skin from "
             << filename << ": " << SDL_GetError() << endl;
        return false;
    }

    gSprites[9]->setSurface(skinSurface);

    SDL_FreeSurface(skin);

    return true;
}

SDL_Surface * gfx_createteamcoloredsurface(SDL_Surface * sImage, short iColor, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    SDL_Surface * sTempImage = SDL_CreateRGBSurface(sImage->flags, iColor == -2 ? sImage->w << 2 : sImage->w, iColor == -1 ? sImage->h << 2 : sImage->h, sImage->format->BitsPerPixel, sImage->format->Rmask, sImage->format->Gmask, sImage->format->Bmask, sImage->format->Amask);

    //Take the loaded image and colorize it
    if (SDL_MUSTLOCK(sTempImage))
        SDL_LockSurface(sTempImage);

    if (SDL_MUSTLOCK(sImage))
        SDL_LockSurface(sImage);

    //Need two counters because the pitch of the surfaces could be different
    int iSrcPixelCounter = 0;
    int iDstPixelCounter = 0;

    int iNextImageOffset = 0;

    if (iColor == -1)
        iNextImageOffset = sImage->pitch * sImage->h;
    else if (iColor == -2)
        iNextImageOffset = sImage->w * 3;

    Uint8 * pixels = (Uint8*)sImage->pixels;
    Uint8 * temppixels = (Uint8*)sTempImage->pixels;

    //Adjust what order we grab the pixels based on where R, G and B are
    short iRedOffset = sImage->format->Rshift >> 3;
    short iGreenOffset = sImage->format->Gshift >> 3;
    short iBlueOffset = sImage->format->Bshift >> 3;

    for (unsigned short j = 0; j < sImage->w; j++) {
        for (unsigned short i = 0; i < sImage->h; i++) {
            Uint8 iColorByte1 = pixels[iSrcPixelCounter + iRedOffset];
            Uint8 iColorByte2 = pixels[iSrcPixelCounter + iGreenOffset];
            Uint8 iColorByte3 = pixels[iSrcPixelCounter + iBlueOffset];

            bool fFoundColor = false;
            for (unsigned short m = 0; m < gfx.getPalette().colorCount(); m++) {
                if (gfx.getPalette().matchesColorAtID(m, iColorByte1, iColorByte2, iColorByte3)) {
                    if (iColor < 0) {
                        for (unsigned short iTeam = 0; iTeam < 4; iTeam++) {
                            unsigned short base = iDstPixelCounter + iNextImageOffset * iTeam;
                            gfx.getPalette().copyColorSchemeTo(
                                iTeam, 0, m,
                                temppixels[base + iRedOffset],
                                temppixels[base + iGreenOffset],
                                temppixels[base + iBlueOffset]);
                        }
                    } else {
                        gfx.getPalette().copyColorSchemeTo(
                            iColor, 0, m,
                            temppixels[iDstPixelCounter + iRedOffset],
                            temppixels[iDstPixelCounter + iGreenOffset],
                            temppixels[iDstPixelCounter + iBlueOffset]);
                    }

                    fFoundColor = true;
                    break;
                }
            }

            if (!fFoundColor) {
                if (iColor < 0) {
                    for (short iTeam = 0; iTeam < 4; iTeam++) {
                        temppixels[iDstPixelCounter + iRedOffset + iNextImageOffset * iTeam] = iColorByte1;
                        temppixels[iDstPixelCounter + iGreenOffset + iNextImageOffset * iTeam] = iColorByte2;
                        temppixels[iDstPixelCounter + iBlueOffset + iNextImageOffset * iTeam] = iColorByte3;
                    }
                } else {
                    temppixels[iDstPixelCounter + iRedOffset] = iColorByte1;
                    temppixels[iDstPixelCounter + iGreenOffset] = iColorByte2;
                    temppixels[iDstPixelCounter + iBlueOffset] = iColorByte3;
                }
            }

            iSrcPixelCounter += 3;
            iDstPixelCounter += 3;
        }

        iSrcPixelCounter += sImage->pitch - sImage->w * 3;
        iDstPixelCounter += sTempImage->pitch - sImage->w * 3;
    }

    SDL_UnlockSurface(sImage);
    SDL_UnlockSurface(sTempImage);

    if ( SDL_SETCOLORKEY(sTempImage, SDL_TRUE, SDL_MapRGB(sTempImage->format, r, g, b)) < 0 ) {
        printf("\n ERROR: Couldn't set ColorKey + RLE for new team colored surface: %s\n", SDL_GetError());
        return NULL;
    }

    if (a < 255) {
        if (SDL_SETALPHABYTE(sTempImage, SDL_TRUE, a) < 0) {
            cout << endl << " ERROR: Couldn't set per-surface alpha: " << SDL_GetError() << endl;
            return NULL;
        }
    }

#ifdef USE_SDL2
    SDL_Surface * sFinalImage = SDL_ConvertSurface(sTempImage, screen->format, 0);
#else
    SDL_Surface * sFinalImage = SDL_DisplayFormat(sTempImage);
#endif
    if (!sFinalImage) {
        printf("\n ERROR: Couldn't create new surface using SDL_DisplayFormat(): %s\n", SDL_GetError());
        return NULL;
    }
    SDL_FreeSurface(sTempImage);

    return sFinalImage;
}

bool gfx_loadteamcoloredimage(gfxSprite ** gSprites, const std::string& filename, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool fWrap)
{
    //Load the image into a surface
    SDL_Surface * sImage = IMG_Load(filename.c_str());

    if (sImage == NULL) {
        cout << endl << " ERROR: Couldn't load " << filename << ": " << SDL_GetError() << endl;
        return false;
    }

    for (short k = 0; k < 4; k++) {
        SDL_Surface * sTeamColoredSurface = gfx_createteamcoloredsurface(sImage, k, r, g, b, a);

        if (sTeamColoredSurface == NULL) {
            cout << endl << " ERROR: Couldn't create menu skin from " << filename << ": " << SDL_GetError() << endl;
            SDL_FreeSurface(sTeamColoredSurface);
            return false;
        }

        gSprites[k]->setSurface(sTeamColoredSurface);
        gSprites[k]->SetWrap(fWrap);
    }

    SDL_FreeSurface(sImage);


    return true;
}

bool gfx_loadteamcoloredimage(gfxSprite * gSprites, const std::string& filename, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool fVertical, bool fWrap)
{
    //Load the image into a surface
    SDL_Surface * sImage = IMG_Load(filename.c_str());

    if (sImage == NULL) {
        cout << endl << " ERROR: Couldn't load " << filename << ": " << SDL_GetError() << endl;
        return false;
    }

    SDL_Surface * sTeamColoredSurface = gfx_createteamcoloredsurface(sImage, fVertical ? -1 : -2, r, g, b, a);

    if (sTeamColoredSurface == NULL) {
        cout << endl << " ERROR: Couldn't create menu skin from " << filename << ": " << SDL_GetError() << endl;
        SDL_FreeSurface(sTeamColoredSurface);
        return false;
    }

    gSprites->setSurface(sTeamColoredSurface);
    gSprites->SetWrap(fWrap);

    SDL_FreeSurface(sImage);

    return true;
}

void gfx_setrect(SDL_Rect * rect, short x, short y, short w, short h)
{
    rect->x = x;
    rect->y = y;
    rect->w = w;
    rect->h = h;
}

void gfx_setrect(SDL_Rect * rect, SDL_Rect * copyrect)
{
    rect->x = copyrect->x;
    rect->y = copyrect->y;
    rect->w = copyrect->w;
    rect->h = copyrect->h;
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

bool gfx_loadteamcoloredimage(gfxSprite * gSprites, const std::string& filename, bool fVertical, bool fWrap)
{
    return gfx_loadteamcoloredimage(gSprites, filename, 255, 0, 255, 255, fVertical, fWrap);
}

bool gfx_loadteamcoloredimage(gfxSprite * gSprites, const std::string& filename, Uint8 a, bool fVertical, bool fWrap)
{
    return gfx_loadteamcoloredimage(gSprites, filename, 255, 0, 255, a, fVertical, fWrap);
}

bool gfx_loadimagenocolorkey(gfxSprite * gSprite, const std::string& f)
{
    return gSprite->init(f);
}

bool gfx_loadimage(gfxSprite * gSprite, const std::string& f, bool fWrap, bool fUseAccel)
{
    return gfx_loadimage(gSprite, f, 255, 0, 255, fWrap, fUseAccel);
}

bool gfx_loadimage(gfxSprite * gSprite, const std::string& f, Uint8 alpha, bool fWrap, bool fUseAccel)
{
    bool fRet = gSprite->init(f, 255, 0, 255, alpha, fUseAccel);

    if (fRet)
        gSprite->SetWrap(fWrap);

    return fRet;
}

bool gfx_loadimage(gfxSprite * gSprite, const std::string& f, Uint8 r, Uint8 g, Uint8 b, bool fWrap, bool fUseAccel)
{
    bool fRet = gSprite->init(f, r, g, b, fUseAccel);

    if (fRet)
        gSprite->SetWrap(fWrap);

    return fRet;
}
