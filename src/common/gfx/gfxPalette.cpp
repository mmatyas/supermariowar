#include "gfxPalette.h"

#include "gfx.h"
#include "gfx/TeamColors.h"

#include <format>
#include <vector>

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

    // The palette image contains the colors of the 4 base teams;
    // the colors of the other teams are generated from them
    const int baseRows = 1 + NUM_BASE_TEAM_COLORS * PlayerPalette::COUNT;
    if (sprite.getHeight() < baseRows)
        throw std::format("The palette {} has too few rows", path.string());

    auto teamColor = [&](int x, size_t team, size_t playerState) {
        return getRgb(sprite.getSurface(), x, 1 + team * PlayerPalette::COUNT + playerState);
    };

    // Overall hue of each base team (from the team specific colors of the normal state)
    std::array<float, NUM_BASE_TEAM_COLORS> baseHues {};
    for (size_t team = 0; team < NUM_BASE_TEAM_COLORS; team++) {
        std::vector<RGB> samples;
        for (int x = 0; x < sprite.getWidth(); x++) {
            const RGB color = teamColor(x, team, PlayerPalette::normal);
            for (size_t other = 0; other < NUM_BASE_TEAM_COLORS; other++) {
                if (teamColor(x, other, PlayerPalette::normal) != color) {
                    samples.push_back(color);
                    break;
                }
            }
        }
        baseHues[team] = teamcolors::dominantHue(samples);
    }

    for (int x = 0; x < sprite.getWidth(); x++) {
        RGB key = getRgb(sprite.getSurface(), x, 0);

        ColorSheet sheet {};
        for (size_t playerState = 0; playerState < PlayerPalette::COUNT; playerState++) {
            std::array<RGB, NUM_BASE_TEAM_COLORS> baseColors {};
            for (size_t team = 0; team < NUM_BASE_TEAM_COLORS; team++) {
                baseColors[team] = teamColor(x, team, playerState);
                sheet.replacements[team * PlayerPalette::COUNT + playerState] = baseColors[team];
            }

            const auto generated = teamcolors::generatePaletteColors(baseColors, baseHues);
            for (size_t i = 0; i < generated.size(); i++) {
                const size_t team = NUM_BASE_TEAM_COLORS + i;
                sheet.replacements[team * PlayerPalette::COUNT + playerState] = generated[i];
            }
        }

        m_colorsheets.emplace(std::move(key), std::move(sheet));
    }

    if (SDL_MUSTLOCK(sprite.getSurface()))
        SDL_UnlockSurface(sprite.getSurface());

    return true;
}
