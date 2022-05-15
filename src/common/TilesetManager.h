#ifndef TILESETMANAGER_H
#define TILESETMANAGER_H

#include "FileList.h"
#include "gfx/gfxSprite.h"
#include "map.h"

#include "SDL.h"
#include <vector>

class CTileset
{
	public:
		CTileset(const char * szdir);
		~CTileset();

		bool ReadTileTypeFile(char * szFile);
    char * GetName() {
        return szName;
    }

		TileType GetTileType(short iTileCol, short iTileRow);
		void SetTileType(short iTileCol, short iTileRow, TileType type);
		TileType IncrementTileType(short iTileCol, short iTileRow);
		TileType DecrementTileType(short iTileCol, short iTileRow);

		void Draw(SDL_Surface * dstSurface, short iTileSize, SDL_Rect * srcRect, SDL_Rect * dstRect);

    short GetHeight() {
        return iHeight;
    }
    short GetWidth() {
        return iWidth;
    }

		void SaveTileset();

    SDL_Surface * GetSurface(short iIndex) {
        if (iIndex < 0 || iIndex > 2) return NULL;
        return sSurfaces[iIndex];
    }

	private:
		char szName[256];
		char szTilesetPath[1024];
		char szGfxPack[256];

		SDL_Surface * sSurfaces[3];
		gfxSprite sSprites[3];

		short iWidth, iHeight;
		std::vector<TileType> tiletypes;
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
