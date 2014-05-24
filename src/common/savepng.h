/*
 IMG_SavePNG.h

 Originally a patch to SDL_image, LGPL

 (c) 2001 Darren Grant
 Endian fixes by J. Fortmann

*/
#ifndef IMG_SAVEPNG_H
#define IMG_SAVEPNG_H

#include "SDL.h"

/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif

int IMG_SavePNG_RW(SDL_Surface *face, SDL_RWops *src);
int IMG_SavePNG(SDL_Surface *surface, const char *file);

/* Ends C function definitions when using C++ */
#ifdef __cplusplus
};
#endif

#endif	// IMG_SAVEPNG_H
