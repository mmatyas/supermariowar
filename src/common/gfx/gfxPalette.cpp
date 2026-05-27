#include "gfxPalette.h"

#include "gfx.h"

#include <SDL3_image/SDL_image.h>

#include <cstdio>

namespace fs = std::filesystem;


std::optional<RGB> gfxPalette::replacementFor(RGB keyColor, size_t teamIndex, PlayerPalette playerState) const
{
    const auto it = m_colorsheets.find(keyColor);
    return it != m_colorsheets.cend()
        ? std::make_optional(it->second.replacementFor(teamIndex, playerState))
        : std::nullopt;
}


bool gfxPalette::load(const fs::path& path)
{
    m_colorsheets.clear();

    const gfxSprite sprite = ImageLoader(path).withoutColorKey().withoutOptimization().create();

    if (SDL_MUSTLOCK(sprite.getSurface()))
        SDL_LockSurface(sprite.getSurface());

    for (int x = 0; x < sprite.getWidth(); x++) {
        RGB key = getRgb(sprite.getSurface(), x, 0);

        ColorSheet sheet {};
        for (size_t team = 0; team < MAX_PLAYERS; team++) {
            for (size_t playerState = 0; playerState < PlayerPalette::COUNT; playerState++) {
                const size_t relativeY = team * PlayerPalette::COUNT + playerState;
                sheet.replacements[relativeY] = getRgb(sprite.getSurface(), x, 1 + relativeY);
            }
        }

        m_colorsheets.emplace(std::move(key), std::move(sheet));
    }

    if (SDL_MUSTLOCK(sprite.getSurface()))
        SDL_UnlockSurface(sprite.getSurface());

    return true;
}
