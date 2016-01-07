#ifndef SMW_MAP_H
#define SMW_MAP_H

#include "GlobalConstants.h"

#include "SDL.h"

#include "gfx.h"

#include <list>
#include <string>
#include <vector>

#define NUM_FRAMES_IN_TILE_ANIMATION        4
#define NUM_FRAMES_BETWEEN_TILE_ANIMATION   8

enum TileType {
    tile_nonsolid = 0,
    tile_solid = 1,
    tile_solid_on_top = 2,
    tile_ice = 3,
    tile_death = 4,
    tile_death_on_top = 5,
    tile_death_on_bottom = 6,
    tile_death_on_left = 7,
    tile_death_on_right = 8,
    tile_ice_on_top = 9,
    tile_ice_death_on_bottom = 10,
    tile_ice_death_on_left = 11,
    tile_ice_death_on_right = 12,
    tile_super_death = 13,
    tile_super_death_top = 14,
    tile_super_death_bottom = 15,
    tile_super_death_left = 16,
    tile_super_death_right = 17,
    tile_player_death = 18,
    tile_gap = 19
};

enum ReadType: uint8_t {
    read_type_full = 0,
    read_type_preview = 1,
    read_type_summary = 2
};

enum TileTypeFlag {
    tile_flag_nonsolid = 0,
    tile_flag_solid = 1,
    tile_flag_solid_on_top = 2,
    tile_flag_ice = 4,
    tile_flag_death_on_top = 8,
    tile_flag_death_on_bottom = 16,
    tile_flag_death_on_left = 32,
    tile_flag_death_on_right = 64,
    tile_flag_gap = 128,
    tile_flag_has_death = 8056,
    tile_flag_super_death_top = 256,
    tile_flag_super_death_bottom = 512,
    tile_flag_super_death_left = 1024,
    tile_flag_super_death_right = 2048,
    tile_flag_player_death = 4096,
    tile_flag_super_or_player_death_top = 4352,
    tile_flag_super_or_player_death_bottom = 4608,
    tile_flag_super_or_player_death_left = 5120,
    tile_flag_super_or_player_death_right = 6144,
    tile_flag_player_or_death_on_bottom = 4112
};

/*
									PD  SDR SDL SDB SDT G	DR	DL	DB	DT	I	SOT	S
	tile_nonsolid = 0				0	0	0	0	0	0	0	0	0	0	0	0	0
	tile_solid = 1					0	0	0	0	0	0	0	0	0	0	0	0	1
	tile_solid_on_top = 2			0	0	0	0	0	0	0	0	0	0	0	1	0
	tile_ice_on_top = 6				0	0	0	0	0	0	0	0	0	0	1	1	0
	tile_ice = 5					0	0	0	0	0	0	0	0	0	0	1	0	1
	tile_death = 121				0	0	0	0	0	0	1	1	1	1	0	0	1
	tile_death_on_top = 9			0	0	0	0	0	0	0	0	0	1	0	0	1
	tile_death_on_bottom = 17		0	0	0	0	0	0	0	0	1	0	0	0	1
	tile_death_on_left = 33			0	0	0	0	0	0	0	1	0	0	0	0	1
	tile_death_on_right = 65		0	0	0	0	0	0	1	0	0	0	0	0	1
	tile_ice_death_on_bottom = 21	0	0	0	0	0	0	0	0	1	0	1	0	1
	tile_ice_death_on_left = 37		0	0	0	0	0	0	0	1	0	0	1	0	1
	tile_ice_death_on_right = 69	0	0	0	0	0	0	1	0	0	0	1	0	1
	tile_gap = 128					0	0	0	0	0	1	0	0	0	0	0	0	0
	tile_super_death = 3961			0	1	1	1	1	0	1	1	1	1	0	0	1
	tile_super_death_top = 265		0	0	0	0	1	0	0	0	0	1	0	0	1
	tile_super_death_bottom	= 529	0	0	0	1	0	0	0	0	1	0	0	0	1
	tile_super_death_left = 1057	0	0	1	0	0	0	0	1	0	0	0	0	1
	tile_super_death_right = 2113	0	1	0	0	0	0	1	0	0	0	0	0	1
	tile_player_death =	4096		1	0	0	0	0	0	0	0	0	0	0	0	0
*/

class CPlayer;
class MovingPlatform;
class IO_MovingObject;

struct Point {
	short x, y;
};

enum WarpEnterDirection {
	WARP_DOWN = 0,
	WARP_LEFT = 1,
	WARP_UP = 2,
	WARP_RIGHT = 3,
	WARP_UNDEFINED = -1
};

enum WarpExitDirection {
	WARP_EXIT_UP = 0,
	WARP_EXIT_RIGHT = 1,
	WARP_EXIT_DOWN = 2,
	WARP_EXIT_LEFT = 3,
	WARP_EXIT_UNDEFINED = -1
};

struct Warp {
	WarpEnterDirection direction;
	short connection;
	short id;
};

struct WarpExit {
	WarpExitDirection direction;
	short connection;
	short id;

	short x; //Player location where player warps out of
	short y;

	short lockx;  //Location to display lock icon
	short locky;

	short warpx;  //map grid location for first block in warp
	short warpy;
	short numblocks;  //number of warp blocks for this warp

	short locktimer;  //If > 0, then warp is locked and has this many frames left until unlock
};

struct SpawnArea {
	short left;
	short top;
	short width;
	short height;
	short size;
};

struct MapItem {
	short itype;
	short ix;
	short iy;
};

struct MapHazard {
	short itype;
	short ix;
	short iy;

	short iparam[NUMMAPHAZARDPARAMS];
	float dparam[NUMMAPHAZARDPARAMS];
};

struct TilesetTile {
	short iID;
	short iCol;
	short iRow;
};

struct TilesetTranslation {
	short iID;
	char szName[128];
};

struct AnimatedTile {
	short id;
	TilesetTile layers[4];
	SDL_Rect rSrc[4][4];
	SDL_Rect rAnimationSrc[2][4];
	SDL_Rect rDest;
	bool fBackgroundAnimated;
	bool fForegroundAnimated;
	MovingPlatform * pPlatform;
};

struct MapTile {
	TileType iType;
	int iFlags;
};

struct MapBlock {
	short iType;
	short iSettings[NUM_BLOCK_SETTINGS];
	bool fHidden;
};

class IO_Block;

void DrawMapHazard(MapHazard * hazard, short iSize, bool fDrawCenter);
void DrawPlatform(short pathtype, TilesetTile ** tiles,
	short startX, short startY, short endX, short endY,
	float angle, float radiusX, float radiusY,
	short iSize, short iPlatformWidth, short iPlatformHeight,
	bool fDrawPlatform, bool fDrawShadow);

class CMap
{
	public:
		CMap();
		~CMap();

		void clearMap();
		void clearPlatforms();

		void loadMap(const std::string& file, ReadType iReadType);
		void saveMap(const std::string& file);

		SDL_Surface * createThumbnailSurface(bool fUseClassicPack);
		void saveThumbnail(const std::string &file, bool fUseClassicPack);

		void UpdateAllTileGaps();
		void UpdateTileGap(short i, short j);

		//void convertMap();

		void shift(int xshift, int yshift);

		void predrawbackground(gfxSprite &background, gfxSprite &mapspr);
		void predrawforeground(gfxSprite &foregroundspr);

		void SetupAnimatedTiles();

		void preDrawPreviewBackground(SDL_Surface * targetSurface, bool fThumbnail);
		void preDrawPreviewBackground(gfxSprite * spr_background, SDL_Surface * targetSurface, bool fThumbnail);
		void preDrawPreviewBlocks(SDL_Surface * targetSurface, bool fThumbnail);
		void preDrawPreviewForeground(SDL_Surface * targetSurface, bool fThumbnail);
		void preDrawPreviewWarps(SDL_Surface * targetSurface, bool fThumbnail);
		void preDrawPreviewMapItems(SDL_Surface * targetSurface, bool fThumbnail);

		void drawfrontlayer();

		bool checkforwarp(short iData1, short iData2, short iData3, short iDirection);

		void optimize();

		//returns the tiletype at the specific position (map coordinates) of the
		//front most visible tile
    int map(int x, int y) {
			return mapdatatop[x][y].iFlags;
		}

    IO_Block * block(short x, short y) {
			return blockdata[x][y];
		}

    Warp * warp(short x, short y) {
			return &warpdata[x][y];
		}

    MapBlock * blockat(short x, short y) {
			return &objectdata[x][y];
		}

    bool spawn(short iType, short x, short y) {
			return !nospawn[iType][x][y];
		}

		void updatePlatforms();
		void drawPlatforms(short iLayer);
		void drawPlatforms(short iOffsetX, short iOffsetY, short iLayer);
		void resetPlatforms();

		void movingPlatformCollision(CPlayer * player);
		void movingPlatformCollision(IO_MovingObject * object);
		bool movingPlatformCheckSides(IO_MovingObject * object);

    bool isconnectionlocked(int connection) {
        return warplocked[connection];
    }
		void lockconnection(int connection);

		WarpExit * getRandomWarpExit(int connection, int currentID);

		void clearWarpLocks();
		void drawWarpLocks();

		void update();

		void AddPermanentPlatform(MovingPlatform * platform);
		void AddTemporaryPlatform(MovingPlatform * platform);

		bool findspawnpoint(short iType, short * x, short * y, short width, short height, bool tilealigned);
		bool IsInPlatformNoSpawnZone(short x, short y, short width, short height);

		char szBackgroundFile[128];
		short backgroundID;
		short eyecandy[3];
		short musicCategoryID;

		short iNumMapItems;
		short iNumMapHazards;

		short iNumRaceGoals;
		short iNumFlagBases;


	private:

		void SetTileGap(short i, short j);
		void calculatespawnareas(short iType, bool fUseTempBlocks, bool fIgnoreDeath);

		TilesetTile	mapdata[MAPWIDTH][MAPHEIGHT][MAPLAYERS];
		MapTile		mapdatatop[MAPWIDTH][MAPHEIGHT];
		MapBlock	objectdata[MAPWIDTH][MAPHEIGHT];
		IO_Block*   blockdata[MAPWIDTH][MAPHEIGHT];
		bool		nospawn[NUMSPAWNAREATYPES][MAPWIDTH][MAPHEIGHT];

		std::vector<AnimatedTile*> animatedtiles;

		MovingPlatform ** platforms;
		short		iNumPlatforms;

		std::list<MovingPlatform*> tempPlatforms;

		MapItem		mapitems[MAXMAPITEMS];
		MapHazard	maphazards[MAXMAPHAZARDS];

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

		Point       racegoallocations[MAXRACEGOALS];

		Point       flagbaselocations[4];

		short		iTileAnimationTimer;
		short		iTileAnimationFrame;

		short iAnimatedBackgroundLayers;
		SDL_Surface * animatedFrontmapSurface;
		SDL_Surface * animatedBackmapSurface;

		SDL_Surface * animatedTilesSurface;

		short iAnimatedTileCount;
		short iAnimatedVectorIndices[NUM_FRAMES_BETWEEN_TILE_ANIMATION + 1];

		std::list<MovingPlatform*> platformdrawlayer[5];

		void AnimateTiles(short iFrame);
		void ClearAnimatedTiles();

		void draw(SDL_Surface *targetsurf, int layer);
		void drawThumbnailHazards(SDL_Surface * targetSurface);
		void drawThumbnailPlatforms(SDL_Surface * targetSurface);
		void drawPreview(SDL_Surface * targetsurf, int layer, bool fThumbnail);
		void drawPreviewBlocks(SDL_Surface * targetSurface, bool fThumbnail);

		void addPlatformAnimatedTiles();

		friend class CPlayerAI;

		friend void drawmap(bool fScreenshot, short iBlockSize, bool fWithPlatforms);
		friend void drawlayer(int layer, bool fUseCopied, short iBlocksize);
		friend void takescreenshot();

		friend bool copyselectedtiles();
		friend void clearselectedmaptiles();
		friend void pasteselectedtiles(int movex, int movey);

		friend TileType CalculateTileType(short x, short y);
		friend void UpdateTileType(short x, short y);
		friend bool TileTypeIsModified(short x, short y);

		friend void AdjustMapItems(short iClickX, short iClickY);
		friend void RemoveMapItemAt(short x, short y);

		friend int editor_edit();
		friend int editor_tiles();
		friend int editor_blocks();
		friend int editor_warp();
		friend int editor_modeitems();
		friend void init_editor_properties(short iBlockCol, short iBlockRow);
		friend int editor_properties(short iBlockCol, short iBlockRow);
		friend int editor_maphazards();
		friend short NewMapHazard();

		friend short * GetBlockProperty(short x, short y, short iBlockCol, short iBlockRow, short * iSettingIndex);
		friend int save_as();
		friend int load();
		friend void LoadMapObjects(bool fPreview);
		friend void LoadMapHazards(bool fPreview);
		friend void draw_platform(short iPlatform, bool fDrawTileTypes);
		friend void insert_platforms_into_map();
		friend void loadcurrentmap();
		friend void loadmap(char * szMapFile);
		friend void SetNoSpawn(short nospawnmode, short col, short row, bool value);
		friend int editor_platforms();

		friend class B_BreakableBlock;
		friend class B_DonutBlock;
		friend class B_ThrowBlock;
		friend class B_OnOffSwitchBlock;
		friend class B_FlipBlock;
		friend class B_WeaponBreakableBlock;

		friend class MovingPlatform;
		friend class MapList;
		friend class MI_MapField;
		friend class CPlayer;
		friend class PlayerWarpStatus;

		friend class MO_FlagBase;
		friend class OMO_RaceGoal;

		friend class MapReader;
			friend class MapReader1500;
			friend class MapReader1600;
			friend class MapReader1700;
			friend class MapReader1701;
			friend class MapReader1702;
			friend class MapReader1800;
			friend class MapReader1802;
};

#endif // SMW_MAP_H
