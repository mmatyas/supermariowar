#ifndef TILESETMANAGER_H
#define TILESETMANAGER_H

#include "FileList.h"
#include "gfx/gfxSprite.h"
#include "map.h"

#include <array>
#include <vector>

class CTileset {
public:
    CTileset(const std::string& dir);

    void SaveTileset();

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
class CTilesetManager : public SimpleDirectoryList
{
    public:
		CTilesetManager();
        virtual ~CTilesetManager();
		void Init(const char * szGfxPack);

		short GetIndexFromName(const char * szName);

		void Draw(SDL_Surface * dstSurface, short iTilesetID, short iTileSize, short iSrcTileCol, short iSrcTileRow, short iDstTileCol, short iDstTileRow);

		void SaveTilesets();

    CTileset * GetClassicTileset() {
        return tClassicTileset;
    }
    short GetClassicTilesetIndex() {
        return iClassicTilesetIndex;
    }

		CTileset * GetTileset(short iID);
		SDL_Rect* GetRect(short size_id, short col, short row);
		SDL_Rect* GetRect(short size_id, size_t idx);

    private:
        std::vector<CTileset*> tilesetlist;
		CTileset* tClassicTileset = nullptr;
		short iClassicTilesetIndex;

		std::vector<SDL_Rect> rects_ingame;
		std::vector<SDL_Rect> rects_preview;
		std::vector<SDL_Rect> rects_thumb;
		size_t max_tileset_cols = 0;

		void InitTilesetRects();
};

#endif // TILESETMANAGER_H
