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
|  + a class for fonts								|
|																|
| have a lot of fun!											|
|					© 2003 Florian Hufsky <fhufsky@phorus.at>	|
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

class gfxSprite
{
	public:
		gfxSprite();
		~gfxSprite();

		bool init(const std::string& filename, Uint8 r, Uint8 g, Uint8 b); //color keyed
		bool init(const std::string& filename, Uint8 r, Uint8 g, Uint8 b, Uint8 a);	//color keyed + alpha
		bool init(const std::string& filename);							//non color keyed
		bool initskin(const std::string& filename, Uint8 r, Uint8 g, Uint8 b, short colorscheme, bool expand);

		bool draw(short x, short y);
		bool draw(short x, short y, short srcx, short srcy, short w, short h, short iHiddenDirection = -1, short iHiddenValue = -1);
		bool drawStretch(short x, short y, short w, short h, short srcx, short srcy, short srcw, short srch);

		void setalpha(Uint8 alpha);

		int getWidth(){return m_picture->w;};
		int getHeight(){return m_picture->h;};

		SDL_Surface *getSurface(){return m_picture;};
		
		void setSurface(SDL_Surface * surface)
		{
			freeSurface();
			m_picture = surface;
			m_bltrect.w = (Uint16)m_picture->w; 
			m_bltrect.h = (Uint16)m_picture->h;
		}

		void freeSurface();

		void SetWrap(bool wrap) {fWrap = wrap;}
		bool AdjustHiddenRects(short x, short y, short srcx, short srcy, short w, short h, short iHiddenDirection, short iHiddenValue);

	private:
		SDL_Surface *m_picture;
		SDL_Rect m_bltrect;
		SDL_Rect m_srcrect;

		bool fHiddenPlane;
		short iHiddenDirection;
		short iHiddenValue;

		bool fWrap;
};


class gfxFont
{
	public:
		gfxFont();
		~gfxFont();

		bool init(const std::string& filename);
		void draw(int x, int y, const std::string& s);
		void drawf(int x, int y, char *s, ...);

		void drawCentered(int x, int y, const char *text);
		void drawChopCentered(int x, int y, int width, const char *text);
		void drawRightJustified(int x, int y, const char *s, ...);
		void drawChopRight(int x, int y, int width, const char *s);

		void setalpha(Uint8 alpha);

		int getHeight(){return SFont_TextHeight(m_font);};
		int getWidth(const char *text){return SFont_TextWidth(m_font, text);};

	private:
		SFont_Font *m_font;
};

#endif

