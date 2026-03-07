#pragma once

#include "Color.h"
#include "GlobalConstants.h"

#include <array>
#include <filesystem>
#include <optional>
#include <vector>


enum PlayerPalette {
    normal,
    invincibility_1,
    invincibility_2,
    invincibility_3,
    shielded,
    tagged,
    ztarred,
    got_shine,
    frozen,
    COUNT,
};


struct ColorSheet {
    RGB key;
    std::array<RGB, MAX_PLAYERS * PlayerPalette::COUNT> replacements;

    RGB replacementFor(size_t teamIdx, PlayerPalette playerState) const {
        const size_t idx = teamIdx * PlayerPalette::COUNT + playerState;
        return replacements[idx];
    }
};


class gfxPalette {
public:
    bool load(const std::filesystem::path& path);

    const std::vector<ColorSheet>& colorSheets() const { return m_colorsheets; }
    std::optional<RGB> replacementFor(RGB keyColor, size_t teamIndex, PlayerPalette playerState) const;
    std::optional<RGB> replacementFor(size_t sheetIndex, size_t teamIndex, PlayerPalette playerState) const;

private:
    std::vector<ColorSheet> m_colorsheets;
};
