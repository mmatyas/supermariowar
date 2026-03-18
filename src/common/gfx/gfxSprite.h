#ifndef GFX_SPRITE
#define GFX_SPRITE

#include "gfx/Color.h"
#include "util/SdlHelpers.h"

#include "SDL.h"

#include <filesystem>
#include <optional>

enum class ClipEdge : unsigned char { Top, Right, Bottom, Left };


class gfxSprite {
public:
    gfxSprite() = default;
    gfxSprite(
        const std::filesystem::path& filename,
        std::optional<RGB> color_key = colors::MAGENTA,
        std::optional<Uint8> alpha = std::nullopt,
        std::optional<int> wrap = std::nullopt);
    gfxSprite(SdlSurfacePtr image, std::optional<int> wrap = 640);

    /// Draw the whole sprite at the given coordinate.
    void draw(int x, int y) const;
    /// Draw part of the sprite at the given coordinate.
    void draw(int x, int y, const SDL_Rect& srcRect) const;
    /// Draw part of the sprite at the given coordinate, clipped along a given direction.
    /// Used mainly for warping and drawing previews.
    /// TODO: Most of the caller sites don't use std::optional. By updating the code there
    /// this function could be merged with the one without the clip parameters.
    void draw(int x, int y, const SDL_Rect& srcRect, ClipEdge clipEdge, int clipTreshold) const;
    /// Draw a part of the sprite scaled to a destination area.
    void drawStretch(const SDL_Rect& dstRect, const SDL_Rect& srcRect) const;

    void setalpha(Uint8 alpha);

    int getWidth() const { return m_picture->w; }
    int getHeight() const { return m_picture->h; }

    SDL_Surface* getSurface() const { return m_picture.get(); }

    void setWrap(short wrapsize = 640);
    bool isWrapping() const { return m_wrap_x.has_value(); }

private:
    SdlSurfacePtr m_picture;
    std::optional<int> m_wrap_x = std::nullopt;
};


class SpriteBuilder {
public:
    SpriteBuilder(std::filesystem::path path)
        : m_path(std::move(path))
    {}

    SpriteBuilder& withColorKey(RGB key) {
        m_color_key = key;
        return *this;
    }

    SpriteBuilder& withoutColorKey() {
        m_color_key = std::nullopt;
        return *this;
    }

    SpriteBuilder& withAlpha(Uint8 alpha) {
        m_alpha = alpha;
        return *this;
    }

    SpriteBuilder& withWrapping(int wrap_x = 640) {
        m_wrap_x = wrap_x;
        return *this;
    }

    gfxSprite create() const {
        return gfxSprite(m_path, m_color_key, m_alpha, m_wrap_x);
    }

private:
    std::filesystem::path m_path;
    std::optional<RGB> m_color_key = colors::MAGENTA;
    std::optional<Uint8> m_alpha = std::nullopt;
    std::optional<int> m_wrap_x = std::nullopt;
};

#endif // GFX_SPRITE
