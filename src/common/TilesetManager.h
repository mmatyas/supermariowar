#pragma once

#include "FileList.h"
#include "map.h"

#include <array>
#include <memory>
#include <vector>


class CTileset {
public:
    CTileset(const std::string& dir);

    void saveTileset() const;

    TileType tileType(size_t iTileCol, size_t iTileRow) const;
    void setTileType(size_t iTileCol, size_t iTileRow, TileType type);
    TileType incrementTileType(size_t iTileCol, size_t iTileRow);
    TileType decrementTileType(size_t iTileCol, size_t iTileRow);

    void Draw(SDL_Surface* dstSurface, short tileSize, SDL_Rect* srcRect, SDL_Rect* dstRect) const;

    const std::string& name() const { return m_name; }
    short height() const { return m_height; }
    short width() const { return m_width; }
    SDL_Surface* surface(size_t index) const;

private:
    std::string m_name;
    std::string m_tilesetPath;

    std::array<gfxSprite, 3> m_sprites;

    short m_width = 0;
    short m_height = 0;
    std::vector<TileType> m_tiletypes;
};


//it was kinda a bad idea to have skinlist and announcer list based on this, because both are accessed in different ways (skinlist like an vector and announcer list like a list). grrrr
class CTilesetManager : public SimpleDirectoryList {
public:
    CTilesetManager();

    void init(const std::string& gfxPack);

    size_t indexFromName(const std::string& name) const;

    void Draw(SDL_Surface * dstSurface, short iTilesetID, short iTileSize, short iSrcTileCol, short iSrcTileRow, short iDstTileCol, short iDstTileRow);

    void saveTilesets() const;

    size_t classicTilesetIndex() const {
        return m_classicTilesetIndex;
    }
    const CTileset& classicTileset() const {
        return *m_tilesetlist.at(classicTilesetIndex());
    }

    CTileset* tileset(size_t index);
    SDL_Rect* rect(short size_id, short col, short row);
    SDL_Rect* rect(short size_id, size_t idx);

private:
    std::vector<std::unique_ptr<CTileset>> m_tilesetlist;  // TODO: Store objects
    size_t m_classicTilesetIndex = SIZE_MAX;

    std::vector<SDL_Rect> m_rects_ingame;
    std::vector<SDL_Rect> m_rects_preview;
    std::vector<SDL_Rect> m_rects_thumb;
    size_t m_max_tileset_cols = 0;

    void initTilesetRects();
};
