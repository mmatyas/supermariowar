#include "TeamColors.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <format>
#include <numbers>


namespace teamcolors {
namespace {
struct HSV {
    float h;  // degrees, [0, 360)
    float s;  // [0, 1]
    float v;  // [0, 1]
};

HSV toHsv(RGB c)
{
    const float r = c.r / 255.f;
    const float g = c.g / 255.f;
    const float b = c.b / 255.f;
    const float maxc = std::max({r, g, b});
    const float minc = std::min({r, g, b});
    const float delta = maxc - minc;

    HSV out {0.f, 0.f, maxc};
    if (maxc <= 0.f || delta <= 0.f)
        return out;

    out.s = delta / maxc;
    if (maxc == r)
        out.h = 60.f * std::fmod((g - b) / delta, 6.f);
    else if (maxc == g)
        out.h = 60.f * ((b - r) / delta + 2.f);
    else
        out.h = 60.f * ((r - g) / delta + 4.f);

    if (out.h < 0.f)
        out.h += 360.f;
    return out;
}

RGB toRgb(HSV c)
{
    const float chroma = c.v * c.s;
    const float hp = c.h / 60.f;
    const float x = chroma * (1.f - std::fabs(std::fmod(hp, 2.f) - 1.f));
    float r = 0.f, g = 0.f, b = 0.f;
    switch (static_cast<int>(hp) % 6) {
        case 0: r = chroma; g = x; break;
        case 1: r = x; g = chroma; break;
        case 2: g = chroma; b = x; break;
        case 3: g = x; b = chroma; break;
        case 4: r = x; b = chroma; break;
        default: r = chroma; b = x; break;
    }
    const float m = c.v - chroma;
    auto to8 = [m](float f) {
        return static_cast<uint8_t>(std::clamp(std::lround((f + m) * 255.f), 0L, 255L));
    };
    return {to8(r), to8(g), to8(b)};
}

// Direct pixel access for ARGB8888 surfaces
Uint32* pixelAt(SDL_Surface* surf, int x, int y)
{
    return reinterpret_cast<Uint32*>(static_cast<Uint8*>(surf->pixels) + y * surf->pitch) + x;
}

constexpr Uint32 ALPHA_MASK = 0xFF000000;

RGB unpack(Uint32 px) {
    return {static_cast<uint8_t>(px >> 16), static_cast<uint8_t>(px >> 8), static_cast<uint8_t>(px)};
}

Uint32 pack(RGB c, Uint32 alpha) {
    return (alpha & ALPHA_MASK) | (Uint32(c.r) << 16) | (Uint32(c.g) << 8) | Uint32(c.b);
}

bool isTransparent(Uint32 px) {
    return (px & ALPHA_MASK) == 0 || unpack(px) == colors::MAGENTA;
}
} // namespace


RGB recolor(RGB color, float hueDelta, float satMul, float valMul)
{
    HSV hsv = toHsv(color);
    hsv.h = std::fmod(hsv.h + hueDelta + 720.f, 360.f);
    hsv.s = std::clamp(hsv.s * satMul, 0.f, 1.f);
    hsv.v = std::clamp(hsv.v * valMul, 0.f, 1.f);
    RGB out = toRgb(hsv);
    if (out == colors::MAGENTA)
        out.g = 1;  // don't become transparent
    return out;
}


float dominantHue(const std::vector<RGB>& colors)
{
    double sx = 0.0;
    double sy = 0.0;
    for (const RGB& color : colors) {
        const HSV hsv = toHsv(color);
        const double weight = hsv.s * hsv.v;
        const double rad = hsv.h * std::numbers::pi / 180.0;
        sx += weight * std::cos(rad);
        sy += weight * std::sin(rad);
    }
    const double deg = std::atan2(sy, sx) * 180.0 / std::numbers::pi;
    return static_cast<float>(std::fmod(deg + 360.0, 360.0));
}


std::array<RGB, MAX_PLAYERS - NUM_BASE_TEAM_COLORS> generatePaletteColors(
    const std::array<RGB, NUM_BASE_TEAM_COLORS>& baseColors,
    const std::array<float, NUM_BASE_TEAM_COLORS>& baseHues)
{
    const bool teamSpecific = std::any_of(baseColors.cbegin(), baseColors.cend(),
        [&](const RGB& c) { return c != baseColors[0]; });

    std::array<RGB, MAX_PLAYERS - NUM_BASE_TEAM_COLORS> out {};
    for (size_t i = 0; i < GENERATED_COLORS.size(); i++) {
        const GeneratedColor& gen = GENERATED_COLORS[i];
        out[i] = teamSpecific
            ? recolor(baseColors[gen.baseTeam], gen.hue - baseHues[gen.baseTeam], gen.satMul, gen.valMul)
            : baseColors[0];
    }
    return out;
}


SdlSurfacePtr extendBands(SDL_Surface* input, const BandLayout& layout)
{
    assert(input);
    auto src = SdlSurfacePtr(SDL_ConvertSurface(input, SDL_PIXELFORMAT_ARGB8888));
    if (!src)
        throw std::format("Couldn't convert surface for team color generation: {}", SDL_GetError());

    const bool horizontal = layout.axis == Axis::Horizontal;
    const int axisLen = horizontal ? src->w : src->h;
    const int perpTotal = horizontal ? src->h : src->w;
    const int perpStart = std::clamp(layout.perpStart, 0, perpTotal);
    const int perpEnd = layout.perpLength < 0 ? perpTotal : std::min(perpTotal, perpStart + layout.perpLength);
    const int bandsEnd = layout.offset + NUM_BASE_TEAM_COLORS * layout.size;
    const int extraBands = MAX_PLAYERS - NUM_BASE_TEAM_COLORS;
    const int grow = extraBands * layout.size;

    const int insertPos = layout.insertAt == BandLayout::AFTER_BANDS ? bandsEnd
        : layout.insertAt == BandLayout::AT_END ? axisLen
        : layout.insertAt;

    if (layout.offset < 0 || layout.size <= 0 || bandsEnd > axisLen || insertPos < bandsEnd || insertPos > axisLen) {
        // The graphics pack doesn't have the expected layout; leave it alone
        return src;
    }

    const int outW = horizontal ? src->w + grow : src->w;
    const int outH = horizontal ? src->h : src->h + grow;
    auto dst = SdlSurfacePtr(SDL_CreateSurface(outW, outH, SDL_PIXELFORMAT_ARGB8888));
    if (!dst)
        throw std::format("Couldn't create surface for team color generation: {}", SDL_GetError());

    SDL_LockSurface(src.get());
    SDL_LockSurface(dst.get());

    const Uint32 fill = pack(colors::MAGENTA, ALPHA_MASK);
    for (int y = 0; y < outH; y++)
        for (int x = 0; x < outW; x++)
            *pixelAt(dst.get(), x, y) = fill;

    // (axis, perpendicular) -> (x, y)
    auto srcPx = [&](int a, int p) { return horizontal ? pixelAt(src.get(), a, p) : pixelAt(src.get(), p, a); };
    auto dstPx = [&](int a, int p) { return horizontal ? pixelAt(dst.get(), a, p) : pixelAt(dst.get(), p, a); };

    // Copy the original content, shifting what's after the bands
    for (int p = 0; p < perpTotal; p++) {
        const bool inRange = perpStart <= p && p < perpEnd;
        for (int a = 0; a < axisLen; a++) {
            const int dstA = (inRange && a >= insertPos) ? a + grow : a;
            *dstPx(dstA, p) = *srcPx(a, p);
        }
    }

    // Find which pixels are team specific, and the hue of each base team
    auto isTeamSpecific = [&](int i, int p) {
        const Uint32 first = *srcPx(layout.offset + i, p);
        for (int team = 1; team < NUM_BASE_TEAM_COLORS; team++)
            if (*srcPx(layout.offset + team * layout.size + i, p) != first)
                return true;
        return false;
    };

    std::array<float, NUM_BASE_TEAM_COLORS> baseHues {};
    for (int team = 0; team < NUM_BASE_TEAM_COLORS; team++) {
        std::vector<RGB> samples;
        for (int i = 0; i < layout.size; i++) {
            for (int p = perpStart; p < perpEnd; p++) {
                const Uint32 px = *srcPx(layout.offset + team * layout.size + i, p);
                if (!isTransparent(px) && isTeamSpecific(i, p))
                    samples.push_back(unpack(px));
            }
        }
        baseHues[team] = dominantHue(samples);
    }

    // Generate the new bands
    for (int n = 0; n < extraBands; n++) {
        const GeneratedColor& gen = GENERATED_COLORS[n];
        const float hueDelta = gen.hue - baseHues[gen.baseTeam];
        for (int i = 0; i < layout.size; i++) {
            for (int p = perpStart; p < perpEnd; p++) {
                Uint32 px = *srcPx(layout.offset + gen.baseTeam * layout.size + i, p);
                if (!isTransparent(px) && isTeamSpecific(i, p))
                    px = pack(recolor(unpack(px), hueDelta, gen.satMul, gen.valMul), px);
                *dstPx(insertPos + n * layout.size + i, p) = px;
            }
        }
    }

    SDL_UnlockSurface(dst.get());
    SDL_UnlockSurface(src.get());
    return dst;
}

} // namespace teamcolors
