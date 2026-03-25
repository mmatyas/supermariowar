#ifndef GFX_SPRITE
#define GFX_SPRITE

#include "gfx/Color.h"
#include "math/Vec2.h"
#include "util/SdlHelpers.h"

#include <SDL3/SDL.h>

#include <filesystem>
#include <optional>

enum class ClipEdge : unsigned char { Top, Right, Bottom, Left };


class gfxSprite {
public:
    explicit gfxSprite() = default;
    explicit gfxSprite(SdlSurfacePtr image, std::optional<int> wrap = 640);

    static gfxSprite blank(unsigned w, unsigned h);

    /// Draw the whole sprite at the given coordinate. Applies camera shaking.
    void draw(int x, int y) const;
    /// Draw part of the sprite at the given coordinate. Applies camera shaking.
    void draw(int x, int y, const SDL_Rect& srcRect) const;
    /// Draw part of the sprite at the given coordinate, clipped along a given direction.
    /// Used mainly for warping and drawing previews. Applies camera shaking.
    /// TODO: Most of the caller sites don't use std::optional. By updating the code there
    /// this function could be merged with the one without the clip parameters.
    void draw(int x, int y, const SDL_Rect& srcRect, ClipEdge clipEdge, int clipTreshold) const;

   /// Draw the whole sprite on a target surface at a target coordinate.
    void draw(SDL_Surface* dst, Vec2i dstPos) const {
        blit(nullptr, dst, dstPos);
    }
    /// Draw the whole sprite on a target surface at a target coordinate.
    void draw(SDL_Surface* dst, const SDL_Rect& dstRect) const {
        blit(nullptr, dst, Vec2i {dstRect.x, dstRect.y});
    }
    /// Draw part of the sprite on a target surface at a target coordinate.
    void draw(const SDL_Rect& srcRect, SDL_Surface* dst, Vec2i dstPos) const {
        blit(&srcRect, dst, dstPos);
    }
    /// Draw part of the sprite on a target surface at a target coordinate.
    void draw(const SDL_Rect& srcRect, SDL_Surface* dst, const SDL_Rect& dstRect) const {
        blit(&srcRect, dst, Vec2i {dstRect.x, dstRect.y});
    }

    /// Draw a part of the sprite scaled to a destination area.
    void drawStretch(const SDL_Rect& srcRect, SDL_Surface* dst, const SDL_Rect& dstRect) const;

    void setalpha(Uint8 alpha);

    int getWidth() const { return m_picture->w; }
    int getHeight() const { return m_picture->h; }

    SDL_Surface* getSurface() const { return m_picture.get(); }

    void setWrap(short wrapsize = 640);
    bool isWrapping() const { return m_wrap_x.has_value(); }

    explicit operator bool() const { return getSurface(); }

private:
    SdlSurfacePtr m_picture;
    std::optional<int> m_wrap_x = std::nullopt;

    void blit(const SDL_Rect* srcRect, SDL_Surface* dst, Vec2i dstPos) const;
};


class ImageLoader {
public:
    explicit ImageLoader(std::filesystem::path path)
        : m_path(std::move(path))
    {}
    ImageLoader& withColorKey(RGB key) {
        m_color_key = key;
        return *this;
    }
    ImageLoader& withoutColorKey() {
        m_color_key = std::nullopt;
        return *this;
    }
    ImageLoader& withAlpha(Uint8 alpha) {
        m_alpha = alpha;
        return *this;
    }
    ImageLoader& withWrapping(int wrap_x = 640) {
        m_wrap_x = wrap_x;
        return *this;
    }
    ImageLoader& withoutOptimization() {
        m_optimize = false;
        return *this;
    }
    gfxSprite create() const;

private:
    std::filesystem::path m_path;
    bool m_optimize = true;
    std::optional<RGB> m_color_key = colors::MAGENTA;
    std::optional<Uint8> m_alpha = std::nullopt;
    std::optional<int> m_wrap_x = std::nullopt;
};

#endif // GFX_SPRITE
