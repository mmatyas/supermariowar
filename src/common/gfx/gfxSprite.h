#ifndef GFX_SPRITE
#define GFX_SPRITE

#include "gfx/Color.h"
#include "SDL.h"

#include <string>

class gfxSprite {
public:
    gfxSprite();
    ~gfxSprite();

    void freeSurface();
    void clearSurface();

    bool init(const std::string& filename, const RGB& rgb, bool fUseAccel = true); //color keyed
    bool init(const std::string& filename, const RGBA& rgba, bool fUseAccel = true); //color keyed + alpha
    bool init(const std::string& filename); //non color keyed
    bool initskin(const std::string& filename, const RGB& rgb, short colorscheme, bool expand);

    bool draw(short x, short y);
    bool draw(short x, short y, short srcx, short srcy, short w, short h, short iHiddenDirection = -1, short iHiddenValue = -1);
    bool drawStretch(short x, short y, short w, short h, short srcx, short srcy, short srcw, short srch);

    void setalpha(Uint8 alpha);

    int getWidth();
    int getHeight();

    void setSurface(SDL_Surface* surface);
    SDL_Surface* getSurface() const { return m_picture; }

    bool GetWrap();
    void SetWrap(bool wrap);
    void SetWrap(bool wrap, short wrapsize);

private:
    SDL_Surface *m_picture;
    SDL_Rect m_bltrect;

    bool fWrap;
    short iWrapSize;
};

#endif // GFX_SPRITE
