#pragma once

#include "GlobalConstants.h"
#include "gfx/Color.h"
#include "util/SdlHelpers.h"

#include <SDL3/SDL.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>


/// The graphics packs only contain art for 4 team colors (red, green, yellow, blue).
/// The colors of teams 5-8 (orange, purple, cyan, pink) are generated from these
/// at load time, by rotating the hue of the team-specific pixels of a base team.
namespace teamcolors {

struct GeneratedColor {
    uint8_t baseTeam;  ///< Which of the original teams the art is derived from
    float hue;         ///< Target hue, in degrees
    float satMul;      ///< Saturation multiplier
    float valMul;      ///< Value (brightness) multiplier
};

constexpr std::array<GeneratedColor, MAX_PLAYERS - NUM_BASE_TEAM_COLORS> GENERATED_COLORS {{
    {0, 28.f, 1.00f, 1.00f},  // orange (from red)
    {3, 280.f, 0.85f, 1.00f}, // purple (from blue)
    {1, 180.f, 1.00f, 1.05f}, // cyan (from green)
    {0, 320.f, 0.75f, 1.10f}, // pink (from red)
}};

/// Human readable team color names, indexed by color ID.
constexpr std::array<const char*, MAX_PLAYERS> COLOR_NAMES {
    "Red", "Green", "Yellow", "Blue", "Orange", "Purple", "Cyan", "Pink",
};

/// A solid color that represents the team, eg. for gamepad LEDs.
constexpr std::array<RGB, MAX_PLAYERS> REPRESENTATIVE_COLORS {{
    {255, 0, 0}, {0, 255, 0}, {255, 255, 0}, {0, 0, 255},
    {255, 128, 0}, {150, 40, 255}, {0, 255, 255}, {255, 90, 200},
}};


/// The original team color a (possibly generated) team color is derived from.
constexpr short baseColorOf(short colorID) {
    return colorID < NUM_BASE_TEAM_COLORS ? colorID : GENERATED_COLORS[colorID - NUM_BASE_TEAM_COLORS].baseTeam;
}

/// Hue-shifts a color. Never returns the transparent color key (magenta).
RGB recolor(RGB color, float hueDelta, float satMul, float valMul);

/// The average hue of the colors in degrees, weighted by saturation and brightness.
float dominantHue(const std::vector<RGB>& colors);


enum class Axis : uint8_t {
    Horizontal,  ///< Team bands are next to each other (column per team)
    Vertical,    ///< Team bands are below each other (row per team)
};

/// Describes where the team colored bands of a sprite sheet are.
struct BandLayout {
    Axis axis;
    int offset;          ///< Position of the first (red) team band along the axis
    int size;            ///< Size of one team band along the axis
    int perpStart = 0;   ///< Start of the banded area, perpendicular to the axis
    int perpLength = -1; ///< Length of the banded area perpendicular to the axis (-1: until the end)
    int insertAt = AFTER_BANDS; ///< Where to put the generated bands along the axis

    static constexpr int AFTER_BANDS = -1; ///< Right after the original bands
    static constexpr int AT_END = -2;      ///< At the end of the image
};

/// Takes a sprite sheet with 4 team bands, and returns a new ARGB8888 surface where
/// the 4 generated team bands are inserted right after the original ones (or at `insertAt`).
/// Content after the insertion point (inside the perpendicular range) is moved
/// by 4 bands; new empty areas are filled with the color key.
/// Pixels that are the same in all 4 original bands are copied unchanged,
/// the others are recolored from the base team.
SdlSurfacePtr extendBands(SDL_Surface* src, const BandLayout& layout);

/// Generates palette colors for the extra teams.
/// `teamColors[team]` contains the replacement colors of a single key color for each base team,
/// the result contains the replacements of the generated teams.
std::array<RGB, MAX_PLAYERS - NUM_BASE_TEAM_COLORS> generatePaletteColors(
    const std::array<RGB, NUM_BASE_TEAM_COLORS>& baseColors,
    const std::array<float, NUM_BASE_TEAM_COLORS>& baseHues);

} // namespace teamcolors
