/*----------------------------------------------+
| gfx							Florian Hufsky	|
|												|
| start:		14.12.2003						|
| last changes:	22.12.2003						|
+----------------------------------------------*/
/*--------------------------------------------------------------+
| gfx															|
|																|
| a stripped down version of tfree/free  for a pixelate article	|
|																|
| gfx is a mini graphics library containing						|
|  + initialisation of SDL										|
|  + a class for simple Sprites									|
|  + a class for fonts											|
|																|
| have a lot of fun!											|
|		(C) 2003-2008 Florian Hufsky <florian.hufsky@gmail.com>	|
+--------------------------------------------------------------*/

#ifndef GFX_H
#define GFX_H

#include "SDL.h"
#include <string>

#include "gfx/gfxSprite.h"
#include "gfx/gfxFont.h"

bool gfx_init(int w, int h, bool fullscreen);
void gfx_changefullscreen(bool fullscreen);
void gfx_flipscreen();
void gfx_settitle(const char*);

void gfx_close();
bool gfx_loadpalette(const std::string& palette_path);

void gfx_setrect(SDL_Rect * rect, short x, short y, short w, short h);
void gfx_setrect(SDL_Rect * rect, SDL_Rect * copyrect);

void gfx_cliprect(SDL_Rect * srcRect, SDL_Rect * dstRect, short x, short y, short w, short h);
bool gfx_adjusthiddenrects(SDL_Rect * srcRect, SDL_Rect * dstRect, short iHiddenDirection, short iHiddenValue);
void gfx_drawpreview(SDL_Surface * surface, short dstX, short dstY, short srcX, short srcY, short iw, short ih, short clipX, short clipY, short clipW, short clipH, bool wrap, short hiddenDirection = -1, short hiddenPlane = -1);

bool gfx_loadfullskin(gfxSprite ** gSprites, const std::string& filename, Uint8 r, Uint8 g, Uint8 b, short colorScheme);
bool gfx_loadmenuskin(gfxSprite ** gSprite, const std::string& filename, Uint8 r, Uint8 g, Uint8 b, short colorScheme, bool fLoadBothDirections);

bool gfx_loadteamcoloredimage(gfxSprite * gSprites, const std::string& filename, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool fVertical, bool fWrap);
//Load image into an array of 4 gfxSprites, each with it's own team color
bool gfx_loadteamcoloredimage(gfxSprite ** gSprites, const std::string& filename, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool fWrap);

//Load image into a single gfxSprite with 4 team colored sprites
bool gfx_loadteamcoloredimage(gfxSprite * gSprites, const std::string& filename, bool fVertical, bool fWrap);
bool gfx_loadteamcoloredimage(gfxSprite * gSprites, const std::string& filename, Uint8 a, bool fVertical, bool fWrap);

bool gfx_loadimagenocolorkey(gfxSprite * gSprite, const std::string& f);
bool gfx_loadimage(gfxSprite * gSprite, const std::string& f, bool fWrap = true, bool fUseAccel = true);
bool gfx_loadimage(gfxSprite * gSprite, const std::string& f, Uint8 alpha, bool fWrap = true, bool fUseAccel = true);
bool gfx_loadimage(gfxSprite * gSprite, const std::string& f, Uint8 r, Uint8 g, Uint8 b, bool fWrap = true, bool fUseAccel = true);

#endif // GFX_H
