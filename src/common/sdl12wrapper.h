#ifndef SDL12WRAPPER
#define SDL12WRAPPER


#ifdef USE_SDL2
    inline int SDL_SETCOLORKEY(SDL_Surface * surf, SDL_bool RLE, Uint32 keycolor)
    {
        int result = SDL_SetColorKey(surf, SDL_TRUE, keycolor);
        if (result < 0)
            return result;

        return (RLE ? SDL_SetSurfaceRLE(surf, RLE) : 0);
    }

    inline int SDL_SETALPHABYTE(SDL_Surface * surf, SDL_bool RLE, Uint8 alpha)
    {
        int result = SDL_SetSurfaceBlendMode(surf, SDL_BLENDMODE_BLEND);
        if (result < 0)
            return result;

        result = SDL_SetSurfaceAlphaMod(surf, alpha);
        if (result < 0)
            return result;

        return (RLE ? SDL_SetSurfaceRLE(surf, RLE) : 0);
    }

    #define SDL_DisplayFormat(surf) SDL_ConvertSurface(surf, screen->format, 0)

    inline SDL_Surface * SDL_DisplayFormatAlpha(SDL_Surface *surface, SDL_Surface *screen)
    {
        SDL_PixelFormat *vf;
        SDL_PixelFormat *format;
        SDL_Surface *converted;
        /* default to ARGB8888 */
        Uint32 amask = 0xff000000;
        Uint32 rmask = 0x00ff0000;
        Uint32 gmask = 0x0000ff00;
        Uint32 bmask = 0x000000ff;

        if ( !screen ) {
            SDL_SetError("No video mode has been set");
            return(NULL);
        }
        vf = screen->format;

        switch(vf->BytesPerPixel) {
            case 2:
            /* For XGY5[56]5, use, AXGY8888, where {X, Y} = {R, B}.
               For anything else (like ARGB4444) it doesn't matter
               since we have no special code for it anyway */
            if ( (vf->Rmask == 0x1f) &&
                 (vf->Bmask == 0xf800 || vf->Bmask == 0x7c00)) {
                rmask = 0xff;
                bmask = 0xff0000;
            }
            break;

            case 3:
            case 4:
            /* Keep the video format, as long as the high 8 bits are
               unused or alpha */
            if ( (vf->Rmask == 0xff) && (vf->Bmask == 0xff0000) ) {
                rmask = 0xff;
                bmask = 0xff0000;
            } else if ( vf->Rmask == 0xFF00 && (vf->Bmask == 0xFF000000) ) {
                amask = 0x000000FF;
                rmask = 0x0000FF00;
                gmask = 0x00FF0000;
                bmask = 0xFF000000;
            }
            break;

            default:
            /* We have no other optimised formats right now. When/if a new
               optimised alpha format is written, add the converter here */
            break;
        }
        format = SDL_AllocFormat(SDL_MasksToPixelFormatEnum(32, rmask, gmask, bmask, amask));
        converted = SDL_ConvertSurface(surface, format, 0);
        SDL_FreeFormat(format);
        return(converted);
    }

    inline int SDL_SCALEBLIT(SDL_Surface* src, SDL_Rect* srcrect,
        SDL_Surface* dst, SDL_Rect* dstrect)
    {
        return SDL_BlitScaled(src, srcrect, dst, dstrect);
    }
#else
    inline int SDL_SETCOLORKEY(SDL_Surface * surf, SDL_bool RLE, Uint32 keycolor)
    {
        return SDL_SetColorKey(surf, SDL_SRCCOLORKEY | (RLE ? SDL_RLEACCEL : 0), keycolor);
    }

    inline int SDL_SETALPHABYTE(SDL_Surface * surf, SDL_bool RLE, Uint8 alpha)
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
