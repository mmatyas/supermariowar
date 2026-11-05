#include "doctest.h"

#include "gfx/TeamColors.h"

#include <SDL3/SDL.h>

#include <cmath>

namespace {
Uint32 pixel(SDL_Surface* surf, int x, int y)
{
    return reinterpret_cast<const Uint32*>(static_cast<const Uint8*>(surf->pixels) + y * surf->pitch)[x];
}

void setPixel(SDL_Surface* surf, int x, int y, RGB color)
{
    reinterpret_cast<Uint32*>(static_cast<Uint8*>(surf->pixels) + y * surf->pitch)[x] =
        0xFF000000 | (Uint32(color.r) << 16) | (Uint32(color.g) << 8) | Uint32(color.b);
}

RGB rgbAt(SDL_Surface* surf, int x, int y)
{
    const Uint32 px = pixel(surf, x, y);
    return {static_cast<uint8_t>(px >> 16), static_cast<uint8_t>(px >> 8), static_cast<uint8_t>(px)};
}

constexpr RGB RED {248, 32, 56};
constexpr RGB GREEN {0, 187, 0};
constexpr RGB YELLOW {248, 192, 0};
constexpr RGB BLUE {72, 152, 208};
constexpr RGB OUTLINE {40, 40, 40};

/// A 3x(4 bands of 2 rows) image: the first column is the team color, the rest is the same for every team.
/// After the bands there's a row of extra content.
SdlSurfacePtr makeBandedSurface()
{
    auto surf = SdlSurfacePtr(SDL_CreateSurface(3, 4 * 2 + 1, SDL_PIXELFORMAT_ARGB8888));
    const RGB teamColors[4] {RED, GREEN, YELLOW, BLUE};
    for (int team = 0; team < 4; team++) {
        for (int row = 0; row < 2; row++) {
            setPixel(surf.get(), 0, team * 2 + row, teamColors[team]);
            setPixel(surf.get(), 1, team * 2 + row, OUTLINE);
            setPixel(surf.get(), 2, team * 2 + row, colors::MAGENTA);
        }
    }
    for (int x = 0; x < 3; x++)
        setPixel(surf.get(), x, 8, {1, 2, 3});
    return surf;
}

float hueOf(RGB color)
{
    return teamcolors::dominantHue({color});
}
} // namespace


TEST_CASE("Team colors are generated in the right hue range")
{
    const std::array<float, NUM_BASE_TEAM_COLORS> baseHues {hueOf(RED), hueOf(GREEN), hueOf(YELLOW), hueOf(BLUE)};
    const auto generated = teamcolors::generatePaletteColors({RED, GREEN, YELLOW, BLUE}, baseHues);

    for (size_t i = 0; i < generated.size(); i++) {
        const float hue = hueOf(generated[i]);
        float diff = std::fabs(hue - teamcolors::GENERATED_COLORS[i].hue);
        diff = std::min(diff, 360.f - diff);
        CHECK(diff < 3.f);
    }
}

TEST_CASE("Colors that are the same for every team are kept")
{
    const std::array<float, NUM_BASE_TEAM_COLORS> baseHues {0.f, 120.f, 50.f, 200.f};
    const auto generated = teamcolors::generatePaletteColors({OUTLINE, OUTLINE, OUTLINE, OUTLINE}, baseHues);
    for (const RGB& color : generated)
        CHECK(color == OUTLINE);
}

TEST_CASE("Recoloring never produces the transparent color key")
{
    // Pure red rotated by -60 degrees would be exactly magenta
    CHECK(teamcolors::recolor({255, 0, 0}, -60.f, 1.f, 1.f) != colors::MAGENTA);
}

TEST_CASE("Base color of a generated team color")
{
    CHECK(teamcolors::baseColorOf(2) == 2);
    for (short color = NUM_BASE_TEAM_COLORS; color < MAX_PLAYERS; color++)
        CHECK(teamcolors::baseColorOf(color) == teamcolors::GENERATED_COLORS[color - NUM_BASE_TEAM_COLORS].baseTeam);
}

TEST_CASE("Team bands are inserted after the original ones")
{
    SdlSurfacePtr src = makeBandedSurface();
    SdlSurfacePtr out = teamcolors::extendBands(src.get(), {teamcolors::Axis::Vertical, 0, 2});
    REQUIRE(out);
    CHECK(out->w == 3);
    CHECK(out->h == MAX_PLAYERS * 2 + 1);

    // The original bands are unchanged
    CHECK(rgbAt(out.get(), 0, 0) == RED);
    CHECK(rgbAt(out.get(), 0, 7) == BLUE);

    for (int team = NUM_BASE_TEAM_COLORS; team < MAX_PLAYERS; team++) {
        for (int row = 0; row < 2; row++) {
            const int y = team * 2 + row;
            // Shared pixels are copied, transparent pixels stay transparent
            CHECK(rgbAt(out.get(), 1, y) == OUTLINE);
            CHECK(rgbAt(out.get(), 2, y) == colors::MAGENTA);

            // Team pixels get the new hue
            const float hue = hueOf(rgbAt(out.get(), 0, y));
            float diff = std::fabs(hue - teamcolors::GENERATED_COLORS[team - NUM_BASE_TEAM_COLORS].hue);
            diff = std::min(diff, 360.f - diff);
            CHECK(diff < 3.f);
        }
    }

    // The content after the bands has moved
    CHECK(rgbAt(out.get(), 0, MAX_PLAYERS * 2) == RGB {1, 2, 3});
}

TEST_CASE("Team bands can be appended to the end of the image")
{
    SdlSurfacePtr src = makeBandedSurface();
    SdlSurfacePtr out = teamcolors::extendBands(src.get(), {
        .axis = teamcolors::Axis::Vertical, .offset = 0, .size = 2, .insertAt = teamcolors::BandLayout::AT_END});
    REQUIRE(out);
    CHECK(out->h == MAX_PLAYERS * 2 + 1);

    // The extra content stays in place, the new bands follow it
    CHECK(rgbAt(out.get(), 0, 8) == RGB {1, 2, 3});
    CHECK(rgbAt(out.get(), 1, 9) == OUTLINE);
}

TEST_CASE("Images with an unexpected layout are not changed")
{
    SdlSurfacePtr src = makeBandedSurface();
    SdlSurfacePtr out = teamcolors::extendBands(src.get(), {teamcolors::Axis::Vertical, 0, 4});
    REQUIRE(out);
    CHECK(out->w == src->w);
    CHECK(out->h == src->h);
}
