/*  SFont: a simple font-library that uses special .pngs as fonts
    Copyright (C) 2003 Karl Bartel

    License: GPL or LGPL (at your choice)
    WWW: http://www.linux-games.com/sfont/

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Karl Bartel
    Cecilienstr. 14
    12307 Berlin
    GERMANY
    karlb@gmx.net
*/

#include "SFont.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"
#include "sdl12wrapper.h"

extern short x_shake;
extern short y_shake;

static Uint32 GetPixel(SDL_Surface *Surface, Sint32 X, Sint32 Y)
{
   Uint8  *bits;
   Uint32 Bpp;

   assert(X>=0);
   assert(X<Surface->w);

   Bpp = Surface->format->BytesPerPixel;
   bits = ((Uint8 *)Surface->pixels)+Y*Surface->pitch+X*Bpp;

   // Get the pixel
   switch (Bpp) {
      case 1:
         return *((Uint8 *)Surface->pixels + Y * Surface->pitch + X);
         break;
      case 2:
         return *((Uint16 *)Surface->pixels + Y * Surface->pitch/2 + X);
         break;
      case 3: { // Format/endian independent
         Uint8 r, g, b;
         r = *((bits)+Surface->format->Rshift/8);
         g = *((bits)+Surface->format->Gshift/8);
         b = *((bits)+Surface->format->Bshift/8);
         return SDL_MapRGB(Surface->format, r, g, b);
         }
         break;
      case 4:
         return *((Uint32 *)Surface->pixels + Y * Surface->pitch/4 + X);
         break;
   }

   return 0;
}

SFont_Font* SFont_InitFont(SDL_Surface* Surface)
{
    int x = 0, i = 0;
    Uint32 pixel;
    SFont_Font* Font;
    Uint32 pink;

    if (Surface == NULL)
	return NULL;

    Font = (SFont_Font *) malloc(sizeof(SFont_Font));
    Font->Surface = Surface;

    SDL_LockSurface(Surface);

    pink = SDL_MapRGB(Surface->format, 255, 0, 255);
    while (x < Surface->w) {
	if (GetPixel(Surface, x, 0) == pink) {
    	    Font->CharPos[i++]=x;
            while ((x < Surface->w) && (GetPixel(Surface, x, 0)== pink))
		x++;
	    Font->CharPos[i++]=x;
	}
	x++;
    }
    Font->MaxPos = x-1;

    pixel = GetPixel(Surface, 0, Surface->h-1);
    SDL_UnlockSurface(Surface);
#ifdef USE_SDL2
    SDL_SetColorKey(Surface, SDL_TRUE, pixel);
#else
    SDL_SetColorKey(Surface, SDL_SRCCOLORKEY, pixel);
#endif

    return Font;
}

void SFont_FreeFont(SFont_Font* FontInfo)
{
    SDL_FreeSurface(FontInfo->Surface);
    free(FontInfo);
}

void SFont_Write(SDL_Surface *Surface, const SFont_Font *Font,
		 int x, int y, const char *text)
{
	const char* c;
    int charoffset;
    SDL_Rect srcrect, dstrect;

    if (text == NULL)
		return;

    // these values won't change in the loop
    srcrect.y = 1;
    srcrect.h = dstrect.h = (Uint16)Font->Surface->h - 1;

    for (c = text; *c != '\0' && x <= Surface->w ; c++) {
		charoffset = ((int) (*c - 33)) * 2 + 1;
		// skip spaces and nonprintable characters
        if (*c == ' ' || charoffset < 0 || charoffset > Font->MaxPos) {
			x += Font->CharPos[2]-Font->CharPos[1];
			continue;
		}

		srcrect.w = dstrect.w = (Sint16)
			(((Font->CharPos[charoffset+2] + Font->CharPos[charoffset+1]) >> 1) -
			((Font->CharPos[charoffset] + Font->CharPos[charoffset-1]) >> 1));

		srcrect.x = (Sint16)(Font->CharPos[charoffset] + Font->CharPos[charoffset-1]) >> 1;

		//changed (Florian Hufsky) (caused a float to short warning)
		//dstrect.x = x - (float)(Font->CharPos[charoffset]
		//		      - Font->CharPos[charoffset-1])/2;
		dstrect.x = (short) (x - (float)((Font->CharPos[charoffset]
					- Font->CharPos[charoffset-1]) >> 1)) + x_shake;

		dstrect.y = (Sint16)(y + y_shake);

		SDL_BlitSurface(Font->Surface, &srcrect, Surface, &dstrect);

		x += Font->CharPos[charoffset+1] - Font->CharPos[charoffset];
    }
}

int SFont_TextWidth(const SFont_Font *Font, const char *text)
{
    const char* c;
    int charoffset=0;
    int width = 0;

    if (text == NULL)
	return 0;

    for (c = text; *c != '\0'; c++) {
		charoffset = ((int) *c - 33) * 2 + 1;

		// skip spaces and nonprintable characters
        if (*c == ' ' || charoffset < 0 || charoffset > Font->MaxPos) {
            width += Font->CharPos[2] - Font->CharPos[1];
			continue;
		}

		width += Font->CharPos[charoffset+1] - Font->CharPos[charoffset];
    }

    return width;
}

int SFont_TextHeight(const SFont_Font* Font)
{
    return Font->Surface->h - 1;
}

void SFont_WriteCenter(SDL_Surface *Surface, const SFont_Font *Font, int x, int y, const char *text)
{
    SFont_Write(Surface, Font, x - (SFont_TextWidth(Font, text) >> 1), y, text);
}

void SFont_WriteChopCenter(SDL_Surface *Surface, const SFont_Font* Font, int x, int y, int w, const char *text)
{
	const char* c;
	int iCurrentWidth = 0;
	int iCurrentChar = 0;
	char szText[256];

	//array overflow safe
	strncpy(szText, text, 255);
	szText[255] = 0;

    for (c = text; *c != '\0'; c++) {
		int charoffset = ((int) text[iCurrentChar] - 33) * 2 + 1;

		int iNextWidth = 0;
		if (*c == ' ' || charoffset < 0 || charoffset > Font->MaxPos)
			iNextWidth = Font->CharPos[2] - Font->CharPos[1];
		else
			iNextWidth = Font->CharPos[charoffset+1] - Font->CharPos[charoffset];

        if (iCurrentWidth + iNextWidth > w) {
			szText[iCurrentChar] = '\0';
			break;
		}

		iCurrentChar++;
		iCurrentWidth += iNextWidth;
	}

	SFont_WriteCenter(Surface, Font, x, y, szText);
}

//Right Aligned
void SFont_WriteRight(SDL_Surface *Surface, const SFont_Font *Font, int x, int y, const char *text)
{
    SFont_Write(Surface, Font, x - SFont_TextWidth(Font, text), y, text);
}

//Left aligned with fixed width
void SFont_WriteChopRight(SDL_Surface *Surface, const SFont_Font *Font,
		 int x, int y, int w, const char *text)
{
    const char* c;
    int charoffset;
    SDL_Rect srcrect, dstrect;
	int startx = x;
	int width = 0;
	int charsize = sizeof(char);

    if (text == NULL)
		return;

    // these values won't change in the loop
    srcrect.y = 1;
    srcrect.h = dstrect.h = (Uint16)Font->Surface->h - 1;

    for (c = text; *c != '\0' && x <= Surface->w ; c += charsize) {
		charoffset = ((int) (*c - 33)) * 2 + 1;
		// skip spaces and nonprintable characters
        if (*c == ' ' || charoffset < 0 || charoffset > Font->MaxPos) {
			x += Font->CharPos[2] - Font->CharPos[1];
			continue;
		}

		srcrect.w = dstrect.w = (Sint16)
			(((Font->CharPos[charoffset+2] + Font->CharPos[charoffset+1]) >> 1) -
			((Font->CharPos[charoffset] + Font->CharPos[charoffset-1]) >> 1));

		width = Font->CharPos[charoffset+1] - Font->CharPos[charoffset];

		if (x - startx + width > w)
			break;

		srcrect.x = (Sint16)(Font->CharPos[charoffset] + Font->CharPos[charoffset-1]) >> 1;
		//changed (Florian Hufsky) (caused a float to short warning)
		//dstrect.x = x - (float)(Font->CharPos[charoffset]
		//		      - Font->CharPos[charoffset-1]) >> 1;
		dstrect.x = (short) (x - (float)((Font->CharPos[charoffset]
					- Font->CharPos[charoffset-1]) >> 1)) + x_shake;

		dstrect.y = (Sint16)(y + y_shake);

		SDL_BlitSurface(Font->Surface, &srcrect, Surface, &dstrect);

		x += width;
    }
}

//Right aligned with fixed width
void SFont_WriteChopLeft(SDL_Surface *Surface, const SFont_Font *Font,
		 int x, int y, int w, const char *text)
{
    const char* c;
    int charoffset;
    SDL_Rect srcrect, dstrect;
	int charsize = sizeof(char);
	int width = 0;
	short iPrintedWidth = 0;
	int startx = x;

    if (text == NULL)
		return;

    // these values won't change in the loop
    srcrect.y = 1;
    srcrect.h = dstrect.h = (Uint16)Font->Surface->h - 1;

    for (c = text + (strlen(text) - 1) * charsize; c >= text && x >= startx - w ; c -= charsize) {
		charoffset = ((int) (*c - 33)) * 2 + 1;
		// skip spaces and nonprintable characters
        if (*c == ' ' || charoffset < 0 || charoffset > Font->MaxPos) {
			width = Font->CharPos[2] - Font->CharPos[1];
			x -= width;

			iPrintedWidth += width;
			continue;
		}

		srcrect.w = dstrect.w = (Sint16)
			(((Font->CharPos[charoffset+2] + Font->CharPos[charoffset+1]) >> 1) -
			((Font->CharPos[charoffset] + Font->CharPos[charoffset-1]) >> 1));

		width = Font->CharPos[charoffset+1] - Font->CharPos[charoffset];

		iPrintedWidth += width;

		if (iPrintedWidth > w)
			break;

		x -= width;

		srcrect.x = (Sint16)(Font->CharPos[charoffset] + Font->CharPos[charoffset-1]) >> 1;
		dstrect.x = (short) (x - (float)((Font->CharPos[charoffset]
					- Font->CharPos[charoffset-1]) >> 1)) + x_shake;

		dstrect.y = (Sint16)(y + y_shake);

		//x -= (width - srcrect.w);

		SDL_BlitSurface(Font->Surface, &srcrect, Surface, &dstrect);

    }
}

