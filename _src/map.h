
#ifndef SMW_MAP_H
#define SMW_MAP_H

enum TileType{tile_nonsolid = 0, tile_solid = 1, tile_solid_on_top = 2, tile_ice = 3, tile_death = 4, tile_death_on_top = 5, tile_death_on_bottom = 6, tile_gap = 8};
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

struct MapItem
{
	short itype;
	short ix;
	short iy;
};

struct TilesetTile
{
	short iID;
	short iCol;
	short iRow;
};

struct TilesetTranslation
{
	short iID;
	char szName[128];
};

struct AnimatedTile
{
	short id;
	TilesetTile layers[4];
	SDL_Rect rSrc[4][4];
	SDL_Rect rAnimationSrc[2][4];
	SDL_Rect rDest;
	bool fBackgroundAnimated;
	bool fForegroundAnimated;
};

class IO_Block;

class CMap
{
	public:
		CMap();
		~CMap();

		void clearMap();
		void clearPlatforms();

		void loadMap(const std::string& file, ReadType iReadType);
		void saveMap(const std::string& file);
		void saveThumbnail(const std::string &file, bool fUseClassicPack);

		void UpdateAllTileGaps();
		void UpdateTileGap(short i, short j);

		void loadPlatforms(FILE * mapfile, bool fPreview, int version[4], short * translationid);

		//void convertMap();

		void shift(int xshift, int yshift);

		void predrawbackground(gfxSprite &background, gfxSprite &mapspr);
		void predrawforeground(gfxSprite &foregroundspr);

		void SetupAnimatedTiles();
		
		void preDrawPreviewBackground(SDL_Surface * targetSurface, bool fThumbnail);
		void preDrawPreviewBackground(gfxSprite * spr_background, SDL_Surface * targetSurface, bool fThumbnail);
		void preDrawPreviewForeground(SDL_Surface * targetSurface, bool fThumbnail);
		void preDrawPreviewWarps(SDL_Surface * targetSurface, bool fThumbnail);
		void preDrawPreviewMapItems(SDL_Surface * targetSurface, bool fThumbnail);

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

		void AddTemporaryPlatform(MovingPlatform * platform);

		void findspawnpoint(short iType, short * x, short * y, short width, short height, bool tilealigned);
		void CalculatePlatformNoSpawnZones();
		bool IsInPlatformNoSpawnZone(short x, short y, short width, short height);

		char szBackgroundFile[128];
		short backgroundID;
		short eyecandyID;
		short musicCategoryID;
		short iNumMapItems;


	private:

		void SetTileGap(short i, short j);
		void calculatespawnareas(short iType, bool fUseTempBlocks);

		TilesetTile	mapdata[MAPWIDTH][MAPHEIGHT][MAPLAYERS];
		TileType	mapdatatop[MAPWIDTH][MAPHEIGHT];
		short		objectdata[MAPWIDTH][MAPHEIGHT];
		IO_Block*   blockdata[MAPWIDTH][MAPHEIGHT];
		bool		nospawn[NUMSPAWNAREATYPES][MAPWIDTH][MAPHEIGHT];

		std::vector<AnimatedTile*> animatedtiles;

		MovingPlatform ** platforms;
		short		iNumPlatforms;

		std::list<MovingPlatform*> tempPlatforms;

		MapItem		mapitems[MAXMAPITEMS];

		SpawnArea	spawnareas[NUMSPAWNAREATYPES][MAXSPAWNAREAS];
		short		numspawnareas[NUMSPAWNAREATYPES];
		short		totalspawnsize[NUMSPAWNAREATYPES];

		Warp        warpdata[MAPWIDTH][MAPHEIGHT];
		short		numwarpexits; //number of warp exits
		WarpExit	warpexits[MAXWARPS];
		short		warplocktimer[10];
		bool		warplocked[10];
		short		maxConnection;

		SDL_Rect	tilebltrect;
		SDL_Rect    bltrect;

		SDL_Rect	drawareas[MAXDRAWAREAS];
		short		numdrawareas;

		short		iSwitches[4];
		std::list<IO_Block*> switchBlocks[8];

		bool		fAutoFilter[NUM_AUTO_FILTERS];

		short		iTileAnimationTimer;
		short		iTileAnimationFrame;

		short iAnimatedBackgroundLayers;
		SDL_Surface * animatedFrontmapSurface;
		SDL_Surface * animatedBackmapSurface;

		SDL_Surface * animatedTilesSurface;

		short iAnimatedTileCount;
		short iAnimatedVectorIndices[NUM_FRAMES_BETWEEN_TILE_ANIMATION + 1];

		void AnimateTiles(short iFrame);
		void ClearAnimatedTiles();

		void draw(SDL_Surface *targetsurf, int layer);
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
		friend void AdjustMapItems(short iClickX, short iClickY);
		friend void RemoveMapItemAt(short x, short y);	

		friend int editor_edit();
		friend int editor_tiles();
		friend int editor_blocks();
		friend int editor_warp();
		friend int save_as();
		friend int load();
		friend void LoadMapObjects();
		friend void draw_platform(short iPlatform, bool fDrawTileTypes);
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

