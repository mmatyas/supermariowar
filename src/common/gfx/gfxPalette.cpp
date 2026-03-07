#include "gfxPalette.h"

#include "gfx.h"

#include "SDL_image.h"

#include <algorithm>
#include <cstdio>

namespace fs = std::filesystem;


std::optional<RGB> gfxPalette::replacementFor(RGB keyColor, size_t teamIndex, PlayerPalette playerState) const
{
    const auto it = std::ranges::find(m_colorsheets, keyColor, &ColorSheet::key);
    return it != m_colorsheets.cend()
        ? std::make_optional(it->replacementFor(teamIndex, playerState))
        : std::nullopt;
}


std::optional<RGB> gfxPalette::replacementFor(size_t sheetIndex, size_t teamIndex, PlayerPalette playerState) const
{
    return sheetIndex < m_colorsheets.size()
        ? std::make_optional(m_colorsheets[sheetIndex].replacementFor(teamIndex, playerState))
        : std::nullopt;
}


bool gfxPalette::load(const fs::path& path)
{
    m_colorsheets.clear();

    SDL_Surface* surf = IMG_Load(path.string().c_str());
    if (!surf) {
        printf("Couldn't load color palette: %s\n", SDL_GetError());
        return false;
    }

    if (SDL_MUSTLOCK(surf))
        SDL_LockSurface(surf);

    for (int x = 0; x < surf->w; x++) {
        ColorSheet sheet {};
        sheet.key = getRgb(surf, x, 0);

        for (size_t team = 0; team < MAX_PLAYERS; team++) {
            for (size_t playerState = 0; playerState < PlayerPalette::COUNT; playerState++) {
                const size_t relativeY = team * PlayerPalette::COUNT + playerState;
                sheet.replacements[relativeY] = getRgb(surf, x, 1 + relativeY);
            }
        }

        m_colorsheets.emplace_back(std::move(sheet));
    }

    if (SDL_MUSTLOCK(surf))
        SDL_UnlockSurface(surf);

    SDL_FreeSurface(surf);
    return true;
}
