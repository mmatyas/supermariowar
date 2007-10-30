#ifndef __TILESETMANAGER_H_
#define __TILESETMANAGER_H_

class CTileset
{
	public:
		CTileset(const char * szdir);
		~CTileset();

		bool ReadTileTypeFile(char * szFile);
		char * GetName() {return szName;}

		void SetTileType(short iTileCol, short iTileRow, TileType type);
		void Draw(SDL_Surface * dstSurface, short iTileSize, SDL_Rect * srcRect, SDL_Rect * dstRect);

	private:
		char szName[256];
		SDL_Surface * sSurfaces[3];
		gfxSprite sSprites[3];

		short iWidth, iHeight;

		TileType * tiletypes;
};


//it was kinda a bad idea to have skinlist and announcer list based on this, because both are accessed in different ways (skinlist like an vector and announcer list like a list). grrrr
class CTilesetManager : public SimpleDirectoryList
{
    public:
		CTilesetManager();
        virtual ~CTilesetManager();
		void Init();

		short GetIndexFromName(char * szName);

		void Draw(SDL_Surface * dstSurface, short iTilesetID, short iTileSize, short iSrcTileCol, short iSrcTileRow, short iDstTileCol, short iDstTileRow);

    private:
        std::vector<CTileset*> tilesetlist;

		SDL_Rect rRects[3][32][32];
};

#endif //__TILESETMANAGER_H_

