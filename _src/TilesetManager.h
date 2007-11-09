#ifndef __TILESETMANAGER_H_
#define __TILESETMANAGER_H_

class CTileset
{
	public:
		CTileset(const char * szdir);
		~CTileset();

		bool ReadTileTypeFile(char * szFile);
		char * GetName() {return szName;}

		TileType GetTileType(short iTileCol, short iTileRow);
		void SetTileType(short iTileCol, short iTileRow, TileType type);
		void IncrementTileType(short iTileCol, short iTileRow);
		
		void Draw(SDL_Surface * dstSurface, short iTileSize, SDL_Rect * srcRect, SDL_Rect * dstRect);

		short GetHeight() {return iHeight;}
		short GetWidth() {return iWidth;}
		

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

		CTileset * GetTileset(short iID);
		SDL_Rect rRects[3][32][32];

    private:
        std::vector<CTileset*> tilesetlist;
};

#endif //__TILESETMANAGER_H_

