#include "gfxFont.h"

#include "gfx.h"

#include "SDL_image.h"

#include <format>
#include <iostream>

extern SDL_Surface * blitdest;
extern SDL_Surface * screen;

namespace {
constexpr int ASCII_FIRST_PRINTABLE = 33;
} // namespace


gfxFont::gfxFont(const std::filesystem::path& path)
{
    const std::string path_str = path.generic_string();

    std::cout << "loading font " << path_str << " ...";
    auto surf = SdlSurfacePtr(IMG_Load(path_str.c_str()));
    if (!surf) {
        throw std::format("Couldn't load {}: {}", path_str, IMG_GetError());
    }

    if (SDL_MUSTLOCK(surf.get()))
        SDL_LockSurface(surf.get());

    std::vector<Uint32> first_row;
    first_row.reserve(surf->w);
    for (int x = 0; x < surf->w; x++)
        first_row.emplace_back(getRawPixel(surf.get(), x, 0));

    if (SDL_MUSTLOCK(surf.get()))
        SDL_UnlockSurface(surf.get());

    m_glyph_areas.reserve(127);

    const Uint32 raw_magenta = SDL_MapRGB(surf->format, 255, 0, 255);
    const int width = static_cast<int>(first_row.size());
    int x = 0;

    // Find first marker
    while (x < width && first_row[x] != raw_magenta)
        x++;

    while (x < width) {
        // Find marker end
        while (x < width && first_row[x] == raw_magenta)
            x++;

        if (width <= x)
            break;

        const int start = x;

        // Grow until next marker
        while (x < width && first_row[x] != raw_magenta)
            x++;

        const int width = x - start;
        m_glyph_areas.emplace_back(start, width);
    }

    if (m_glyph_areas.empty())
        throw std::format("Didn't find any characters on font image {}", path_str);

    m_glyph_areas.shrink_to_fit();

    const Uint32 color_key = getRawPixel(surf.get(), 0, 1);
    if (SDL_SetColorKey(surf.get(), SDL_TRUE, color_key) < 0)
        throw std::format("Couldn't set color key on font image: {}", SDL_GetError());

    auto surf_opti = SdlSurfacePtr(SDL_ConvertSurface(surf.get(), screen->format, 0));
    if (!surf_opti)
        throw std::format("Couldn't convert {} to the display's pixel format: {}", path_str, SDL_GetError());

    if (SDL_SetSurfaceRLE(surf_opti.get(), 1) < 0)
        throw std::format("Couldn't set RLE acceleration for {}: {}", path_str, SDL_GetError());

    m_sprite = gfxSprite(std::move(surf_opti), std::nullopt);
    std::cout << "done" << std::endl;
}

void gfxFont::draw(int x, int y, std::string_view text, int dst_min_x, int dst_max_x) const
{
    for (const char ch : text) {
        if (dst_max_x < x)
            break;

        if (ch < ASCII_FIRST_PRINTABLE) {
            x += m_glyph_areas.front().w;
            continue;
        }

        const size_t idx = ch - ASCII_FIRST_PRINTABLE;
        if (idx >= m_glyph_areas.size())
            continue;

        const SDL_Rect src {
            m_glyph_areas[idx].x, 1,
            m_glyph_areas[idx].w, m_sprite.getHeight() - 1,
        };
        if (dst_min_x <= x + src.w)
            m_sprite.draw(x, y, src);

        x += m_glyph_areas[idx].w;
    }
}

void gfxFont::setAlpha(Uint8 alpha)
{
    if (SDL_SetSurfaceBlendMode(m_sprite.getSurface(), SDL_BLENDMODE_BLEND) < 0) {
        fprintf(stderr, "\n ERROR: couldn't set blend mode on font surface: %s\n", SDL_GetError());
        return;
    }
    if (SDL_SetSurfaceAlphaMod(m_sprite.getSurface(), alpha) < 0) {
        fprintf(stderr, "\n ERROR: couldn't set alpha on font surface: %s\n", SDL_GetError());
        return;
    }
}

int gfxFont::getHeight() const
{
    return m_sprite.getHeight() - 1;
}

int gfxFont::getWidth(std::string_view text) const
{
    int width = 0;
    for (const char ch : text) {
        size_t idx = 0;
        if (ch >= ASCII_FIRST_PRINTABLE)
            idx = ch - ASCII_FIRST_PRINTABLE;
        if (idx < m_glyph_areas.size())
            width += m_glyph_areas[idx].w;
    }
    return width;
}
