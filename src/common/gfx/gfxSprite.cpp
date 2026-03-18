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

bool gfxSprite::draw(short x, short y)
{
    assert(m_picture != NULL);

    SDL_Rect dstRect {x + x_shake, y + y_shake, getWidth(), getHeight()};

    if (SDL_BlitSurface(m_picture.get(), NULL, blitdest, &dstRect) < 0) {
        fprintf(stderr, "BlitSurface error: %s\n", SDL_GetError());
        return false;
    }

    if (fWrap) {
        if (x + m_picture->w >= iWrapSize) {
            dstRect.x = x - iWrapSize + x_shake;
            dstRect.y = y + y_shake;

            if (SDL_BlitSurface(m_picture.get(), NULL, blitdest, &dstRect) < 0) {
                fprintf(stderr, "SDL_BlitSurface error: %s\n", SDL_GetError());
                return false;
            }
        } else if (x < 0) {
            dstRect.x = x + iWrapSize + x_shake;
            dstRect.y = y + y_shake;

            if (SDL_BlitSurface(m_picture.get(), NULL, blitdest, &dstRect) < 0) {
                fprintf(stderr, "SDL_BlitSurface error: %s\n", SDL_GetError());
                return false;
            }
        }
    }

    return true;
}

//TODO Perf Optimization: Set w/h once when sprite is initialized, set srcx/srcy just when animation frame advance happens
bool gfxSprite::draw(short x, short y, short srcx, short srcy, short w, short h, short sHiddenDirection, short sHiddenValue)
{
    assert(m_picture != NULL);

    SDL_Rect srcRect {srcx, srcy, w, h};
    SDL_Rect dstRect {x + x_shake, y + y_shake, w, h};

    if (sHiddenDirection > -1) {
        if (gfx_adjusthiddenrects(&srcRect, &dstRect, sHiddenDirection, sHiddenValue))
            return true;
    }

    // Blit onto the screen surface
    if (SDL_BlitSurface(m_picture.get(), &srcRect, blitdest, &dstRect) < 0) {
        fprintf(stderr, "SDL_BlitSurface error: %s\n", SDL_GetError());
        return false;
    }

    if (fWrap) {
        if (x + w >= iWrapSize) {
            srcRect = {srcx, srcy, w, h};
            dstRect = {x - iWrapSize + x_shake, y + y_shake, w, h};

            if (sHiddenDirection > -1) {
                if (gfx_adjusthiddenrects(&srcRect, &dstRect, sHiddenDirection, sHiddenValue))
                    return true;
            }

            if (SDL_BlitSurface(m_picture.get(), &srcRect, blitdest, &dstRect) < 0) {
                fprintf(stderr, "SDL_BlitSurface error: %s\n", SDL_GetError());
                return false;
            }
        } else if (x < 0) {
            srcRect = {srcx, srcy, w, h};
            dstRect = {x + iWrapSize + x_shake, y + y_shake, w, h};

            if (sHiddenDirection > -1) {
                if (gfx_adjusthiddenrects(&srcRect, &dstRect, sHiddenDirection, sHiddenValue))
                    return true;
            }

            if (SDL_BlitSurface(m_picture.get(), &srcRect, blitdest, &dstRect) < 0) {
                fprintf(stderr, "SDL_BlitSurface error: %s\n", SDL_GetError());
                return false;
            }
        }
    }

    return true;
}

bool gfxSprite::drawStretch(short x, short y, short w, short h, short srcx, short srcy, short srcw, short srch)
{
    assert(m_picture != NULL);

    const SDL_Rect srcRect {srcx, srcy, srcw, srch};
    SDL_Rect dstRect {x + x_shake, y + y_shake, w, h};

    // Looks like SoftStretch doesn't respect transparent colors
    // I need to look into the actual SDL code to see if I can fix this
    if (SDL_BlitScaled(m_picture.get(), &srcRect, blitdest, &dstRect) < 0) {
        fprintf(stderr, "SDL_BlitScaled error: %s\n", SDL_GetError());
        return false;
    }

    return true;
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
