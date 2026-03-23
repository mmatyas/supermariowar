#pragma once

#include "gfx.h"
#include "TileTypes.h"

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
    // The map format supports tilesets with up to 128x128 tiles
    static constexpr int MAX_TILES_PER_AXIS = 128;
    static constexpr int MAX_TILES = MAX_TILES_PER_AXIS * MAX_TILES_PER_AXIS;

    CTileset(const std::filesystem::path& dir);
    void ensureLoaded();

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
    std::filesystem::path m_tileset_path;
    std::vector<TileType> m_tiletypes;

    gfxSprite m_sprite_large;
    gfxSprite m_sprite_medium;
    gfxSprite m_sprite_small;

    short m_width = 0;
    short m_height = 0;
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

    static std::array<SDL_Rect, CTileset::MAX_TILES> s_rects_ingame;
    static std::array<SDL_Rect, CTileset::MAX_TILES> s_rects_preview;
    static std::array<SDL_Rect, CTileset::MAX_TILES> s_rects_thumb;
};
