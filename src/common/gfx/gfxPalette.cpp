#include "gfxPalette.h"

#include "SDL_image.h"

#include <cassert>
#include <cstdio>


namespace {
Uint32 getRawPixel(SDL_Surface* surf, int x, int y)
{
    assert(surf);

    const Uint8 bpp = surf->format->BytesPerPixel;
    const size_t idx = y * surf->pitch + x * bpp;
    const auto* pixel8 = static_cast<Uint8*>(surf->pixels) + idx;

    switch (bpp) {
        case 1: return *pixel8;
        case 2: return *pixel8;
        case 3: return (SDL_BYTEORDER == SDL_BIG_ENDIAN)
            ? pixel8[0] << 16 | pixel8[1] << 8 | pixel8[2]
            : pixel8[0] | pixel8[1] << 8 | pixel8[2] << 16;
        case 4: return *pixel8;
        default: return 0x0;
    }
}


RGB getRgb(SDL_Surface* surf, int x, int y)
{
    assert(surf);
    const Uint32 rawPixel = getRawPixel(surf, x, y);
    RGB color;
    SDL_GetRGB(rawPixel, surf->format, &color.r, &color.g, &color.b);
    return color;
}
} // namespace


void gfxPalette::clear()
{
    m_colorCodes.clear();

    for (size_t team = 0; team < m_teamSchemes.size(); team++) {
        for (size_t row = 0; row < PlayerPalette::NUM_PALETTES; row++) {
            m_teamSchemes[team][row].clear();
        }
    }
}


bool gfxPalette::matchesColorAtID(size_t idx, uint8_t r, uint8_t g, uint8_t b) const
{
    assert(idx < m_colorCount);
    return r == m_colorCodes[idx].r && g == m_colorCodes[idx].g && b == m_colorCodes[idx].b;
}


void gfxPalette::copyColorSchemeTo(
    size_t teamID, size_t schemeID, size_t colorID,
    uint8_t& r, uint8_t& g, uint8_t& b) const
{
    const RGB& color = m_teamSchemes.at(teamID).at(schemeID).at(colorID);
    r = color.r;
    g = color.g;
    b = color.b;
}


bool gfxPalette::load(const std::string& palettePath)
{
    clear();

    SDL_Surface* surf = IMG_Load(palettePath.c_str());
    if (!surf) {
        printf("Couldn't load color palette: %s\n", SDL_GetError());
        return false;
    }

    if (SDL_MUSTLOCK(surf))
        SDL_LockSurface(surf);

    m_colorCount = surf->w;

    m_colorCodes.reserve(m_colorCount);
    for (int x = 0; x < surf->w; x++) {
        m_colorCodes.emplace_back(getRgb(surf, x, 0));
    }

    for (size_t team = 0; team < m_teamSchemes.size(); team++) {
        for (size_t palette = 0; palette < PlayerPalette::NUM_PALETTES; palette++) {
            m_teamSchemes[team][palette].reserve(m_colorCount);
            for (int x = 0; x < surf->w; x++) {
                const size_t y = 1 + team * PlayerPalette::NUM_PALETTES + palette;
                m_teamSchemes[team][palette].emplace_back(getRgb(surf, x, y));
            }
        }
    }

    if (SDL_MUSTLOCK(surf))
        SDL_UnlockSurface(surf);

    SDL_FreeSurface(surf);
    return true;
}
