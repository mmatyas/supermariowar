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

#include "gfx/Color.h"
#include "gfx/gfxSprite.h"
#include "gfx/gfxFont.h"

enum PlayerPalette {
    normal,
    invincibility_1,
    invincibility_2,
    invincibility_3,
    shielded,
    tagged,
    ztarred,
    got_shine,
    frozen,
    NUM_PALETTES
};

bool gfx_init(int w, int h, bool fullscreen);
void gfx_changefullscreen(bool fullscreen);
void gfx_flipscreen();
void gfx_settitle(const char*);
void gfx_show_error(const char*);
void gfx_take_screenshot();

void gfx_close();
bool gfx_loadpalette(const std::string& palette_path);

void gfx_cliprect(SDL_Rect * srcRect, SDL_Rect * dstRect, short x, short y, short w, short h);
bool gfx_adjusthiddenrects(SDL_Rect * srcRect, SDL_Rect * dstRect, short iHiddenDirection, short iHiddenValue);
void gfx_drawpreview(SDL_Surface * surface, short dstX, short dstY, short srcX, short srcY, short iw, short ih, short clipX, short clipY, short clipW, short clipH, bool wrap, short hiddenDirection = -1, short hiddenPlane = -1);

bool gfx_loadfullskin(gfxSprite ** gSprites, const std::string& filename, const RGB& colorkey, short colorScheme);
bool gfx_loadmenuskin(gfxSprite ** gSprite, const std::string& filename, const RGB& colorkey, short colorScheme, bool fLoadBothDirections);

bool gfx_loadimagenocolorkey(gfxSprite * gSprite, const std::string& f);
bool gfx_loadimage(gfxSprite& sprite, const std::string& path, bool fWrap = true);
bool gfx_loadimage(gfxSprite& sprite, const std::string& path, Uint8 alpha, bool fWrap = true);
bool gfx_loadimage(gfxSprite& sprite, const std::string& path, const RGB& rgb, bool fWrap = true);

void gfx_setjoystickteamcolor(SDL_Joystick * joystick, unsigned short team, float brightness);


RGB getRgb(SDL_Surface* surf, int x, int y);

#endif // GFX_H
