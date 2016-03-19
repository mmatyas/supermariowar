#ifndef SDL12WRAPPER
#define SDL12WRAPPER

#include "SDL.h"

#ifdef USE_SDL2
    extern SDL_Surface * screen;

    inline int SDL_SETCOLORKEY(SDL_Surface* surf, SDL_bool RLE, Uint32 keycolor)
    {
        int result = SDL_SetColorKey(surf, SDL_TRUE, keycolor);
        if (result < 0)
            return result;

        return (RLE ? SDL_SetSurfaceRLE(surf, RLE) : 0);
    }

    inline int SDL_SETALPHABYTE(SDL_Surface* surf, SDL_bool RLE, Uint8 alpha)
    {
        int result = SDL_SetSurfaceBlendMode(surf, SDL_BLENDMODE_BLEND);
        if (result < 0)
            return result;

        result = SDL_SetSurfaceAlphaMod(surf, alpha);
        if (result < 0)
            return result;

        return (RLE ? SDL_SetSurfaceRLE(surf, RLE) : 0);
    }

    inline int SDL_SCALEBLIT(SDL_Surface* src, SDL_Rect* srcrect,
        SDL_Surface* dst, SDL_Rect* dstrect)
    {
        return SDL_BlitScaled(src, srcrect, dst, dstrect);
    }
#else
    inline int SDL_SETCOLORKEY(SDL_Surface* surf, SDL_bool RLE, Uint32 keycolor)
    {
        return SDL_SetColorKey(surf, SDL_SRCCOLORKEY | (RLE ? SDL_RLEACCEL : 0), keycolor);
    }

    inline int SDL_SETALPHABYTE(SDL_Surface* surf, SDL_bool RLE, Uint8 alpha)
    {
        return SDL_SetAlpha(surf, SDL_SRCALPHA | (RLE ? SDL_RLEACCEL : 0), alpha);
    }

    inline int SDL_SCALEBLIT(SDL_Surface* src, SDL_Rect* srcrect,
        SDL_Surface* dst, SDL_Rect* dstrect)
    {
        return SDL_SoftStretch(src, srcrect, dst, dstrect);
    }
#endif


#endif
