#include "gfxSprite.h"

#include "gfx.h"

#include "SDL_image.h"
#include "sdl12wrapper.h"

#include <cassert>
#include <cstdio>
#include <iostream>

using namespace std;

extern SDL_Surface * blitdest;
extern SDL_Surface * screen;
extern short x_shake;
extern short y_shake;

gfxSprite::gfxSprite()
{
    clearSurface();
    iWrapSize = 640; // TODO: Get it from a global setting.
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

    fWrap = false;
}

void gfxSprite::freeSurface()
{
    if (m_picture) {
        SDL_FreeSurface(m_picture);
        m_picture = NULL;
    }
}

//
// Color keyed without alpha
//
bool gfxSprite::init(const std::string& filename, const RGB& key)
{
    cout << "loading sprite (mode 1) " << filename << "...";

    if (m_picture) {
        SDL_FreeSurface(m_picture);
        m_picture = NULL;
    }

    // Load the BMP file into a surface
    m_picture = IMG_Load(filename.c_str());
    if (!m_picture) {
        cout << endl << " ERROR: Couldn't load " << filename << ": " << IMG_GetError() << endl;
        return false;
    }

    if ( SDL_SETCOLORKEY(m_picture, SDL_TRUE, SDL_MapRGB(m_picture->format, key.r, key.g, key.b)) < 0) {
        cout << endl << " ERROR: Couldn't set ColorKey + RLE for "
             << filename << ": " << SDL_GetError() << endl;
        return false;
    }

#ifdef USE_SDL2
    // FIXME: this causes missing map tiles
    // SDL_Surface* temp = SDL_ConvertSurface(m_picture, screen->format, 0);
#else
    SDL_Surface* temp = SDL_DisplayFormat(m_picture);
    if (!temp) {
        cout << endl << " ERROR: Couldn't convert "
             << filename << " to the display's pixel format: " << SDL_GetError()
             << endl;
        return false;
    }

    SDL_FreeSurface(m_picture);
    m_picture = temp;
#endif

    m_bltrect.w = (Uint16)m_picture->w;
    m_bltrect.h = (Uint16)m_picture->h;

    cout << "done" << endl;
    return true;
}

//
// Color keyed + alpha
//
bool gfxSprite::init(const std::string& filename, const RGB& key, Uint8 alpha)
{
    cout << "Loading sprite (mode 2) " << filename << " ...";

    if (m_picture) {
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

    if ( SDL_SETCOLORKEY(m_picture, SDL_TRUE, SDL_MapRGB(m_picture->format, key.r, key.g, key.b)) < 0) {
        cout << endl << " ERROR: Couldn't set ColorKey + RLE for "
             << filename << ": " << SDL_GetError() << endl;
        return false;
    }

    if ( (SDL_SETALPHABYTE(m_picture, SDL_TRUE, alpha)) < 0) {
        cout << endl << " ERROR: Couldn't set per-surface alpha on "
             << filename << ": " << SDL_GetError() << endl;
        return false;
    }

#ifdef USE_SDL2
    SDL_Surface* temp = SDL_ConvertSurface(m_picture, screen->format, 0);
#else
    SDL_Surface* temp = SDL_DisplayFormatAlpha(m_picture);
#endif
    if (!temp) {
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

//
// Non color keyed
//
bool gfxSprite::init(const std::string& filename)
{
    cout << "loading sprite (mode 3) " << filename << "...";

    if (m_picture) {
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

#ifdef USE_SDL2
    SDL_Surface * temp = SDL_ConvertSurface(m_picture, screen->format, 0);
#else
    SDL_Surface * temp = SDL_DisplayFormat(m_picture);
#endif
    if (!temp) {
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

bool gfxSprite::draw(short x, short y)
{
    assert(m_picture != NULL);

    m_bltrect.x = x + x_shake;
    m_bltrect.y = y + y_shake;

    if (SDL_BlitSurface(m_picture, NULL, blitdest, &m_bltrect) < 0) {
        fprintf(stderr, "BlitSurface error: %s\n", SDL_GetError());
        return false;
    }

    if (fWrap) {
        if (x + m_picture->w >= iWrapSize) {
            m_bltrect.x = x - iWrapSize + x_shake;
            m_bltrect.y = y + y_shake;

            if (SDL_BlitSurface(m_picture, NULL, blitdest, &m_bltrect) < 0) {
                fprintf(stderr, "SDL_BlitSurface error: %s\n", SDL_GetError());
                return false;
            }
        } else if (x < 0) {
            m_bltrect.x = x + iWrapSize + x_shake;
            m_bltrect.y = y + y_shake;

            if (SDL_BlitSurface(m_picture, NULL, blitdest, &m_bltrect) < 0) {
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
    assert(m_picture != NULL);

    m_bltrect.x = x + x_shake;
    m_bltrect.y = y + y_shake;
    m_bltrect.w = w;
    m_bltrect.h = h;

    SDL_Rect srcrect {
        srcx,
        srcy,
        w,
        h,
    };

    if (sHiddenDirection > -1) {
        if (gfx_adjusthiddenrects(&srcrect, &m_bltrect, sHiddenDirection, sHiddenValue))
            return true;
    }

    // Blit onto the screen surface
    if (SDL_BlitSurface(m_picture, &srcrect, blitdest, &m_bltrect) < 0) {
        fprintf(stderr, "SDL_BlitSurface error: %s\n", SDL_GetError());
        return false;
    }

    if (fWrap) {
        if (x + w >= iWrapSize) {
            srcrect = {srcx, srcy, w, h};
            m_bltrect = {x - iWrapSize + x_shake, y + y_shake, w, h};

            if (sHiddenDirection > -1) {
                if (gfx_adjusthiddenrects(&srcrect, &m_bltrect, sHiddenDirection, sHiddenValue))
                    return true;
            }

            if (SDL_BlitSurface(m_picture, &srcrect, blitdest, &m_bltrect) < 0) {
                fprintf(stderr, "SDL_BlitSurface error: %s\n", SDL_GetError());
                return false;
            }
        } else if (x < 0) {
            srcrect = {srcx, srcy, w, h};
            m_bltrect = {x + iWrapSize + x_shake, y + y_shake, w, h};

            if (sHiddenDirection > -1) {
                if (gfx_adjusthiddenrects(&srcrect, &m_bltrect, sHiddenDirection, sHiddenValue))
                    return true;
            }

            if (SDL_BlitSurface(m_picture, &srcrect, blitdest, &m_bltrect) < 0) {
                fprintf(stderr, "SDL_BlitSurface error: %s\n", SDL_GetError());
                return false;
            }
        }
    }

    return true;
}

bool gfxSprite::drawStretch(short x, short y, short w, short h, short srcx, short srcy, short srcw, short srch)
{
    assert(m_picture != NULL);

    m_bltrect.x = x + x_shake;
    m_bltrect.y = y + y_shake;
    m_bltrect.w = w;
    m_bltrect.h = h;

    SDL_Rect srcrect {
        srcx,
        srcy,
        srcw,
        srch,
    };

    // Looks like SoftStretch doesn't respect transparent colors
    // I need to look into the actual SDL code to see if I can fix this
    if (SDL_SCALEBLIT(m_picture, &srcrect, blitdest, &m_bltrect) < 0) {
        fprintf(stderr, "SDL_SoftStretch error: %s\n", SDL_GetError());
        return false;
    }

    return true;
}

void gfxSprite::setalpha(Uint8 alpha)
{
    assert(m_picture != NULL);

    if ( (SDL_SETALPHABYTE(m_picture, SDL_TRUE, alpha)) < 0) {
        printf("\n ERROR: couldn't set alpha on sprite: %s\n", SDL_GetError());
    }
}

int gfxSprite::getWidth()
{
    return m_picture->w;
}
int gfxSprite::getHeight()
{
    return m_picture->h;
}

void gfxSprite::setSurface(SDL_Surface * surface)
{
    freeSurface();
    m_picture = surface;
    m_bltrect.w = (Uint16)m_picture->w;
    m_bltrect.h = (Uint16)m_picture->h;
}

bool gfxSprite::GetWrap() {
    return fWrap;
}

void gfxSprite::SetWrap(bool wrap) {
    fWrap = wrap;
}

void gfxSprite::SetWrap(bool wrap, short wrapsize) {
    fWrap = wrap;
    iWrapSize = wrapsize;
}
