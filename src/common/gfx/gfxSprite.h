#ifndef GFX_SPRITE
#define GFX_SPRITE

#include "SDL.h"

#include <string>

class gfxSprite {
public:
    gfxSprite();
    ~gfxSprite();

    void freeSurface();
    void clearSurface();

    bool init(const std::string& filename, Uint8 r, Uint8 g, Uint8 b, bool fUseAccel = true); //color keyed
    bool init(const std::string& filename, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool fUseAccel = true); //color keyed + alpha
    bool init(const std::string& filename); //non color keyed
    bool initskin(const std::string& filename, Uint8 r, Uint8 g, Uint8 b, short colorscheme, bool expand);

    bool draw(short x, short y);
    bool draw(short x, short y, short srcx, short srcy, short w, short h, short iHiddenDirection = -1, short iHiddenValue = -1);
    bool drawStretch(short x, short y, short w, short h, short srcx, short srcy, short srcw, short srch);

    void setalpha(Uint8 alpha);

    int getWidth();
    int getHeight();

    void setSurface(SDL_Surface * surface);
    SDL_Surface *getSurface();

    bool GetWrap();
    void SetWrap(bool wrap);
    void SetWrap(bool wrap, short wrapsize);

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

#endif // GFX_SPRITE
