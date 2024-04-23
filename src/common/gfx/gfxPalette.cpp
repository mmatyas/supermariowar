#include "gfxPalette.h"

#include "SDL_image.h"

#include <cstdio>


void gfxPalette::clear()
{
    m_colorCodes.clear();

    for (size_t team = 0; team < m_teamSchemes.size(); team++) {
        for (size_t row = 0; row < PlayerPalette::NUM_PALETTES; row++) {
            m_teamSchemes[team][row].clear();
        }
    }
}


const RGB& gfxPalette::colorScheme(size_t teamID, size_t schemeID, size_t colorID) const
{
    return m_teamSchemes.at(teamID).at(schemeID).at(colorID);
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
