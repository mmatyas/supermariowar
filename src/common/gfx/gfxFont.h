#pragma once

#include "gfx/gfxSprite.h"

#include <filesystem>
#include <limits>
#include <string_view>
#include <vector>


class gfxFont {
public:
    explicit gfxFont() = default;
    explicit gfxFont(const std::filesystem::path& path);

    void draw(int x, int y, std::string_view text,
        int dst_min_x = std::numeric_limits<int>::min(),
        int dst_max_x = std::numeric_limits<int>::max()) const;

    void drawCentered(int x, int y, std::string_view text) const {
        draw(x - getWidth(text) / 2, y, text);
    }
    void drawChopCentered(int x, int y, int width, std::string_view text) const {
        width /= 2;
        draw(x - getWidth(text) / 2, y, text, x - width, x + width);
    }
    void drawRightJustified(int x, int y, std::string_view text) const {
        draw(x - getWidth(text), y, text);
    }
    void drawChopRight(int x, int y, int width, std::string_view text) const {
        draw(x, y, text, x, x + width);
    }
    void drawChopLeft(int x, int y, int width, std::string_view text) const {
        draw(x - getWidth(text), y, text, x - width, x);
    }

    void setAlpha(Uint8 alpha);

    int getHeight() const;
    int getWidth(std::string_view text) const;

private:
    gfxSprite m_sprite;

    struct GlyphArea {
        int x = 0;
        int w = 0;
    };
    std::vector<GlyphArea> m_glyph_areas;
};
