
#ifndef SMW_MAP_H
#define SMW_MAP_H

enum TileType{tile_nonsolid = 0, tile_solid = 1, tile_solid_on_top = 2, tile_ice = 3, tile_death = 4, tile_death_on_top = 5, tile_death_on_bottom = 6};
enum ReadType{read_type_full = 0, read_type_preview = 1, read_type_summary = 2};

class MovingPlatform;

struct Warp
{
	short direction;
	short connection;
	short id;
};

struct WarpExit
{
	short direction;
	short connection;
	short id;
	
	short x; //Player location where player warps out of
	short y; 

	short lockx;  //Location to display lock icon
	short locky;  

	short warpx;  //map grid location for first block in warp
	short warpy;
	short numblocks;  //number of warp blocks for this warp
};

struct SpawnArea
{
	short left;
	short top;
	short width;
	short height;
	short size;
};

class IO_Block;

class CMap
{
	public:
		CMap();
		~CMap();

		void loadTileSet(const std::string& tilesetfile, const std::string tilesetpng[3]);
		void saveTileSet(const std::string& tilesetfile);
		void clearTileSet();

		void clearMap();
		void clearPlatforms();

		void loadMap(const std::string& file, ReadType iReadType);
		void saveMap(const std::string& file);
		void saveThumbnail(const std::string &file, bool fUseClassicPack);

		void loadPlatforms(FILE * mapfile, bool fPreview);

		void convertMap();

		void shift(int xshift, int yshift);

		void predrawbackground(gfxSprite &background, gfxSprite &mapspr);
		void predrawforeground(gfxSprite &foregroundspr);
		
		void preDrawPreviewBackground(SDL_Surface * targetSurface, bool fThumbnail);
		void preDrawPreviewBackground(gfxSprite * spr_background, SDL_Surface * targetSurface, bool fThumbnail);
		void preDrawPreviewForeground(SDL_Surface * targetSurface, bool fThumbnail);
		void preDrawPreviewWarps(SDL_Surface * targetSurface, bool fThumbnail);

		void drawfrontlayer();

		void optimize();

		//returns the tiletype at the specific position (map coordinates) of the
		//front most visible tile
		TileType map(int x, int y)
		{
			return mapdatatop[x][y];
		}

		IO_Block * block(short x, short y)
		{
			return blockdata[x][y];
		}

		Warp * warp(short x, short y)
		{
			return &warpdata[x][y];
		}

		short blockat(short x, short y)
		{
			return objectdata[x][y];
		}

		bool spawn(short iType, short x, short y)
		{
			return !nospawn[iType][x][y];
		}

		void updatePlatforms();
		void drawPlatforms();
		void drawPlatforms(short iOffsetX, short iOffsetY);
		void resetPlatforms();

		void movingPlatformCollision(CPlayer * player);
		void movingPlatformCollision(IO_MovingObject * object);

		bool isconnectionlocked(int connection) {return warplocked[connection];}
		void lockconnection(int connection) {warplocked[connection] = true;}

		WarpExit * getRandomWarpExit(int connection, int currentID);

		void clearWarpLocks();
		void drawWarpLocks();

		void update();

		void drawbackanimations();
		void drawfrontanimations();
		void clearAnimations();

		void findspawnpoint(short iType, short * x, short * y, short width, short height, bool tilealigned);
		void CalculatePlatformNoSpawnZones();
		bool IsInPlatformNoSpawnZone(short x, short y, short width, short height);

		char szBackgroundFile[128];
		short backgroundID;
		short eyecandyID;
		short musicCategoryID;

		void WriteInt(int out, FILE * outFile);
		int ReadInt(FILE * inFile);
		void ReadIntChunk(int * mem, size_t iQuantity, FILE * inFile);

		void WriteFloat(float out, FILE * outFile);
		float ReadFloat(FILE * inFile);

		void WriteString(char * szString, FILE * outFile);
		void ReadString(char * szString, short size, FILE * outFile);

	private:
		
		void calculatespawnareas(short iType, bool fUseTempBlocks);
		
		short		mapdata[MAPWIDTH][MAPHEIGHT][MAPLAYERS];		//0 to TILESETSIZE-1: tiles, TILESETSIZE: no tile
		TileType	mapdatatop[MAPWIDTH][MAPHEIGHT];
		short		objectdata[MAPWIDTH][MAPHEIGHT];
		IO_Block*   blockdata[MAPWIDTH][MAPHEIGHT];
		bool		nospawn[NUMSPAWNAREATYPES][MAPWIDTH][MAPHEIGHT];

		MovingPlatform ** platforms;
		short		iNumPlatforms;

		SpawnArea	spawnareas[NUMSPAWNAREATYPES][MAXSPAWNAREAS];
		short		numspawnareas[NUMSPAWNAREATYPES];
		short		totalspawnsize[NUMSPAWNAREATYPES];

		Warp        warpdata[MAPWIDTH][MAPHEIGHT];
		short		numwarpexits; //number of warp exits
		WarpExit	warpexits[MAXWARPS];
		short		warplocktimer[10];
		bool		warplocked[10];
		short		maxConnection;

		TileType	tileset[TILESETSIZE+1];
		SDL_Surface	*tilesetsurface[3];

		SDL_Rect	tilebltrect;
		SDL_Rect    bltrect;

		SDL_Rect	drawareas[MAXDRAWAREAS];
		short		numdrawareas;

		short		iSwitches[4];
		std::list<IO_Block*> switchBlocks[8];

		short		iThumbTileX[TILESETSIZE]; //Optimization for drawing preview map
		short		iThumbTileY[TILESETSIZE]; 

		short		iPreviewTileX[TILESETSIZE]; //Optimization for drawing preview map
		short		iPreviewTileY[TILESETSIZE]; 

		short		iGameTileX[TILESETSIZE]; //Optimization for drawing real map on load
		short		iGameTileY[TILESETSIZE]; 

		bool		fAutoFilter[NUM_AUTO_FILTERS];

		CEyecandyContainer animatedtilesback;
		CEyecandyContainer animatedtilesfront;

		void draw(SDL_Surface *targetsurf, int layer, bool fFront);
		void drawThumbnailPlatforms(SDL_Surface * targetSurface);
		void drawPreview(SDL_Surface * targetsurf, int layer, bool fThumbnail);
		void drawPreviewBlocks(SDL_Surface * targetSurface, bool fThumbnail);

		friend void drawmap(bool fScreenshot, short iBlockSize);
		friend void drawlayer(int layer, bool fUseCopied, short iBlocksize);
		friend void takescreenshot();

		friend bool copyselectedtiles();
		friend void clearselectedmaptiles();
		friend void pasteselectedtiles(int movex, int movey);

		friend void UpdateTileType(short x, short y);

		friend int editor_edit();
		friend int editor_tiles();
		friend int editor_blocks();
		friend int editor_warp();
		friend int save_as();
		friend int load();
		friend void LoadMapObjects();
		friend void draw_platform(short iPlatform);
		friend void insert_platforms_into_map();
		friend void loadcurrentmap();
		friend void loadmap(char * szMapFile);
		
		friend class B_BreakableBlock;
		friend class B_DonutBlock;
		friend class B_ThrowBlock;
		friend class B_OnOffSwitchBlock;
		friend class B_FlipBlock;

		friend class MovingPlatform;
		friend class MapList;
		friend class CPlayer;
};

#endif

