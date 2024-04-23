#pragma once

#include "gfx.h"
#include "GlobalConstants.h"

#include <array>
#include <vector>


class gfxPalette {
public:
    bool load(const std::string& path);
    void clear();

    size_t colorCount() const { return m_colorCount; }
    bool matchesColorAtID(size_t idx, uint8_t r, uint8_t g, uint8_t b) const;
    void copyColorSchemeTo(
        size_t teamID, size_t schemeID, size_t colorID,
        uint8_t& r, uint8_t& g, uint8_t& b) const;

private:
    using ColorList = std::vector<RGB>;
    using SchemeList = std::array<ColorList, PlayerPalette::NUM_PALETTES>;

    size_t m_colorCount = 0;
    std::vector<RGB> m_colorCodes;
    std::array<SchemeList, MAX_PLAYERS> m_teamSchemes;
};
