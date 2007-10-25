#ifndef __TILESETMANAGER_H_
#define __TILESETMANAGER_H_

class CTileset
{
	public:
		CTileset(const char * szdir);
		~CTileset();

		bool ReadTileTypeFile(char * szFile);
		char * GetName() {return szName;}

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

		short GetIndexFromName(char * szName);

    private:
        std::vector<CTileset*> tilesetlist;

		SDL_Rect rRects[32][32];
};

#endif //__TILESETMANAGER_H_

