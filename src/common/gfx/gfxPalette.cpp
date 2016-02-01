#include "gfxPalette.h"

#include "SDL_image.h"

#include <cassert>
#include <cstdio>

gfxPalette::gfxPalette()
    : numcolors(0)
{
    for (int k = 0; k < 3; k++) {
        colorcodes[k] = NULL;

        for (int i = 0; i < 4; i++)
            for (int j = 0; j < NUM_SCHEMES; j++)
                colorschemes[i][j][k] = NULL;
    }
}

gfxPalette::~gfxPalette()
{
    clear();
}

void gfxPalette::clear()
{
    for (int k = 0; k < 3; k++) {
        delete [] colorcodes[k];
        colorcodes[k] = NULL;

        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < NUM_SCHEMES; j++) {
                delete [] colorschemes[i][j][k];
                colorschemes[i][j][k] = NULL;
            }
        }
    }
}

bool gfxPalette::matchesColorAtID(unsigned short id, uint8_t r, uint8_t g, uint8_t b)
{
    assert(id < numcolors);
    return r == colorcodes[0][id] && g == colorcodes[1][id] && b == colorcodes[2][id];
}

void gfxPalette::copyColorSchemeTo(
    unsigned short teamID, unsigned short schemeID, unsigned short colorID,
    uint8_t& r, uint8_t& g, uint8_t& b)
{
    r = colorschemes[teamID][schemeID][0][colorID];
    g = colorschemes[teamID][schemeID][1][colorID];
    b = colorschemes[teamID][schemeID][2][colorID];
}

bool gfxPalette::load(const char* palette_path)
{
    clear();

    SDL_Surface * palette = IMG_Load(palette_path);

    if ( palette == NULL ) {
        printf("Couldn't load color palette: %s\n", SDL_GetError());
        return false;
    }

    numcolors = (short)palette->w;

    for (int k = 0; k < 3; k++) {
        colorcodes[k] = new Uint8[numcolors];

        for (int i = 0; i < 4; i++)
            for (int j = 0; j < NUM_SCHEMES; j++)
                colorschemes[i][j][k] = new Uint8[numcolors];
    }

    if (SDL_MUSTLOCK(palette))
        SDL_LockSurface(palette);

    int counter = 0;

    Uint8 * pixels = (Uint8*)palette->pixels;

    short iRedIndex = palette->format->Rshift >> 3;
    short iGreenIndex = palette->format->Gshift >> 3;
    short iBlueIndex = palette->format->Bshift >> 3;

    for (int k = 0; k < numcolors; k++) {
        colorcodes[iRedIndex][k] = pixels[counter++];
        colorcodes[iGreenIndex][k] = pixels[counter++];
        colorcodes[iBlueIndex][k] = pixels[counter++];
    }

    counter += palette->pitch - palette->w * 3;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < NUM_SCHEMES; j++) {
            for (int m = 0; m < numcolors; m++) {
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
