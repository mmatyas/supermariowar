#include "gfxSprite.h"

#include "gfx.h"

#include "SDL_image.h"

#include <cassert>
#include <cstdio>
#include <filesystem>
#include <iostream>
#include <optional>

namespace fs = std::filesystem;

extern SDL_Surface * blitdest;
extern SDL_Surface * screen;
extern short x_shake;
extern short y_shake;

namespace {
SdlSurfacePtr loadImage(
    const fs::path& path,
    std::optional<RGB> color_key = std::nullopt,
    std::optional<Uint8> alpha = std::nullopt)
{
    const std::string path_str = path.string();

    std::cout << "loading sprite";
    if (color_key) {
        std::cout << " with key";
        if (alpha) {
            std::cout << "+alpha";
        }
        std::cout << " ";
    }
    std::cout << path_str << " ...";

    auto raw = SdlSurfacePtr(IMG_Load(path_str.c_str()));
    if (!raw) {
        std::cout << "\nERROR: Couldn't load " << path << ": " << IMG_GetError() << std::endl;
        return {};
    }

    if (color_key) {
        const Uint32 key = SDL_MapRGB(raw->format, color_key->r, color_key->g, color_key->b);
        if (SDL_SetColorKey(raw.get(), SDL_TRUE, key) < 0) {
            std::cout << "\nERROR: Couldn't set color key for " << path << ": " << SDL_GetError() << std::endl;
            return {};
        }
    }

    auto img = SdlSurfacePtr(SDL_ConvertSurface(raw.get(), screen->format, 0));
    if (!img) {
        std::cout << "\nERROR: Couldn't convert " << path << " to the display's pixel format: " << SDL_GetError() << std::endl;
        return {};
    }

    if (SDL_SetSurfaceRLE(img.get(), 1) < 0) {
        std::cout << "\nERROR: Couldn't set RLE acceleration for " << path << ": " << SDL_GetError() << std::endl;
        return {};
    }

    if (alpha) {
        if (SDL_SetSurfaceBlendMode(img.get(), SDL_BLENDMODE_BLEND) < 0) {
            std::cout << "\nERROR: Couldn't set blend mode for " << path << ": " << SDL_GetError() << std::endl;
            return {};
        }
        if (SDL_SetSurfaceAlphaMod(img.get(), *alpha) < 0) {
            std::cout << "\nERROR: Couldn't set alpha modulation for " << path << ": " << SDL_GetError() << std::endl;
            return {};
        }
    }

    std::cout << " done" << std::endl;
    return img;
}

void blitSurface(SDL_Surface* src, const SDL_Rect* srcArea, SDL_Surface* dst, SDL_Rect* dstArea)
{
    if (SDL_BlitSurface(src, srcArea, dst, dstArea) < 0) {
        fprintf(stderr, "SDL_BlitSurface error: %s\n", SDL_GetError());
    }
}
} // namespace

//
// Color keyed without alpha
//
bool gfxSprite::init(const fs::path& filename, const RGB& key)
{
    m_picture = loadImage(filename, key);
    return m_picture.get();
}

//
// Color keyed + alpha
//
bool gfxSprite::init(const fs::path& filename, const RGB& key, Uint8 alpha)
{
    m_picture = loadImage(filename, key, alpha);
    return m_picture.get();
}

//
// Non color keyed
//
bool gfxSprite::init(const fs::path& filename)
{
    m_picture = loadImage(filename);
    return m_picture.get();
}

void gfxSprite::draw(int x, int y) const
{
    assert(m_picture);

    SDL_Rect dstRect {x + x_shake, y + y_shake, getWidth(), getHeight()};
    blitSurface(m_picture.get(), NULL, blitdest, &dstRect);

    if (fWrap) {
        if (x + getWidth() >= iWrapSize) {
            dstRect.x -= iWrapSize;
            blitSurface(m_picture.get(), NULL, blitdest, &dstRect);
        } else if (x < 0) {
            dstRect.x += iWrapSize;
            blitSurface(m_picture.get(), NULL, blitdest, &dstRect);
        }
    }
}

void gfxSprite::draw(int x, int y, const SDL_Rect& srcRect) const
{
    assert(m_picture);

    SDL_Rect dstRect {x + x_shake, y + y_shake, srcRect.w, srcRect.h};
    blitSurface(m_picture.get(), &srcRect, blitdest, &dstRect);

    if (fWrap) {
        if (x + getWidth() >= iWrapSize) {
            dstRect.x -= iWrapSize;
            blitSurface(m_picture.get(), &srcRect, blitdest, &dstRect);
        } else if (x < 0) {
            dstRect.x += iWrapSize;
            blitSurface(m_picture.get(), &srcRect, blitdest, &dstRect);
        }
    }
}

void gfxSprite::draw(int x, int y, const SDL_Rect& srcRect, ClipEdge clipEdge, int clipTreshold) const
{
    assert(m_picture);

    const SDL_Rect dstRect {x + x_shake, y + y_shake, srcRect.w, srcRect.h};
    SDL_Rect adjustedSrcRect = srcRect;
    SDL_Rect adjustedDstRect = dstRect;

    if (gfx_adjusthiddenrects(adjustedSrcRect, adjustedDstRect, clipEdge, clipTreshold))
        return;

    blitSurface(m_picture.get(), &adjustedSrcRect, blitdest, &adjustedDstRect);

    if (fWrap) {
        if (x + srcRect.w >= iWrapSize) {
            adjustedSrcRect = srcRect;
            adjustedDstRect = dstRect;
            adjustedDstRect.x -= iWrapSize;

            if (gfx_adjusthiddenrects(adjustedSrcRect, adjustedDstRect, clipEdge, clipTreshold))
                return;

            blitSurface(m_picture.get(), &adjustedSrcRect, blitdest, &adjustedDstRect);
        } else if (x < 0) {
            adjustedSrcRect = srcRect;
            adjustedDstRect = dstRect;
            adjustedDstRect.x += iWrapSize;

            if (gfx_adjusthiddenrects(adjustedSrcRect, adjustedDstRect, clipEdge, clipTreshold))
                return;

            blitSurface(m_picture.get(), &adjustedSrcRect, blitdest, &adjustedDstRect);
        }
    }
}

void gfxSprite::drawStretch(const SDL_Rect& dstRect, const SDL_Rect& srcRect) const
{
    assert(m_picture);

    // TODO: SDL2 requires dst to be writable. Fixed in SDL3.
    SDL_Rect dstRect_w = dstRect;

    if (SDL_BlitScaled(m_picture.get(), &srcRect, blitdest, &dstRect_w) < 0) {
        fprintf(stderr, "SDL_BlitScaled error: %s\n", SDL_GetError());
    }
}

void gfxSprite::setalpha(Uint8 alpha)
{
    assert(m_picture != NULL);

    if (SDL_SetSurfaceBlendMode(m_picture.get(), SDL_BLENDMODE_BLEND) < 0) {
        fprintf(stderr, "\n ERROR: couldn't set blend mode on sprite: %s\n", SDL_GetError());
        return;
    }
    if (SDL_SetSurfaceAlphaMod(m_picture.get(), alpha) < 0) {
        fprintf(stderr, "\n ERROR: couldn't set alpha on sprite: %s\n", SDL_GetError());
        return;
    }
}

void gfxSprite::setSurface(SdlSurfacePtr surface)
{
    m_picture = std::move(surface);
}

void gfxSprite::SetWrap(bool wrap, short wrapsize)
{
    fWrap = wrap;
    iWrapSize = wrapsize;
}
