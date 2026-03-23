#pragma once

#include "FileList.h"
#include "map.h"

#include <array>
#include <filesystem>
#include <memory>
#include <vector>


enum class DrawSize {
    Ingame,
    Preview,
    Thumbnail,
};


class CTileset {
public:
    CTileset(const std::filesystem::path& dir);

    void saveTileset() const;

    TileType tileType(size_t iTileCol, size_t iTileRow) const;
    void setTileType(size_t iTileCol, size_t iTileRow, TileType type);
    TileType incrementTileType(size_t iTileCol, size_t iTileRow);
    TileType decrementTileType(size_t iTileCol, size_t iTileRow);

    void Draw(SDL_Surface* dstSurface, DrawSize tileSize, SDL_Rect* srcRect, SDL_Rect* dstRect) const;

    const std::string& name() const { return m_name; }
    short height() const { return m_height; }
    short width() const { return m_width; }
    SDL_Surface* surface(DrawSize size) const;

private:
    std::string m_name;
    std::filesystem::path m_tilesetPath;

    gfxSprite m_sprite_large;
    gfxSprite m_sprite_medium;
    gfxSprite m_sprite_small;

    short m_width = 0;
    short m_height = 0;
    std::vector<TileType> m_tiletypes;
};


class CTilesetManager {
public:
    CTilesetManager(const std::filesystem::path& gfxPack);

    size_t indexFromName(const std::string& name) const;

    void Draw(SDL_Surface* dstSurface, size_t iTilesetID, DrawSize iTileSize, short iSrcTileCol, short iSrcTileRow, short iDstTileCol, short iDstTileRow);

    void saveTilesets() const;

    size_t classicTilesetIndex() const {
        return m_classicTilesetIndex;
    }
    CTileset* classicTileset() const {
        return tileset(classicTilesetIndex());
    }

    CTileset* tileset(size_t index) const;
    SDL_Rect* rect(DrawSize size, short col, short row);
    SDL_Rect* rect(DrawSize size, size_t idx);
    size_t count() const { return m_tilesetlist.size(); }

private:
    std::vector<std::unique_ptr<CTileset>> m_tilesetlist;  // TODO: Store objects
    size_t m_classicTilesetIndex = SIZE_MAX;

    std::vector<SDL_Rect> m_rects_ingame;
    std::vector<SDL_Rect> m_rects_preview;
    std::vector<SDL_Rect> m_rects_thumb;
    size_t m_max_tileset_cols = 0;

    void initTilesetRects();
};
