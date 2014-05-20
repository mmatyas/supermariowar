
#include "gfx.h"
#include "global.h"

#include <stdarg.h>		//atexit?
#include <stdlib.h>		//atexit?
#include <iostream>
#include <string>
using namespace std;

#include "sdl12wrapper.h"

#ifdef _WIN32
#pragma comment(lib, "SDL_image.lib")

#ifndef _XBOX
#pragma comment(lib, "SDL.lib")
#pragma comment(lib, "SDLmain.lib")
#endif

#endif

#ifdef USE_SDL2
    extern SDL_Window   * window;
    extern SDL_Renderer * renderer;
    extern SDL_Texture  * screenAsTexture;
#endif
extern SDL_Surface *blitdest;
extern SDL_Surface *screen;

#define GFX_BPP		16
#ifdef _XBOX
#define GFX_FLAGS	SDL_SWSURFACE | SDL_HWACCEL
#else
#define GFX_FLAGS	SDL_SWSURFACE | SDL_HWACCEL
#endif

//[colorcomponents][numcolors]
Uint8 * colorcodes[3];

#define NUM_SCHEMES 9

//[numplayers][colorscheme][colorcomponents][numcolors]
Uint8 * colorschemes[4][NUM_SCHEMES][3];
short numcolors = 0;

extern short x_shake;
extern short y_shake;

//gfx_init
bool gfx_init(int w, int h, bool fullscreen)
{
    // show SDL version
    SDL_version ver_compiled;
    SDL_version ver_current;
    SDL_VERSION(&ver_compiled);
#ifdef USE_SDL2
    SDL_GetVersion(&ver_current);
#else
    const SDL_version * constptr_ver_current = SDL_Linked_Version(); // dyn. linked version
    ver_current = *constptr_ver_current;
#endif
    printf("[info] Initializing SDL %d.%d.%d (compiled with %d.%d.%d) ... ",
        ver_current.major, ver_current.minor, ver_current.patch,
        ver_compiled.major, ver_compiled.minor, ver_compiled.patch);

    // init SDL
    if( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
        printf("error: %s\n", SDL_GetError());
        return false;
    } else
        printf("ok\n");

    // Clean up on exit
    atexit(SDL_Quit);


#ifndef __EMSCRIPTEN__
    // show SDL_image version
    const SDL_version * ver_img_current = IMG_Linked_Version();
    SDL_IMAGE_VERSION(&ver_compiled);
    printf("[info] Initializing SDL image %d.%d.%d (compiled with %d.%d.%d) ... ",
        ver_img_current->major, ver_img_current->minor, ver_img_current->patch,
        ver_compiled.major, ver_compiled.minor, ver_compiled.patch);
#endif

    // init SDL_image
    int img_flags = IMG_INIT_JPG | IMG_INIT_PNG;
    if ((IMG_Init(img_flags) & img_flags) != img_flags) {
        printf("error: %s\n", IMG_GetError());
        return false;
    } else
        printf("ok\n");

#ifndef __EMSCRIPTEN__
    // Clean up on exit
    atexit(IMG_Quit);
#endif


#ifdef USE_SDL2
    window = SDL_CreateWindow("smw",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        w, h, fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
        if ( window == NULL ) {
            printf("Couldn't create %dx%d window: %s\n", w, h, SDL_GetError());
            return false;
        }

    renderer = SDL_CreateRenderer(window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
        if ( renderer == NULL ) {
            printf("Couldn't create renderer: %s\n", SDL_GetError());
            return false;
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
        SDL_RenderSetLogicalSize(renderer, w, h);

    screen = SDL_CreateRGBSurface(0, w, h, 32,
        0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
        if ( screen == NULL ) {
            printf("Couldn't create video buffer: %s\n", SDL_GetError());
            return false;
        }

    screenAsTexture = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, w, h);
        if ( screenAsTexture == NULL ) {
            printf("Couldn't create video texture: %s\n", SDL_GetError());
            return false;
        }


#else
    if(fullscreen)
        screen = SDL_SetVideoMode(w, h, GFX_BPP, GFX_FLAGS | SDL_FULLSCREEN);
    else
        screen = SDL_SetVideoMode(w, h, GFX_BPP, GFX_FLAGS);

    if ( screen == NULL ) {
        printf("Couldn't set video mode %dx%d: %s\n", w, h, SDL_GetError());
        return false;
    }
#endif

    printf("[info] running at %dx%d %dbpp\n", w, h, screen->format->BitsPerPixel);

    for(int k = 0; k < 3; k++) {
        colorcodes[k] = NULL;

        for(int i = 0; i < 4; i++)
            for(int j = 0; j < NUM_SCHEMES; j++)
                colorschemes[i][j][k] = NULL;
    }

    return true;
}

void gfx_freepalette()
{
    for(int k = 0; k < 3; k++) {
        delete [] colorcodes[k];

        for(int i = 0; i < 4; i++)
            for(int j = 0; j < NUM_SCHEMES; j++)
                delete [] colorschemes[i][j][k];
    }
}

bool gfx_loadpalette()
{
    gfx_freepalette();

    SDL_Surface * palette = IMG_Load(convertPathCP("gfx/packs/palette.bmp", gamegraphicspacklist->current_name()));

    if ( palette == NULL ) {
        printf("Couldn't load color palette: %s\n", SDL_GetError());
        return false;
    }

    numcolors = (short)palette->w;

    for(int k = 0; k < 3; k++) {
        colorcodes[k] = new Uint8[numcolors];

        for(int i = 0; i < 4; i++)
            for(int j = 0; j < NUM_SCHEMES; j++)
                colorschemes[i][j][k] = new Uint8[numcolors];
    }

    if(SDL_MUSTLOCK(palette))
        SDL_LockSurface(palette);

    int counter = 0;

    Uint8 * pixels = (Uint8*)palette->pixels;

    short iRedIndex = palette->format->Rshift >> 3;
    short iGreenIndex = palette->format->Gshift >> 3;
    short iBlueIndex = palette->format->Bshift >> 3;

    for(int k = 0; k < numcolors; k++) {
        colorcodes[iRedIndex][k] = pixels[counter++];
        colorcodes[iGreenIndex][k] = pixels[counter++];
        colorcodes[iBlueIndex][k] = pixels[counter++];
    }

    counter += palette->pitch - palette->w * 3;

    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < NUM_SCHEMES; j++) {
            for(int m = 0; m < numcolors; m++) {
                colorschemes[i][j][iRedIndex][m] = pixels[counter++];
                colorschemes[i][j][iGreenIndex][m] = pixels[counter++];
                colorschemes[i][j][iBlueIndex][m] = pixels[counter++];
            }

            counter += palette->pitch - palette->w * 3;
        }
    }

    if (SDL_MUSTLOCK(palette))
        SDL_UnlockSurface(palette);

    SDL_FreeSurface(palette);

    return true;
}

void gfx_setresolution(int w, int h, bool fullscreen)
{
#ifdef USE_SDL2
    assert(window);
    assert(renderer);
    assert(screen);
    assert(screenAsTexture);

    if(fullscreen) {
        SDL_DestroyWindow(window);
        window = SDL_CreateWindow("smw",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            w, h, SDL_WINDOW_FULLSCREEN_DESKTOP);
    }
    else
        SDL_SetWindowSize(window, w, h);

        if ( window == NULL ) {
            printf("Couldn't set video mode %dx%d: %s\n", w, h, SDL_GetError());
            //return false;
        }

    // on some systems there's a mouse input bug after re-creating the window
    if (SDL_GetRelativeMouseMode()) {
        SDL_SetRelativeMouseMode(SDL_FALSE);
        SDL_SetRelativeMouseMode(SDL_TRUE);
    }
#else
    Uint32 flags = GFX_FLAGS;
    if(fullscreen)
        flags |= SDL_FULLSCREEN;

    #ifdef _XBOX

        if(game_values.aspectratio10x11)
            flags |= SDL_10X11PIXELASPECTRATIO;

        screen = SDL_SetVideoModeWithFlickerFilter(w, h, GFX_BPP, flags, game_values.flickerfilter, game_values.softfilter);

    #else
        screen = SDL_SetVideoMode(w, h, GFX_BPP, flags);
    #endif
#endif
}

void gfx_close()
{
    for(int i = 0; i < 3; i++) {
        delete [] colorcodes[i];

        for(int j = 0; j < 4; j++)
            for(int k = 0; k < NUM_SCHEMES; k++)
                delete [] colorschemes[j][k][i];
    }
#ifdef USE_SDL2
    SDL_DestroyTexture(screenAsTexture);
    SDL_FreeSurface(screen);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
#endif
}


bool ValidSkinSurface(SDL_Surface * skin)
{
    if(skin->w == 192 && skin->h == 32 && skin->format->BitsPerPixel == 24)
        return true;

    return false;
}

SDL_Surface * gfx_createskinsurface(SDL_Surface * skin, short spriteindex, Uint8 r, Uint8 g, Uint8 b, short colorScheme, bool expand, bool reverse)
{
    int loops = 1;
    if(expand)
        loops = NUM_SCHEMES;

    //Blit over loaded skin into player image set
    SDL_Surface * temp = SDL_CreateRGBSurface(skin->flags, 32 * loops, 32, skin->format->BitsPerPixel, skin->format->Rmask, skin->format->Gmask, skin->format->Bmask, skin->format->Amask);

    //Take the loaded skin and colorize it for each state (normal, 3 frames of invincibiliy, shielded, tagged, ztarred. got shine)
    if(SDL_MUSTLOCK(temp))
        SDL_LockSurface(temp);

    if(SDL_MUSTLOCK(skin))
        SDL_LockSurface(skin);

    int skincounter = spriteindex * 96;
    int tempcounter = 0;

    int reverseoffset = 0;

    Uint8 * pixels = (Uint8*)skin->pixels;
    Uint8 * temppixels = (Uint8*)temp->pixels;

    short iRedOffset = skin->format->Rshift >> 3;
    short iGreenOffset = skin->format->Gshift >> 3;
    short iBlueOffset = skin->format->Bshift >> 3;

    for(int j = 0; j < 32; j++) {
        for(int i = 0; i < 32; i++) {
            if(reverse)
                reverseoffset = (31 - (i * 2)) * 3;

            Uint8 iColorByte1 = pixels[skincounter + iRedOffset];
            Uint8 iColorByte2 = pixels[skincounter + iGreenOffset];
            Uint8 iColorByte3 = pixels[skincounter + iBlueOffset];

            bool fFoundColor = false;
            for(int m = 0; m < numcolors; m++) {
                if(iColorByte1 == colorcodes[0][m] && iColorByte2 == colorcodes[1][m] && iColorByte3 == colorcodes[2][m]) {
                    for(int k = 0; k < loops; k++) {
                        temppixels[tempcounter + k * 96 + reverseoffset + iRedOffset] = colorschemes[colorScheme][k][0][m];
                        temppixels[tempcounter + k * 96 + reverseoffset + iGreenOffset] = colorschemes[colorScheme][k][1][m];
                        temppixels[tempcounter + k * 96 + reverseoffset + iBlueOffset] = colorschemes[colorScheme][k][2][m];
                    }

                    fFoundColor = true;
                    break;
                }
            }

            if(!fFoundColor) {
                for(int k = 0; k < loops; k++) {
                    temppixels[tempcounter + k * 96 + reverseoffset + iRedOffset] = iColorByte1;
                    temppixels[tempcounter + k * 96 + reverseoffset + iGreenOffset] = iColorByte2;
                    temppixels[tempcounter + k * 96 + reverseoffset + iBlueOffset] = iColorByte3;
                }
            }

            skincounter += 3;
            tempcounter += 3;
        }

        // 5 * 96 shall be replaced by a better variable expression
        skincounter += 5 * 96 + skin->pitch - (skin->w * 3);
        tempcounter += 96 * (loops - 1) + temp->pitch - (temp->w * 3);
    }

    SDL_UnlockSurface(skin);
    SDL_UnlockSurface(temp);

    if( SDL_SETCOLORKEY(temp, SDL_TRUE, SDL_MapRGB(temp->format, r, g, b)) < 0 ) {
        printf("\n ERROR: Couldn't set ColorKey + RLE for new skin surface: %s\n", SDL_GetError());
        return NULL;
    }

#ifdef __EMSCRIPTEN__
    SDL_Surface * final = SDL_DisplayFormatAlpha(temp);
#else
    SDL_Surface * final = SDL_DisplayFormat(temp);
#endif
    if(!final) {
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

    if(!ValidSkinSurface(skin))
        return false;

    if (skin == NULL) {
        cout << endl << " ERROR: Couldn't load " << filename << ": "
             << SDL_GetError() << endl;
        return false;
    }

    for(short iSprite = 0; iSprite < 2; iSprite++) {
        SDL_Surface * skinSurface = gfx_createskinsurface(skin, iSprite, r, g, b, colorScheme, true, false);

        if (skinSurface == NULL) {
            cout << endl << " ERROR: Couldn't create menu skin from " << filename
                 << ": " << SDL_GetError() << endl;

            return false;
        }

        gSprite[iSprite * 2]->setSurface(skinSurface);
    }

    if(fLoadBothDirections) {
        for(short iSprite = 0; iSprite < 2; iSprite++) {
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

    if(!ValidSkinSurface(skin))
        return false;

    if (skin == NULL) {
        cout << endl << " ERROR: Couldn't load " << filename
             << ": " << SDL_GetError() << endl;
        return false;
    }

    for(short k = 0; k < 4; k++) {
        for(short j = 0; j < 2; j++) {
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
    if(SDL_MUSTLOCK(sTempImage))
        SDL_LockSurface(sTempImage);

    if(SDL_MUSTLOCK(sImage))
        SDL_LockSurface(sImage);

    //Need two counters because the pitch of the surfaces could be different
    int iSrcPixelCounter = 0;
    int iDstPixelCounter = 0;

    int iNextImageOffset = 0;

    if(iColor == -1)
        iNextImageOffset = sImage->pitch * sImage->h;
    else if(iColor == -2)
        iNextImageOffset = sImage->w * 3;

    Uint8 * pixels = (Uint8*)sImage->pixels;
    Uint8 * temppixels = (Uint8*)sTempImage->pixels;

    //Adjust what order we grab the pixels based on where R, G and B are
    short iRedOffset = sImage->format->Rshift >> 3;
    short iGreenOffset = sImage->format->Gshift >> 3;
    short iBlueOffset = sImage->format->Bshift >> 3;

    for(int j = 0; j < sImage->w; j++) {
        for(int i = 0; i < sImage->h; i++) {
            Uint8 iColorByte1 = pixels[iSrcPixelCounter + iRedOffset];
            Uint8 iColorByte2 = pixels[iSrcPixelCounter + iGreenOffset];
            Uint8 iColorByte3 = pixels[iSrcPixelCounter + iBlueOffset];

            bool fFoundColor = false;
            for(int m = 0; m < numcolors; m++) {
                if(iColorByte1 == colorcodes[0][m] && iColorByte2 == colorcodes[1][m] && iColorByte3 == colorcodes[2][m]) {
                    if(iColor < 0) {
                        for(short iTeam = 0; iTeam < 4; iTeam++) {
                            temppixels[iDstPixelCounter + iRedOffset + iNextImageOffset * iTeam] = colorschemes[iTeam][0][0][m];
                            temppixels[iDstPixelCounter + iGreenOffset + iNextImageOffset * iTeam] = colorschemes[iTeam][0][1][m];
                            temppixels[iDstPixelCounter + iBlueOffset + iNextImageOffset * iTeam] = colorschemes[iTeam][0][2][m];
                        }
                    } else {
                        temppixels[iDstPixelCounter + iRedOffset] = colorschemes[iColor][0][0][m];
                        temppixels[iDstPixelCounter + iGreenOffset] = colorschemes[iColor][0][1][m];
                        temppixels[iDstPixelCounter + iBlueOffset] = colorschemes[iColor][0][2][m];
                    }

                    fFoundColor = true;
                    break;
                }
            }

            if(!fFoundColor) {
                if(iColor < 0) {
                    for(short iTeam = 0; iTeam < 4; iTeam++) {
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

    if( SDL_SETCOLORKEY(sTempImage, SDL_TRUE, SDL_MapRGB(sTempImage->format, r, g, b)) < 0 ) {
        printf("\n ERROR: Couldn't set ColorKey + RLE for new team colored surface: %s\n", SDL_GetError());
        return NULL;
    }

    if(a < 255) {
        if(SDL_SETALPHABYTE(sTempImage, SDL_TRUE, a) < 0) {
            cout << endl << " ERROR: Couldn't set per-surface alpha: " << SDL_GetError() << endl;
            return NULL;
        }
    }

#ifdef __EMSCRIPTEN__
    SDL_Surface * sFinalImage = SDL_DisplayFormatAlpha(sTempImage);
#else
    SDL_Surface * sFinalImage = SDL_DisplayFormat(sTempImage);
#endif
    if(!sFinalImage) {
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

    for(short k = 0; k < 4; k++) {
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
    if(dstRect->x >= x + w || dstRect->x + dstRect->w < x || dstRect->y >= y + h || dstRect->y + dstRect->h < y) {
        srcRect->w = 0;
        srcRect->h = 0;
        return;
    }

    if(dstRect->x < x) {
        short iDiffX = x - dstRect->x;
        srcRect->x += iDiffX;
        srcRect->w -= iDiffX;
        dstRect->x = x;
        //dstRect->w -= iDiffX;
    }

    if(dstRect->x + dstRect->w >= x + w) {
        short iDiffX = dstRect->x + dstRect->w - x - w;
        srcRect->w -= iDiffX;
        //dstRect->w -= iDiffX;
    }

    if(dstRect->y < y) {
        short iDiffY = y - dstRect->y;
        srcRect->y += iDiffY;
        srcRect->h -= iDiffY;
        dstRect->y = y;
        //dstRect->h -= iDiffY;
    }

    if(dstRect->y + dstRect->h >= y + h) {
        short iDiffY = dstRect->y + dstRect->h - y - h;
        srcRect->h -= iDiffY;
        //dstRect->h -= iDiffY;
    }
}

bool gfx_adjusthiddenrects(SDL_Rect * srcRect, SDL_Rect * dstRect, short iHiddenDirection, short iHiddenValue)
{
    if(iHiddenDirection == 0) {
        if(dstRect->y <= iHiddenValue) {
            if(dstRect->y + srcRect->h <= iHiddenValue)
                return true;

            short iDiff = iHiddenValue - dstRect->y;
            srcRect->y += iDiff;
            srcRect->h -= iDiff;
            dstRect->y = iHiddenValue + y_shake;
            dstRect->h = srcRect->h;
        }
    } else if(iHiddenDirection == 1) {
        if(dstRect->x + srcRect->w >= iHiddenValue) {
            if(dstRect->x >= iHiddenValue)
                return true;

            srcRect->w = iHiddenValue - dstRect->x;
            dstRect->w = srcRect->w;
        }
    } else if(iHiddenDirection == 2) {
        if(dstRect->y + srcRect->h >= iHiddenValue) {
            if(dstRect->y >= iHiddenValue)
                return true;

            srcRect->h = iHiddenValue - dstRect->y;
            dstRect->h = srcRect->h;
        }
    } else if(iHiddenDirection == 3) {
        if(dstRect->x <= iHiddenValue) {
            if(dstRect->x + srcRect->w <= iHiddenValue)
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

void gfx_drawpreview(SDL_Surface * surface, short dstX, short dstY, short srcX, short srcY, short iw, short ih, short clipX, short clipY, short clipW, short clipH, bool wrap, short hiddenDirection, short hiddenPlane)
{
    //need to set source rect before each blit so it can be clipped correctly
    SDL_Rect rSrcRect = {srcX, srcY, iw, ih};
    SDL_Rect rDstRect = {dstX, dstY, iw, ih};

    gfx_cliprect(&rSrcRect, &rDstRect, clipX, clipY, clipW, clipH);

    if(hiddenDirection > -1)
        gfx_adjusthiddenrects(&rSrcRect, &rDstRect, hiddenDirection, hiddenPlane);

    // Blit onto the screen surface
    if(SDL_BlitSurface(surface, &rSrcRect, blitdest, &rDstRect) < 0) {
        fprintf(stderr, "SDL_BlitSurface error: %s\n", SDL_GetError());
        return;
    }

    if(wrap) {
        //Deal with wrapping over sides of screen
        bool fBlitSide = false;
        if(dstX < clipX) {
            rDstRect.x = dstX + smw->ScreenWidth/2;
            fBlitSide = true;
        } else if(dstX + iw >= clipX + clipW) {
            rDstRect.x = dstX - smw->ScreenWidth/2;
            fBlitSide = true;
        }

        if(fBlitSide) {
            //need to set source rect before each blit so it can be clipped correctly
            rSrcRect.x = srcX;
            rSrcRect.y = srcY;
            rSrcRect.w = iw;
            rSrcRect.h = ih;

            rDstRect.y = dstY;
            rDstRect.w = iw;
            rDstRect.h = ih;

            gfx_cliprect(&rSrcRect, &rDstRect, clipX, clipY, clipW, clipH);

            if(hiddenDirection > -1)
                gfx_adjusthiddenrects(&rSrcRect, &rDstRect, hiddenDirection, hiddenPlane);

            if(SDL_BlitSurface(surface, &rSrcRect, blitdest, &rDstRect) < 0) {
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
    bool fRet = gSprite->init(f, 255, 0, 255, fUseAccel);

    if(fRet)
        gSprite->SetWrap(fWrap);

    return fRet;
}

bool gfx_loadimage(gfxSprite * gSprite, const std::string& f, Uint8 alpha, bool fWrap, bool fUseAccel)
{
    bool fRet = gSprite->init(f, 255, 0, 255, alpha, fUseAccel);

    if(fRet)
        gSprite->SetWrap(fWrap);

    return fRet;
}

bool gfx_loadimage(gfxSprite * gSprite, const std::string& f, Uint8 r, Uint8 g, Uint8 b, bool fWrap, bool fUseAccel)
{
    bool fRet = gSprite->init(f, r, g, b, fUseAccel);

    if(fRet)
        gSprite->SetWrap(fWrap);

    return fRet;
}


//gfxSprite

gfxSprite::gfxSprite()
{
    clearSurface();
    iWrapSize = smw->ScreenWidth;
}

gfxSprite::~gfxSprite()
{
    //free the allocated surface
    freeSurface();
}

void gfxSprite::clearSurface()
{
    m_bltrect.x = 0;
    m_bltrect.y = 0;
    m_bltrect.w = 0;
    m_bltrect.h = 0;
    m_picture = NULL;

    fHiddenPlane = false;
    iHiddenDirection = 0;
    iHiddenValue = 0;

    fWrap = false;
}

bool gfxSprite::init(const std::string& filename, Uint8 r, Uint8 g, Uint8 b, bool fUseAccel)
{
    cout << "loading sprite (mode 1) " << filename << "...";

    if(m_picture) {
        SDL_FreeSurface(m_picture);
        m_picture = NULL;
    }

    // Load the BMP file into a surface
    m_picture = IMG_Load(filename.c_str());
    if (!m_picture) {
        cout << endl << " ERROR: Couldn't load " << filename << ": " << IMG_GetError() << endl;
        return false;
    }

    if( SDL_SETCOLORKEY(m_picture, fUseAccel ? SDL_TRUE : SDL_FALSE, SDL_MapRGB(m_picture->format, r, g, b)) < 0) {
        cout << endl << " ERROR: Couldn't set ColorKey + RLE for "
             << filename << ": " << SDL_GetError() << endl;
        return false;
    }

#ifdef __EMSCRIPTEN__
    SDL_Surface * temp = SDL_DisplayFormatAlpha(m_picture);
#else
    SDL_Surface * temp = SDL_DisplayFormat(m_picture);
#endif

    if(!temp) {
        cout << endl << " ERROR: Couldn't convert "
             << filename << " to the display's pixel format: " << SDL_GetError()
             << endl;
        return false;
    }

    SDL_FreeSurface(m_picture);
    m_picture = temp;

    m_bltrect.w = (Uint16)m_picture->w;
    m_bltrect.h = (Uint16)m_picture->h;

    cout << "done" << endl;
    return true;
}

bool gfxSprite::init(const std::string& filename, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool fUseAccel)
{
    cout << "Loading sprite (mode 2) " << filename << " ...";

    if(m_picture) {
        SDL_FreeSurface(m_picture);
        m_picture = NULL;
    }

    // Load the BMP file into a surface
    m_picture = IMG_Load(filename.c_str());
    if (!m_picture) {
        cout << endl << " ERROR: Couldn't load "
             << filename << ": " << SDL_GetError() << endl;
        return false;
    }

    if( SDL_SETCOLORKEY(m_picture, fUseAccel ? SDL_TRUE : SDL_FALSE, SDL_MapRGB(m_picture->format, r, g, b)) < 0) {
        cout << endl << " ERROR: Couldn't set ColorKey + RLE for "
             << filename << ": " << SDL_GetError() << endl;
        return false;
    }

    if( (SDL_SETALPHABYTE(m_picture, fUseAccel ? SDL_TRUE : SDL_FALSE, a)) < 0) {
        cout << endl << " ERROR: Couldn't set per-surface alpha on "
             << filename << ": " << SDL_GetError() << endl;
        return false;
    }

#ifdef USE_SDL2
    printf("%d\n", screen->format);
    SDL_Surface *temp = SDL_DisplayFormatAlpha(m_picture, screen);
#else
    SDL_Surface *temp = SDL_DisplayFormatAlpha(m_picture);
#endif

    if(!temp) {
        cout << endl << " ERROR: Couldn't convert "
             << filename << " to the display's pixel format: " << SDL_GetError()
             << endl;
        return false;
    }
    SDL_FreeSurface(m_picture);
    m_picture = temp;

    m_bltrect.w = (Uint16)m_picture->w;
    m_bltrect.h = (Uint16)m_picture->h;

    cout << "done" << endl;
    return true;
}

bool gfxSprite::init(const std::string& filename)
{
    cout << "loading sprite (mode 3) " << filename << "...";

    if(m_picture) {
        SDL_FreeSurface(m_picture);
        m_picture = NULL;
    }

    // Load the BMP file into a surface
    m_picture = IMG_Load(filename.c_str());

    if (!m_picture) {
        cout << endl << " ERROR: Couldn't load "
             << filename << ": " << SDL_GetError() << endl;
        return false;
    }

#ifdef __EMSCRIPTEN__
    SDL_Surface * temp = SDL_DisplayFormatAlpha(m_picture);
#else
    SDL_Surface * temp = SDL_DisplayFormat(m_picture);
#endif

    if(!temp) {
        cout << endl << " ERROR: Couldn't convert "
             << filename << " to the display's pixel format: " << SDL_GetError()
             << endl;
        return false;
    }

    SDL_FreeSurface(m_picture);
    m_picture = temp;

    m_bltrect.w = (Uint16)m_picture->w;
    m_bltrect.h = (Uint16)m_picture->h;

    m_srcrect.x = 0;
    m_srcrect.y = 0;
    m_srcrect.w = (Uint16)m_picture->w;
    m_srcrect.h = (Uint16)m_picture->h;

    cout << "done" << endl;
    return true;
}

bool gfxSprite::draw(short x, short y)
{
    m_bltrect.x = x + x_shake;
    m_bltrect.y = y + y_shake;

    if(SDL_BlitSurface(m_picture, NULL, blitdest, &m_bltrect) < 0) {
        fprintf(stderr, "BlitSurface error: %s\n", SDL_GetError());
        return false;
    }

    if(fWrap) {
        if(x + m_picture->w >= iWrapSize) {
            m_bltrect.x = x - iWrapSize + x_shake;
            m_bltrect.y = y + y_shake;

            if(SDL_BlitSurface(m_picture, NULL, blitdest, &m_bltrect) < 0) {
                fprintf(stderr, "SDL_BlitSurface error: %s\n", SDL_GetError());
                return false;
            }
        } else if(x < 0) {
            m_bltrect.x = x + iWrapSize + x_shake;
            m_bltrect.y = y + y_shake;

            if(SDL_BlitSurface(m_picture, NULL, blitdest, &m_bltrect) < 0) {
                fprintf(stderr, "SDL_BlitSurface error: %s\n", SDL_GetError());
                return false;
            }
        }
    }

    return true;
}

//TODO Perf Optimization: Set w/h once when sprite is initialized, set srcx/srcy just when animation frame advance happens
bool gfxSprite::draw(short x, short y, short srcx, short srcy, short w, short h, short sHiddenDirection, short sHiddenValue)
{
    m_bltrect.x = x + x_shake;
    m_bltrect.y = y + y_shake;
    m_bltrect.w = w;
    m_bltrect.h = h;

    m_srcrect.x = srcx;
    m_srcrect.y = srcy;
    m_srcrect.w = w;
    m_srcrect.h = h;

    if(sHiddenDirection > -1) {
        if(gfx_adjusthiddenrects(&m_srcrect, &m_bltrect, sHiddenDirection, sHiddenValue))
            return true;
    }

    // Blit onto the screen surface
    if(SDL_BlitSurface(m_picture, &m_srcrect, blitdest, &m_bltrect) < 0) {
        fprintf(stderr, "SDL_BlitSurface error: %s\n", SDL_GetError());
        return false;
    }

    if(fWrap) {
        if(x + w >= iWrapSize) {
            gfx_setrect(&m_srcrect, srcx, srcy, w, h);
            gfx_setrect(&m_bltrect, x - iWrapSize + x_shake, y + y_shake, w, h);

            if(sHiddenDirection > -1) {
                if(gfx_adjusthiddenrects(&m_srcrect, &m_bltrect, sHiddenDirection, sHiddenValue))
                    return true;
            }

            if(SDL_BlitSurface(m_picture, &m_srcrect, blitdest, &m_bltrect) < 0) {
                fprintf(stderr, "SDL_BlitSurface error: %s\n", SDL_GetError());
                return false;
            }
        } else if(x < 0) {
            gfx_setrect(&m_srcrect, srcx, srcy, w, h);
            gfx_setrect(&m_bltrect, x + iWrapSize + x_shake, y + y_shake, w, h);

            if(sHiddenDirection > -1) {
                if(gfx_adjusthiddenrects(&m_srcrect, &m_bltrect, sHiddenDirection, sHiddenValue))
                    return true;
            }

            if(SDL_BlitSurface(m_picture, &m_srcrect, blitdest, &m_bltrect) < 0) {
                fprintf(stderr, "SDL_BlitSurface error: %s\n", SDL_GetError());
                return false;
            }
        }
    }

    return true;
}

bool gfxSprite::drawStretch(short x, short y, short w, short h, short srcx, short srcy, short srcw, short srch)
{
    m_bltrect.x = x + x_shake;
    m_bltrect.y = y + y_shake;
    m_bltrect.w = w;
    m_bltrect.h = h;

    m_srcrect.x = srcx;
    m_srcrect.y = srcy;
    m_srcrect.w = srcw;
    m_srcrect.h = srch;

    // Looks like SoftStretch doesn't respect transparent colors
    // I need to look into the actual SDL code to see if I can fix this
    if(SDL_SCALEBLIT(m_picture, &m_srcrect, blitdest, &m_bltrect) < 0) {
        fprintf(stderr, "SDL_SoftStretch error: %s\n", SDL_GetError());
        return false;
    }

    return true;
}

void gfxSprite::setalpha(Uint8 alpha)
{
    assert(m_picture != NULL);

    if( (SDL_SETALPHABYTE(m_picture, SDL_TRUE, alpha)) < 0) {
        printf("\n ERROR: couldn't set alpha on sprite: %s\n", SDL_GetError());
    }
}

void gfxSprite::freeSurface()
{
    if(m_picture) {
        SDL_FreeSurface(m_picture);
        m_picture = NULL;
    }
}


// gfxFont
gfxFont::gfxFont()
{
    m_font = NULL;
}

gfxFont::~gfxFont()
{
    if(m_font)
        SFont_FreeFont(m_font);
};

bool gfxFont::init(const std::string& filename)
{
    if(m_font)
        SFont_FreeFont(m_font);

    cout << "loading font " << filename << " ... ";

    SDL_Surface *fontsurf = IMG_Load(filename.c_str());
    if(fontsurf == NULL) {
        cout << endl << " ERROR: Couldn't load file "
             << filename << ": " << SDL_GetError() << endl;
        return false;
    }

    m_font = SFont_InitFont(fontsurf);
    if(!m_font) {
        cout << endl << " ERROR: an error occurre while loading the font." << endl;
        return false;
    }

    cout << "done" << endl;
    return true;
}

void gfxFont::draw(int x, int y, const std::string& s)
{
    //if(y + getHeight() < 0)
    //	return;

    SFont_Write(blitdest, m_font, x, y, s.c_str());
}

void gfxFont::drawChopRight(int x, int y, int width, const char *s)
{
    //if(y + getHeight() < 0)
    //	return;

    SFont_WriteChopRight(blitdest, m_font, x, y, width, s);
}

void gfxFont::drawChopLeft(int x, int y, int width, const char *s)
{
    //if(y + getHeight() < 0)
    //	return;

    SFont_WriteChopLeft(blitdest, m_font, x, y, width, s);
}

void gfxFont::drawCentered(int x, int y, const char *text)
{
    //if(y + getHeight() < 0)
    //	return;

    SFont_WriteCenter(blitdest, m_font, x, y, text);
};

void gfxFont::drawChopCentered(int x, int y, int width, const char *text)
{
    //if(y + getHeight() < 0)
    //	return;

    SFont_WriteChopCenter(blitdest, m_font, x, y, width, text);
};

void gfxFont::drawRightJustified(int x, int y, const char *s, ...)
{
    char buffer[256];

    va_list zeiger;
    va_start(zeiger, s);
    vsprintf(buffer, s, zeiger);
    va_end(zeiger);

    //if(y + getHeight() < 0)
    //	return;

    SFont_WriteRight(blitdest, m_font, x, y, buffer);
};


void gfxFont::drawf(int x, int y, const char *s, ...)
{
    char buffer[256];

    va_list zeiger;
    va_start(zeiger, s);
    vsprintf(buffer, s, zeiger);
    va_end(zeiger);

    draw(x,y,buffer);
}

void gfxFont::setalpha(Uint8 alpha)
{
    if( (SDL_SETALPHABYTE(m_font->Surface, SDL_TRUE, alpha)) < 0) {
        printf("\n ERROR: couldn't set alpha on sprite: %s\n", SDL_GetError());
    }
}
