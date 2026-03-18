#ifndef GFX_SPRITE
#define GFX_SPRITE

#include "gfx/Color.h"
#include "util/SdlHelpers.h"

#include "SDL.h"

#include <filesystem>

class gfxSprite {
public:
    bool init(const std::filesystem::path& filename); //non color keyed
    bool init(const std::filesystem::path& filename, const RGB& key); //color keyed
    bool init(const std::filesystem::path& filename, const RGB& key, Uint8 alpha); //color keyed + alpha

    /// Draw the whole sprite at the given coordinate.
    void draw(int x, int y) const;
    bool draw(short x, short y, short srcx, short srcy, short w, short h, short iHiddenDirection = -1, short iHiddenValue = -1);
    bool drawStretch(short x, short y, short w, short h, short srcx, short srcy, short srcw, short srch);

    void setalpha(Uint8 alpha);

    int getWidth() const { return m_picture->w; }
    int getHeight() const { return m_picture->h; }

    void setSurface(SdlSurfacePtr surface);
    SDL_Surface* getSurface() const { return m_picture.get(); }

    bool GetWrap() const { return fWrap; }
    void SetWrap(bool wrap, short wrapsize = 640);

private:
    SdlSurfacePtr m_picture;

    bool fWrap = false;
    short iWrapSize = 640;  // TODO: Get it from a global setting.
};

#endif // GFX_SPRITE
