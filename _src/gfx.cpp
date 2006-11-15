
#include "gfx.h"
#include "global.h"

#include <stdarg.h>		//atexit?
#include <stdlib.h>		//atexit?
#include <iostream>
#include <string>
using namespace std;

#ifdef _WIN32
	#pragma comment(lib, "SDL_image.lib")

	#ifndef _XBOX
		#pragma comment(lib, "SDL.lib")
		#pragma comment(lib, "SDLmain.lib")
	#endif
#endif

extern SDL_Surface *blitdest;
extern SDL_Surface *screen;

#define GFX_BPP		16
#ifdef _XBOX
	#define GFX_FLAGS	SDL_SWSURFACE | SDL_HWACCEL
#else
	#define GFX_FLAGS	SDL_SWSURFACE /*| SDL_HWACCEL*/
#endif

//[colorcomponents][numcolors]
Uint8 * colorcodes[3];

#define NUM_SCHEMES 8

//[numplayers][colorscheme][colorcomponents][numcolors]
Uint8 * colorschemes[4][NUM_SCHEMES][3]; 
short numcolors = 0;

extern short x_shake;
extern short y_shake;

//gfx_init
bool gfx_init(int w, int h, bool fullscreen)
{
	printf("init SDL\n");
	if( SDL_Init(SDL_INIT_VIDEO ) < 0 )
	{
        printf("Couldn't initialize SDL: %s\n", SDL_GetError());
        return false;
    }

    // Clean up on exit
    atexit(SDL_Quit);

    if(fullscreen)
		screen = SDL_SetVideoMode(w, h, GFX_BPP, GFX_FLAGS | SDL_FULLSCREEN);
	else
		screen = SDL_SetVideoMode(w, h, GFX_BPP, GFX_FLAGS);

    if ( screen == NULL ) 
	{
        printf("Couldn't set video mode %dx%d: %s\n", w, h, SDL_GetError());
		return false;
    }

    printf(" running @ %dx%d %dbpp (done)\n", w, h, screen->format->BitsPerPixel);	

	for(int k = 0; k < 3; k++)
	{
		colorcodes[k] = NULL;

		for(int i = 0; i < 4; i++)
			for(int j = 0; j < NUM_SCHEMES; j++)
				colorschemes[i][j][k] = NULL;
	}

	return true;
}

void gfx_freepalette()
{
	for(int k = 0; k < 3; k++)
	{
		delete [] colorcodes[k];

		for(int i = 0; i < 4; i++)
			for(int j = 0; j < NUM_SCHEMES; j++)
				delete [] colorschemes[i][j][k];
	}
}

bool gfx_loadpalette()
{
	gfx_freepalette();

	SDL_Surface * palette = IMG_Load(convertPathCP("gfx/packs/palette.bmp", gamegraphicspacklist.current_name()));

	if ( palette == NULL ) 
	{
        printf("Couldn't load color palette: %s\n", SDL_GetError());
		return false;
    }

	numcolors = (short)palette->w;

	for(int k = 0; k < 3; k++)
	{
		colorcodes[k] = new Uint8[numcolors];

		for(int i = 0; i < 4; i++)
			for(int j = 0; j < NUM_SCHEMES; j++)
				colorschemes[i][j][k] = new Uint8[numcolors];
	}

	if(SDL_MUSTLOCK(palette))
		SDL_LockSurface(palette);

	int counter = 0;

	for(int k = 0; k < numcolors; k++)
	{
		colorcodes[0][k] = ((Uint8*)palette->pixels)[counter++];
		colorcodes[1][k] = ((Uint8*)palette->pixels)[counter++];
		colorcodes[2][k] = ((Uint8*)palette->pixels)[counter++];
	}

	counter += palette->pitch - palette->w * 3;

	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < NUM_SCHEMES; j++)
		{
			for(int m = 0; m < numcolors; m++)
			{
#if (SDL_BYTEORDER==SDL_BIG_ENDIAN)
				colorschemes[i][j][2][m] = ((Uint8*)palette->pixels)[counter++];
				colorschemes[i][j][1][m] = ((Uint8*)palette->pixels)[counter++];
				colorschemes[i][j][0][m] = ((Uint8*)palette->pixels)[counter++];
#else
				colorschemes[i][j][0][m] = ((Uint8*)palette->pixels)[counter++];
				colorschemes[i][j][1][m] = ((Uint8*)palette->pixels)[counter++];
				colorschemes[i][j][2][m] = ((Uint8*)palette->pixels)[counter++];
#endif
			}

			counter += palette->pitch - palette->w * 3;
		}
	}
	
    if (SDL_MUSTLOCK(palette))
        SDL_UnlockSurface(palette);

	SDL_FreeSurface(palette);

	return true;
}

void gfx_setresolution(int w, int h, bool fullscreen)
{
	Uint32 flags = GFX_FLAGS;
	if(fullscreen)
		flags |= SDL_FULLSCREEN;

#ifdef _XBOX

	if(game_values.aspectratio10x11)
		flags |= SDL_10X11PIXELASPECTRATIO;

	screen = SDL_SetVideoModeWithFlickerFilter(w, h, GFX_BPP, flags, game_values.flickerfilter, game_values.softfilter);

#else
    screen = SDL_SetVideoMode(w, h, GFX_BPP, flags);
#endif
}

void gfx_close()
{
	for(int i = 0; i < 3; i++)
	{
		delete [] colorcodes[i];

		for(int j = 0; j < 4; j++)
			for(int k = 0; k < NUM_SCHEMES; k++)
				delete [] colorschemes[j][k][i];
	}
}


bool ValidSkinSurface(SDL_Surface * skin)
{
	if(skin->w == 192 && skin->h == 32 && skin->format->BitsPerPixel == 24)
		return true;

	return false;
}

SDL_Surface * gfx_createskinsurface(SDL_Surface * skin, short spriteindex, Uint8 r, Uint8 g, Uint8 b, short colorScheme, bool expand, bool reverse)
{
	int loops = 1;
	if(expand)
		loops = NUM_SCHEMES;

	//Blit over loaded skin into player image set
	SDL_Surface * temp = SDL_CreateRGBSurface(skin->flags, 32 * loops, 32, skin->format->BitsPerPixel, 0, 0, 0, 0);
	
	//Take the loaded skin and colorize it for each state (normal, 3 frames of invincibiliy, shielded, tagged, ztarred. got shine)
	if(SDL_MUSTLOCK(temp))
		SDL_LockSurface(temp);

	if(SDL_MUSTLOCK(skin))
		SDL_LockSurface(skin);

	int skincounter = spriteindex * 96;
	int tempcounter = 0;

	int reverseoffset = 0;

	for(int j = 0; j < 32; j++)
	{
		for(int i = 0; i < 32; i++)
		{
			if(reverse)
				reverseoffset = (31 - (i * 2)) * 3;

			bool fFoundColor = false;
			for(int m = 0; m < numcolors; m++)
			{
				if( ((Uint8*)skin->pixels)[skincounter] == colorcodes[0][m] && 
					((Uint8*)skin->pixels)[skincounter + 1] == colorcodes[1][m] &&
					((Uint8*)skin->pixels)[skincounter + 2] == colorcodes[2][m])
				{
					for(int k = 0; k < loops; k++)
					{
						((Uint8*)temp->pixels)[tempcounter + k * 96 + reverseoffset] = colorschemes[colorScheme][k][0][m];
						((Uint8*)temp->pixels)[tempcounter + k * 96 + reverseoffset + 1] = colorschemes[colorScheme][k][1][m];
						((Uint8*)temp->pixels)[tempcounter + k * 96 + reverseoffset + 2] = colorschemes[colorScheme][k][2][m];
					}
					
					fFoundColor = true;
					break;
				}
			}

			if(!fFoundColor)
			{
				for(int k = 0; k < loops; k++)
				{
					((Uint8*)temp->pixels)[tempcounter + k * 96 + reverseoffset] = ((Uint8*)skin->pixels)[skincounter];
					((Uint8*)temp->pixels)[tempcounter + k * 96 + reverseoffset + 1] = ((Uint8*)skin->pixels)[skincounter + 1];
					((Uint8*)temp->pixels)[tempcounter + k * 96 + reverseoffset + 2] = ((Uint8*)skin->pixels)[skincounter + 2];
				}
			}

			skincounter += 3;
			tempcounter += 3;
		}

		skincounter += 480 + skin->pitch - (skin->w * 3);
		tempcounter += 96 * (loops - 1) + temp->pitch - (temp->w * 3);
	}

	SDL_UnlockSurface(skin);
	SDL_UnlockSurface(temp);

	if( SDL_SetColorKey(temp, SDL_SRCCOLORKEY | SDL_RLEACCEL, SDL_MapRGB(temp->format, r, g, b)) < 0)
	{
		printf("\n ERROR: Couldn't set ColorKey + RLE for new skin surface: %s\n", SDL_GetError());
		return NULL;
	}

	SDL_Surface * final = SDL_DisplayFormat(temp);
	if(!final)
	{
		printf("\n ERROR: Couldn't create new surface using SDL_DisplayFormat(): %s\n", SDL_GetError());
		return NULL;
	}
	SDL_FreeSurface(temp);

	return final;
}


bool gfx_createmenuskin(gfxSprite ** gSprite, const std::string& filename, Uint8 r, Uint8 g, Uint8 b, short colorScheme)
{
    // Load the BMP file into a surface
	SDL_Surface * skin = IMG_Load(filename.c_str());

	if(!ValidSkinSurface(skin))
		return false;

    if (skin == NULL)
	{
        cout << endl << " ERROR: Couldn't load " << filename << ": "
             << SDL_GetError() << endl;
        return false;
    }

	for(short iSprite = 0; iSprite < 2; iSprite++)
	{
		SDL_Surface * skinSurface = gfx_createskinsurface(skin, iSprite, r, g, b, colorScheme, true, false);

		if (skinSurface == NULL)
		{
			cout << endl << " ERROR: Couldn't create menu skin from " << filename
				<< ": " << SDL_GetError() << endl;

			return false;
		}

		gSprite[iSprite * 2]->setSurface(skinSurface);
	}

	SDL_FreeSurface(skin);

	return true;
}


bool gfx_createfullskin(gfxSprite ** gSprites, const std::string& filename, Uint8 r, Uint8 g, Uint8 b, short colorScheme)
{
    // Load the BMP file into a surface
	SDL_Surface * skin = IMG_Load(filename.c_str());

	if(!ValidSkinSurface(skin))
		return false;

    if (skin == NULL)
	{
        cout << endl << " ERROR: Couldn't load " << filename
             << ": " << SDL_GetError() << endl;
        return false;
    }

	for(short k = 0; k < 4; k++)
	{
		for(short j = 0; j < 2; j++)
		{
			SDL_Surface * skinSurface = gfx_createskinsurface(skin, k, r, g, b, colorScheme, true, j != 0);

			if (skinSurface == NULL)
			{
                cout << endl << " ERROR: Couldn't create menu skin from "
                     << filename << ": " << SDL_GetError() << endl;
				SDL_FreeSurface(skin);
				return false;
			}

			gSprites[(k * 2) + j]->setSurface(skinSurface);
		}
	}

	//Dead Flying Sprite
	SDL_Surface * skinSurface = gfx_createskinsurface(skin, 4, r, g, b, colorScheme, true, false);

	if (skinSurface == NULL)
	{
        cout << endl << " ERROR: Couldn't create menu skin from "
             << filename << ": " << SDL_GetError() << endl;
		SDL_FreeSurface(skin);
		return false;
	}

	gSprites[8]->setSurface(skinSurface);

	//Dead Stomped Sprite
	skinSurface = gfx_createskinsurface(skin, 5, r, g, b, colorScheme, true, false);

	if (skinSurface == NULL)
	{
        cout << endl << " ERROR: Couldn't create menu skin from "
             << filename << ": " << SDL_GetError() << endl;
		return false;
	}

	gSprites[9]->setSurface(skinSurface);

	SDL_FreeSurface(skin);
	
	return true;
}

//gfxSprite

gfxSprite::gfxSprite()
{
	m_bltrect.x = 0;
	m_bltrect.y = 0;
	m_bltrect.w = 0;
	m_bltrect.h = 0;
	m_picture = NULL;

	fHiddenPlane = false;
	iHiddenDirection = 0;
	iHiddenValue = 0;

	fWrap = false;
}

gfxSprite::~gfxSprite()
{
	//free the allocated BMP surface 
	freeSurface();
}

bool gfxSprite::init(const std::string& filename, Uint8 r, Uint8 g, Uint8 b)
{
    cout << "loading sprite " << filename << "...";

	if(m_picture)
	{
		SDL_FreeSurface(m_picture);
		m_picture = NULL;
	}

    // Load the BMP file into a surface
	m_picture = IMG_Load(filename.c_str());

    if (!m_picture)
	{
        cout << endl << " ERROR: Couldn't load " << filename << ": " << SDL_GetError() << endl;
        return false;
    }

	if( SDL_SetColorKey(m_picture, SDL_SRCCOLORKEY | SDL_RLEACCEL, SDL_MapRGB(m_picture->format, r, g, b)) < 0)
	{
        cout << endl << " ERROR: Couldn't set ColorKey + RLE for "
             << filename << ": " << SDL_GetError() << endl;
		return false;
	}

	SDL_Surface *temp = SDL_DisplayFormat(m_picture);
	if(!temp)
	{
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

bool gfxSprite::init(const std::string& filename, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    cout << "Loading sprite " << filename << " ...";

	if(m_picture)
	{
		SDL_FreeSurface(m_picture);
		m_picture = NULL;
	}

    // Load the BMP file into a surface
	m_picture = IMG_Load(filename.c_str());

    if (!m_picture)
	{
        cout << endl << " ERROR: Couldn't load "
             << filename << ": " << SDL_GetError() << endl;
        return false;
    }

	if( SDL_SetColorKey(m_picture, SDL_SRCCOLORKEY | SDL_RLEACCEL, SDL_MapRGB(m_picture->format, r, g, b)) < 0)
	{
        cout << endl << " ERROR: Couldn't set ColorKey + RLE for "
             << filename << ": " << SDL_GetError() << endl;
		return false;
	}

	if( (SDL_SetAlpha(m_picture, SDL_SRCALPHA | SDL_RLEACCEL, a)) < 0)
	{
        cout << endl << " ERROR: Couldn't set per-surface alpha on "
             << filename << ": " << SDL_GetError() << endl;
		return false;
	}
	
	SDL_Surface *temp = SDL_DisplayFormatAlpha(m_picture);
	if(!temp)
	{
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

bool gfxSprite::init(const std::string& filename) 
{
    cout << "loading sprite " << filename << "...";

	if(m_picture)
	{
		SDL_FreeSurface(m_picture);
		m_picture = NULL;
	}

    // Load the BMP file into a surface
	m_picture = IMG_Load(filename.c_str());

    if (!m_picture)
	{
        cout << endl << " ERROR: Couldn't load "
             << filename << ": " << SDL_GetError() << endl;
        return false;
    }

	SDL_Surface *temp = SDL_DisplayFormat(m_picture);
	if(!temp)
	{
        cout << endl << " ERROR: Couldn't convert "
             << filename << " to the display's pixel format: " << SDL_GetError()
             << endl;
		return false;
	}

	SDL_FreeSurface(m_picture);
	m_picture = temp;

	m_bltrect.w = (Uint16)m_picture->w;
	m_bltrect.h = (Uint16)m_picture->h;

	m_srcrect.x = 0;
	m_srcrect.y = 0;
	m_srcrect.w = (Uint16)m_picture->w;
	m_srcrect.h = (Uint16)m_picture->h;

    cout << "done" << endl;
	return true;
}


bool gfxSprite::draw(short x, short y)
{
	m_bltrect.x = x + x_shake;
	m_bltrect.y = y + y_shake;

	if(SDL_BlitSurface(m_picture, NULL, blitdest, &m_bltrect) < 0)
	{
		fprintf(stderr, "BlitSurface error: %s\n", SDL_GetError());
		return false;
	}

	if(fWrap)
	{
		if(x + m_picture->w >= 640)
		{
			m_bltrect.x = x - 640 + x_shake;
			m_bltrect.y = y + y_shake;

			if(SDL_BlitSurface(m_picture, NULL, blitdest, &m_bltrect) < 0)
			{
				fprintf(stderr, "SDL_BlitSurface error: %s\n", SDL_GetError());
				return false;
			}
		}
		else if(x < 0)
		{
			m_bltrect.x = x + 640 + x_shake;
			m_bltrect.y = y + y_shake;

			if(SDL_BlitSurface(m_picture, NULL, blitdest, &m_bltrect) < 0)
			{
				fprintf(stderr, "SDL_BlitSurface error: %s\n", SDL_GetError());
				return false;
			}
		}
	}

	return true;
}

//TODO Perf Optimization: Set w/h once when sprite is initialized, set srcx/srcy just when animation frame advance happens
bool gfxSprite::draw(short x, short y, short srcx, short srcy, short w, short h, short iHiddenDirection, short iHiddenValue)
{
	m_bltrect.x = x + x_shake;
	m_bltrect.y = y + y_shake;
	m_bltrect.w = w;
	m_bltrect.h = h;

	m_srcrect.x = srcx;
	m_srcrect.y = srcy;
	m_srcrect.w = w;
	m_srcrect.h = h;

	if(iHiddenDirection > -1)
	{
		if(AdjustHiddenRects(x, y, srcx, srcy, w, h, iHiddenDirection, iHiddenValue))
			return true;
	}

	// Blit onto the screen surface
	if(SDL_BlitSurface(m_picture, &m_srcrect, blitdest, &m_bltrect) < 0)
	{
		fprintf(stderr, "SDL_BlitSurface error: %s\n", SDL_GetError());
		return false;
	}

	if(fWrap)
	{
		if(x + w >= 640)
		{
			m_bltrect.x = x - 640 + x_shake;
			m_bltrect.y = y + y_shake;

			if(iHiddenDirection > -1)
			{
				if(AdjustHiddenRects(x, y, srcx, srcy, w, h, iHiddenDirection, iHiddenValue))
					return true;
			}

			if(SDL_BlitSurface(m_picture, &m_srcrect, blitdest, &m_bltrect) < 0)
			{
				fprintf(stderr, "SDL_BlitSurface error: %s\n", SDL_GetError());
				return false;
			}
		}
		else if(x < 0)
		{
			m_bltrect.x = x + 640 + x_shake;
			m_bltrect.y = y + y_shake;

			if(iHiddenDirection > -1)
			{
				if(AdjustHiddenRects(x, y, srcx, srcy, w, h, iHiddenDirection, iHiddenValue))
					return true;
			}

			if(SDL_BlitSurface(m_picture, &m_srcrect, blitdest, &m_bltrect) < 0)
			{
				fprintf(stderr, "SDL_BlitSurface error: %s\n", SDL_GetError());
				return false;
			}
		}
	}

	return true;
}

bool gfxSprite::AdjustHiddenRects(short x, short y, short srcx, short srcy, short w, short h, short iHiddenDirection, short iHiddenValue)
{
	if(iHiddenDirection == 0)
	{
		if(y <= iHiddenValue)
		{
			if(y + h <= iHiddenValue)
				return true;

			m_bltrect.y = iHiddenValue + y_shake;
			m_bltrect.h = h - iHiddenValue + y;
			m_srcrect.y = srcy + iHiddenValue - y;
			m_srcrect.h = m_bltrect.h;
		}
	}
	else if(iHiddenDirection == 1)
	{
		if(x + w >= iHiddenValue)
		{
			if(x >= iHiddenValue)
				return true;

			m_bltrect.w = iHiddenValue - x;
			m_srcrect.w = m_bltrect.w;
		}
	}
	else if(iHiddenDirection == 2)
	{
		if(y + h >= iHiddenValue)
		{
			if(y >= iHiddenValue)
				return true;

			m_bltrect.h = iHiddenValue - y;
			m_srcrect.h = m_bltrect.h;
		}
	}
	else if(iHiddenDirection == 3)
	{
		if(x <= iHiddenValue)
		{
			if(x + w <= iHiddenValue)
				return true;

			m_bltrect.x = iHiddenValue + x_shake;
			m_bltrect.w = w - iHiddenValue + x;
			m_srcrect.x = srcx + iHiddenValue - x;
			m_srcrect.w = m_bltrect.w;
		}
	}

	return false;
}

bool gfxSprite::drawStretch(short x, short y, short w, short h, short srcx, short srcy, short srcw, short srch)
{
	m_bltrect.x = x + x_shake;
	m_bltrect.y = y + y_shake;
	m_bltrect.w = w;
	m_bltrect.h = h;

	m_srcrect.x = srcx;
	m_srcrect.y = srcy;
	m_srcrect.w = srcw;
	m_srcrect.h = srch;

	// Looks like SoftStretch doesn't respect transparent colors
	// I need to look into the actual SDL code to see if I can fix this
	if(SDL_SoftStretch(m_picture, &m_srcrect, blitdest, &m_bltrect) < 0)
	{
		fprintf(stderr, "SDL_SoftStretch error: %s\n", SDL_GetError());
		return false;
	}

	return true;
}

void gfxSprite::setalpha(Uint8 alpha)
{
	if( (SDL_SetAlpha(m_picture, SDL_SRCALPHA | SDL_RLEACCEL, alpha)) < 0)
	{
		printf("\n ERROR: couldn't set alpha on sprite: %s\n", SDL_GetError());
	}
}

void gfxSprite::freeSurface()
{
	if(m_picture)
	{
		SDL_FreeSurface(m_picture);
		m_picture = NULL;
	}
}


// gfxFont

gfxFont::gfxFont()
{
	m_font = NULL;
}

gfxFont::~gfxFont()
{
	if(m_font)
		SFont_FreeFont(m_font);
};

bool gfxFont::init(const std::string& filename)
{
	if(m_font)
		SFont_FreeFont(m_font);

	cout << "loading font " << filename << " ... ";

	SDL_Surface *fontsurf = IMG_Load(filename.c_str());
	if(fontsurf == NULL)
	{
        cout << endl << " ERROR: Couldn't load file "
             << filename << ": " << SDL_GetError() << endl;
		return false;
	}

	m_font = SFont_InitFont(fontsurf);
	if(!m_font)
	{
        cout << endl << " ERROR: an error occurre while loading the font." << endl;
		return false;
	}

    cout << "done" << endl;
	return true;
}

void gfxFont::draw(int x, int y, const std::string& s)
{
	//if(y + getHeight() < 0)
	//	return;

	SFont_Write(blitdest, m_font, x, y, s.c_str());
}

void gfxFont::drawChopRight(int x, int y, int width, const char *s)
{
	//if(y + getHeight() < 0)
	//	return;
		
	SFont_WriteChopRight(blitdest, m_font, x, y, width, s);
}

void gfxFont::drawCentered(int x, int y, const char *text)
{
	//if(y + getHeight() < 0)
	//	return;
		
	SFont_WriteCenter(blitdest, m_font, x, y, text);
};

void gfxFont::drawChopCentered(int x, int y, int width, const char *text)
{
	//if(y + getHeight() < 0)
	//	return;
		
	SFont_WriteChopCenter(blitdest, m_font, x, y, width, text);
};

void gfxFont::drawRightJustified(int x, int y, const char *s, ...)
{
	char buffer[256];

	va_list zeiger;
	va_start(zeiger, s);
	vsprintf(buffer, s, zeiger);
	va_end(zeiger);

	//if(y + getHeight() < 0)
	//	return;
		
	SFont_WriteRight(blitdest, m_font, x, y, buffer);
};


void gfxFont::drawf(int x, int y, char *s, ...)
{
	char buffer[256];

	va_list zeiger;
	va_start(zeiger, s);
	vsprintf(buffer, s, zeiger);
	va_end(zeiger);

	draw(x,y,buffer);
}

void gfxFont::setalpha(Uint8 alpha)
{
	if( (SDL_SetAlpha(m_font->Surface, SDL_SRCALPHA | SDL_RLEACCEL, alpha)) < 0)
	{
		printf("\n ERROR: couldn't set alpha on sprite: %s\n", SDL_GetError());
	}
}

