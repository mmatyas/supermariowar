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
|		  © 2003-2008 Florian Hufsky <florian.hufsky@gmail.com>	|
+--------------------------------------------------------------*/

#ifndef __GFX_H__
#define __GFX_H__

#include "SDL_image.h"
#include "SDL.h"
#include "SFont.h"
#include <string>

bool gfx_init(int w, int h, bool fullscreen);
void gfx_setresolution(int w, int h, bool fullscreen);

void gfx_close();
bool gfx_loadpalette();

void gfx_setrect(SDL_Rect * rect, short x, short y, short w, short h);
void gfx_setrect(SDL_Rect * rect, SDL_Rect * copyrect);

void gfx_cliprect(SDL_Rect * srcRect, SDL_Rect * dstRect, short x, short y, short w, short h);
bool gfx_adjusthiddenrects(SDL_Rect * srcRect, SDL_Rect * dstRect, short iHiddenDirection, short iHiddenValue);
void gfx_drawpreview(SDL_Surface * surface, short dstX, short dstY, short srcX, short srcY, short iw, short ih, short clipX, short clipY, short clipW, short clipH, bool wrap, short hiddenDirection = -1, short hiddenPlane = -1);

class gfxSprite
{
	public:
		gfxSprite();
		~gfxSprite();

		void clearSurface();

		bool init(const std::string& filename, Uint8 r, Uint8 g, Uint8 b, bool fUseAccel = true); //color keyed
		bool init(const std::string& filename, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool fUseAccel = true);	//color keyed + alpha
		bool init(const std::string& filename);							//non color keyed
		bool initskin(const std::string& filename, Uint8 r, Uint8 g, Uint8 b, short colorscheme, bool expand);

		bool draw(short x, short y);
		bool draw(short x, short y, short srcx, short srcy, short w, short h, short iHiddenDirection = -1, short iHiddenValue = -1);
		bool drawStretch(short x, short y, short w, short h, short srcx, short srcy, short srcw, short srch);

		void setalpha(Uint8 alpha);

		int getWidth(){return m_picture->w;}
		int getHeight(){return m_picture->h;}

		SDL_Surface *getSurface(){return m_picture;}
		
		void setSurface(SDL_Surface * surface)
		{
			freeSurface();
			m_picture = surface;
			m_bltrect.w = (Uint16)m_picture->w; 
			m_bltrect.h = (Uint16)m_picture->h;
		}

		void freeSurface();

		bool GetWrap() {return fWrap;}
		void SetWrap(bool wrap) {fWrap = wrap;}
		void SetWrap(bool wrap, short wrapsize) {fWrap = wrap; iWrapSize = wrapsize;}

	private:
		SDL_Surface *m_picture;
		SDL_Rect m_bltrect;
		SDL_Rect m_srcrect;

		bool fHiddenPlane;
		short iHiddenDirection;
		short iHiddenValue;

		bool fWrap;
		short iWrapSize;
};


class gfxFont
{
	public:
		gfxFont();
		~gfxFont();

		bool init(const std::string& filename);
		void draw(int x, int y, const std::string& s);
		void drawf(int x, int y, const char *s, ...);

		void drawCentered(int x, int y, const char *text);
		void drawChopCentered(int x, int y, int width, const char *text);
		void drawRightJustified(int x, int y, const char *s, ...);
		void drawChopRight(int x, int y, int width, const char *s);
		void drawChopLeft(int x, int y, int width, const char *s);

		void setalpha(Uint8 alpha);

		int getHeight(){return SFont_TextHeight(m_font);};
		int getWidth(const char *text){return SFont_TextWidth(m_font, text);};

	private:
		SFont_Font *m_font;
};

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

#endif //__GFX_H__

