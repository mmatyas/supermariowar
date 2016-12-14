/*----------------------------------------------------------+
| Super Mario War Leveleditor								|
|															|
| based on the jnrdev #1/#2 level editor					|
|															|
| ...a big bunch of crappy code ;)							|
| ...with some improvements									|
|															|
|															|
| this sourcecode is released under the GPLv2.				|
|															|
| start:		18.12.2003									|
| last changes:	 3.12.2007									|
|															|
|  (C) 2003-2009 Florian Hufsky <florian.hufsky@gmail.com>	|
+----------------------------------------------------------*/

#define SMW_EDITOR

#include "eyecandy.h"
#include "FPSLimiter.h"
#include "GameMode.h"
#include "gfx.h"
#include "GlobalConstants.h"
#include "map.h"
#include "MapList.h"
#include "movingplatform.h"
#include "path.h"
#include "FileIO.h"
#include "ResourceManager.h"
#include "sfx.h"
#include "TilesetManager.h"

// Included only for movingplatform
// TODO: Remove and fix linker errors
#include "object.h"
#include "player.h"
void CPlayer::flipsidesifneeded() {}
bool CPlayer::isInvincible() const { return false; }
bool CPlayer::isShielded() const { return false; }
short CPlayer::KillPlayerMapHazard(bool, killstyle, bool, short) { return 0; }
bool PlayerKuriboShoe::is_on() const { return false; }
void IO_MovingObject::flipsidesifneeded() {}
void IO_MovingObject::KillObjectMapHazard(short playerID) {}
void removeifprojectile(IO_MovingObject * object, bool playsound, bool forcedead) {}

#include "SDL.h"
#include "SDL_image.h"
#include "sdl12wrapper.h"

#ifdef PNG_SAVE_FORMAT
	// this function was added to SDL2
	#ifndef USE_SDL2
	    #include "savepng.h"
	#endif
#endif

#include <cmath>
#include <ctype.h>
#include <cstring>
#include <cstdlib>

#ifdef _WIN32
	#include <windows.h>
    #ifdef _MSC_VER
    #if _MSC_VER >= 1400
        #include <stdio.h>
        FILE _iob[] = {*stdin, *stdout, *stderr};
        extern "C" FILE * __cdecl __iob_func(void) { return _iob; }
    #endif
    #endif
#endif

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif


#define MAPTITLESTRING "SMW 2.0 Level Editor"


extern CMap* g_map;
extern CTilesetManager* g_tilesetmanager;

extern FiltersList *filterslist;
extern MapList *maplist;

extern short g_iDefaultPowerupPresets[NUM_POWERUP_PRESETS][NUM_POWERUPS];
extern short g_iCurrentPowerupPresets[NUM_POWERUP_PRESETS][NUM_POWERUPS];

extern CResourceManager* rm;
extern std::string RootDataDirectory;

enum {EDITOR_EDIT, EDITOR_TILES, EDITOR_QUIT, SAVE_AS, FIND, CLEAR_MAP, EDITOR_BLOCKS, NEW_MAP, SAVE, EDITOR_WARP, EDITOR_EYECANDY, DISPLAY_HELP, EDITOR_PLATFORM, EDITOR_TILETYPE, EDITOR_BACKGROUNDS, EDITOR_MAPITEMS, EDITOR_ANIMATION, EDITOR_PROPERTIES, EDITOR_MODEITEMS, EDITOR_MAPHAZARDS};

#define MAX_PLATFORMS 8
#define MAX_PLATFORM_VELOCITY 16

class EditorMapTile
{
	public:
		TilesetTile	tile[MAPLAYERS];
		MapBlock	block;
		Warp		warp;
		bool		nospawn[NUMSPAWNAREATYPES];
		TileType	tiletype;
		int			item;
};

SDL_Surface		*screen;
SDL_Surface		*blitdest;
SDL_Event		event;

TileType		set_type = tile_solid;
int				set_tile_rows = 0;
int				set_tile_cols = 0;
int				set_tile_tileset = 0;
int				set_tile_start_x = 0;
int				set_tile_start_y = 0;
int				set_tile_end_x = 0;
int				set_tile_end_y = 0;
bool			set_tile_drag = false;

int				view_tileset_x = 0;
int				view_tileset_y = 0;

int				view_animated_tileset_x = 0;

int				set_block = 0;
int				set_block_switch_on = 0;
TileType		set_tiletype = tile_nonsolid;
int				set_mapitem = 0;

int				set_direction = 0;
int				set_connection = 0;

int				edit_mode = 1;
int				selected_layer = 0;
int				nospawn_mode = 0;

int				move_mode = 0;
int				move_start_x = 0;
int				move_start_y = 0;
int				move_offset_x = 0;
int				move_offset_y = 0;
bool			move_nodrag = false;
bool			move_replace = true;

int				move_drag_start_x = 0;
int				move_drag_start_y = 0;
int				move_drag_offset_x = 0;
int				move_drag_offset_y = 0;

int				state;
bool			selectedtiles[MAPWIDTH][MAPHEIGHT];
bool			moveselectedtiles[MAPWIDTH][MAPHEIGHT];
EditorMapTile	copiedtiles[MAPWIDTH][MAPHEIGHT];
int				copiedlayer;

//// Global stuff that the map editor doesn't need, but has references to
short			x_shake = 0;
short			y_shake = 0;

CEyecandyContainer eyecandy[3];
CGameMode		*gamemodes[GAMEMODE_LAST];
CPlayer			*list_players[4];
short			list_players_cnt = 0;

void DECLSPEC soundfinished(int channel){}
void DECLSPEC musicfinished(){}
//sfxSound * g_PlayingSoundChannels[NUM_SOUND_CHANNELS];

IO_MovingObject * createpowerup(short iType, short ix, short iy, bool side, bool spawn)
{
    return NULL;
}
///////

class MapPlatform
{
	public:
    MapPlatform() {
			tiles = new TilesetTile*[MAPWIDTH];

			for (short i = 0; i < MAPWIDTH; i++)
				tiles[i] = new TilesetTile[MAPHEIGHT];

			preview = NULL;
		}

    ~MapPlatform() {
			for (short i = 0; i < MAPWIDTH; i++)
				delete [] tiles[i];

			delete [] tiles;

			if (preview)
				SDL_FreeSurface(preview);
		}

    void UpdatePreview() {
        if (!preview) {
				preview = SDL_CreateRGBSurface(screen->flags, 160, 120, screen->format->BitsPerPixel, 0, 0, 0, 0);
				SDL_SetColorKey(preview, SDL_SRCCOLORKEY, SDL_MapRGB(preview->format, 255, 0, 255));
			}

			SDL_FillRect(preview, NULL, SDL_MapRGB(preview->format, 255, 0, 255));

        for (short iPlatformX = 0; iPlatformX < MAPWIDTH; iPlatformX++) {
            for (short iPlatformY = 0; iPlatformY < MAPHEIGHT; iPlatformY++) {
					TilesetTile * tile = &tiles[iPlatformX][iPlatformY];

					SDL_Rect bltrect = {iPlatformX << 3, iPlatformY << 3, THUMBTILESIZE, THUMBTILESIZE};
                if (tile->iID >= 0) {
						SDL_BlitSurface(g_tilesetmanager->GetTileset(tile->iID)->GetSurface(2), &g_tilesetmanager->rRects[2][tile->iCol][tile->iRow], preview, &bltrect);
                } else if (tile->iID == TILESETANIMATED) {
						SDL_BlitSurface(rm->spr_tileanimation[2].getSurface(), &g_tilesetmanager->rRects[2][tile->iCol << 2][tile->iRow], preview, &bltrect);
                } else if (tile->iID == TILESETUNKNOWN) {
						//Draw unknown tile
						SDL_BlitSurface(rm->spr_unknowntile[2].getSurface(), &g_tilesetmanager->rRects[2][0][0], preview, &bltrect);
					}
				}
			}
		}

		TilesetTile ** tiles;
		TileType types[MAPWIDTH][MAPHEIGHT];
		short iVelocity;
		short iStartX;
		short iStartY;
		short iEndX;
		short iEndY;

		short iPathType;

		float fAngle;
		float fRadiusX;
		float fRadiusY;

		short iDrawLayer;

		SDL_Rect rIcon[2];
		SDL_Surface * preview;
};

extern TileType GetIncrementedTileType(TileType type);

TileType * animatedtiletypes;
bool ReadAnimatedTileTypeFile(const char * szFile);
bool WriteAnimatedTileTypeFile(const char * szFile);


SDL_Surface * s_platform;
SDL_Surface * s_platformpathbuttons;
SDL_Surface * s_maphazardbuttons;

int save_as();
int find();
int clearMap();
bool dialog(const char * title, const char * instructions, char * input, int inputsize);
int display_help();

#ifdef _DEBUG
	void convertAll();
#endif

void drawmap(bool fScreenshot, short iBlockSize, bool fWithPlatforms = false);
void draw_platform(short iPlatform, bool fDrawTileTypes);
void SetPlatformToDefaults(short iPlatform);

void loadcurrentmap();
int savecurrentmap();
int findcurrentstring();
int newmap();
void save_map(const std::string &file);
void insert_platforms_into_map();
void CalculatePlatformDims(short iPlatform, short * ix, short * iy, short * iw, short * ih);
void LoadBackgroundPage(SDL_Surface ** sBackgrounds, short iPage);

int editor_edit();
int editor_warp();
int editor_eyecandy();
int	editor_tiles();
int editor_blocks();
int editor_mapitems();
int editor_platforms();
int editor_maphazards();
int editor_tiletype();
int editor_backgrounds();
int editor_animation();
int editor_properties(short iBlockCol, short iBlockRow);
int editor_modeitems();

void DisplayPlatformPreview(short iPlatformId, short iMouseX, short iMouseY);
void SwitchPlatforms(short iPlatformId1, short iPlatformId2);
void CopyPlatform(MapPlatform * toPlatform, MapPlatform * fromPlatform);

void UpdatePlatformPathStart(short iEditPlatform, short iClickX, short iClickY, bool fSnapToTile);
void UpdatePlatformPathEnd(short iEditPlatform, short iClickX, short iClickY, bool fSnapToTile);
void UpdatePlatformPathAngle(short iEditPlatform, short iClickX, short iClickY, bool fSnapToAngle);
void UpdatePlatformPathRadius(short iEditPlatform, short iClickX, short iClickY, bool fSnapToRadius, bool fIsRadiusX, bool fSetCircle);

void resetselectedtiles();
void copymoveselection();
void pastemoveselection(int movex, int movey);
bool copyselectedtiles();
void clearselectedmaptiles();
void pasteselectedtiles(int movex, int movey);
void getcenterselection(int * x, int * y);
void takescreenshot();

bool viewblocks = true;
bool view_only_layer = false;
bool viewwarps = true;
bool ignoreclick = false;

char findstring[FILEBUFSIZE] = "";

short g_iNumPlatforms = 0;
MapPlatform g_Platforms[MAX_PLATFORMS];

BackgroundList *backgroundlist;
extern const char * g_szMusicCategoryNames[MAXMUSICCATEGORY];
short g_musiccategorydisplaytimer = 0;

short g_messagedisplaytimer = 0;
std::string g_szMessageTitle = "";
std::string g_szMessageLine[3];
void DrawMessage();

void CopyTilesetTile(TilesetTile * to, TilesetTile * from)
{
	to->iID = from->iID;
	to->iCol = from->iCol;
	to->iRow = from->iRow;
}

void SetTilesetTile(TilesetTile * tile, short iTileset, short iCol, short iRow)
{
	tile->iID = iTileset;
	tile->iCol = iCol;
	tile->iRow = iRow;
}

void ClearTilesetTile(TilesetTile * tile)
{
	SetTilesetTile(tile, TILESETNONE, 0, 0);
}

short NewMapHazard();
void DrawMapHazardControls(MapHazard * hazard);
void AdjustMapHazardRadius(MapHazard * hazard, short iClickX, short iClickY);

void SetNoSpawn(short nospawnmode, short col, short row, bool value);

bool g_fFullScreen = false;

#ifdef __EMSCRIPTEN__
void gameloop_frame();
#endif

//main main main
int main(int argc, char *argv[])
{
	if (argc >= 2)
		RootDataDirectory = argv[1];

	smw = new CGame(RootDataDirectory.c_str());
	rm = new CResourceManager();
	g_map = new CMap();
	g_tilesetmanager = new CTilesetManager();
	filterslist = new FiltersList();
	maplist = new MapList(false);
    backgroundlist = new BackgroundList();

    /* This must occur before any data files are loaded */
    Initialize_Paths();

	bool done;

	printf("-------------------------------------------------------------------------------\n");
	printf(" %s\n", MAPTITLESTRING);
	printf("-------------------------------------------------------------------------------\n");
	printf("\n---------------- startup ----------------\n");

    {
        BinaryFile editor_settings("leveleditor.bin", "rb");
        if (editor_settings.is_open()) {
            g_fFullScreen = editor_settings.read_bool();
            editor_settings.read_string_long(findstring, FILEBUFSIZE);
        }
    }

	gfx_init(640,480, g_fFullScreen);
	blitdest = screen;
	g_tilesetmanager->Init(convertPath("gfx/Classic/tilesets").c_str());

	//Add all of the maps that are world only so we can edit them
	maplist->addWorldMaps();

	SDL_WM_SetCaption(MAPTITLESTRING, "leveleditor.ico");

	printf("\n---------------- loading graphics ----------------\n");

	rm->spr_tiletypes.init(convertPath("gfx/leveleditor/leveleditor_tile_types.png"));
	rm->spr_transparenttiles.init(convertPath("gfx/leveleditor/leveleditor_transparent_tiles.png"), 255, 0, 255, 160, true);

	rm->spr_backgroundlevel.init(convertPath("gfx/leveleditor/leveleditor_background_levels.png"), 255, 0, 255, true);
	rm->spr_tilesetlevel.init(convertPath("gfx/leveleditor/leveleditor_tileset_levels.png"), 255, 0, 255, true);

	rm->spr_eyecandy.init(convertPathC("gfx/leveleditor/leveleditor_eyecandy.png"), 255, 0, 255, true);

	s_platform = IMG_Load(convertPathC("gfx/leveleditor/leveleditor_platform.png"));
	s_platformpathbuttons = IMG_Load(convertPathC("gfx/leveleditor/leveleditor_pathtype_buttons.png"));
	s_maphazardbuttons = IMG_Load(convertPathC("gfx/leveleditor/leveleditor_maphazard_buttons.png"));

	rm->spr_warps[0].init(convertPath("gfx/leveleditor/leveleditor_warp.png"), 255, 0, 255, true);
	rm->spr_warps[1].init(convertPath("gfx/leveleditor/leveleditor_warp_preview.png"), 255, 0, 255, true);
	rm->spr_warps[2].init(convertPath("gfx/leveleditor/leveleditor_warp_thumbnail.png"), 255, 0, 255, true);

	rm->spr_platformpath.init(convertPath("gfx/leveleditor/leveleditor_platform_path.png"), 255, 0, 255, 128, true);

	rm->spr_selectedtile.init(convertPath("gfx/leveleditor/leveleditor_selectedtile.png"), 0, 0, 0, 128, true);
	rm->spr_nospawntile.init(convertPath("gfx/leveleditor/leveleditor_nospawntile.png"), 0, 0, 0, 128, true);
	rm->spr_noitemspawntile.init(convertPath("gfx/leveleditor/leveleditor_noitemspawntile.png"), 0, 0, 0, 128, true);
	rm->spr_platformstarttile.init(convertPath("gfx/leveleditor/leveleditor_platformstarttile.png"), 0, 0, 0, 64, true);
	rm->spr_platformendtile.init(convertPath("gfx/leveleditor/leveleditor_selectedtile.png"), 0, 0, 0, 64, true);
	rm->spr_platformstarttile.SetWrap(true);
	rm->spr_platformendtile.SetWrap(true);

	rm->spr_mapitems[0].init(convertPath("gfx/leveleditor/leveleditor_mapitems.png"), 255, 0, 255);
	rm->spr_mapitems[1].init(convertPath("gfx/leveleditor/leveleditor_mapitems_preview.png"), 255, 0, 255);
	rm->spr_mapitems[2].init(convertPath("gfx/leveleditor/leveleditor_mapitems_thumbnail.png"), 255, 0, 255);

	rm->spr_dialog.init(convertPath("gfx/leveleditor/leveleditor_dialog.png"), 255, 0, 255, 255, true);
	rm->menu_shade.init(convertPath("gfx/leveleditor/leveleditor_shade.png"), 255, 0, 255, 128, true);

	rm->spr_tileanimation[0].init(convertPath("gfx/packs/Classic/tilesets/tile_animation.png"), 255, 0, 255);
	rm->spr_tileanimation[1].init(convertPath("gfx/packs/Classic/tilesets/tile_animation_preview.png"), 255, 0, 255);
	rm->spr_tileanimation[2].init(convertPath("gfx/packs/Classic/tilesets/tile_animation_thumbnail.png"), 255, 0, 255);

	rm->spr_blocks[0].init(convertPath("gfx/packs/Classic/tilesets/blocks.png"), 255, 0, 255);
	rm->spr_blocks[1].init(convertPath("gfx/packs/Classic/tilesets/blocks_preview.png"), 255, 0, 255);
	rm->spr_blocks[2].init(convertPath("gfx/packs/Classic/tilesets/blocks_thumbnail.png"), 255, 0, 255);

	rm->spr_unknowntile[0].init(convertPath("gfx/packs/Classic/tilesets/unknown_tile.png"), 255, 0, 255);
	rm->spr_unknowntile[1].init(convertPath("gfx/packs/Classic/tilesets/unknown_tile_preview.png"), 255, 0, 255);
	rm->spr_unknowntile[2].init(convertPath("gfx/packs/Classic/tilesets/unknown_tile_thumbnail.png"), 255, 0, 255);

	rm->spr_powerups.init(convertPath("gfx/packs/Classic/powerups/large.png"), 255, 0, 255);
	rm->spr_powerupselector.init(convertPath("gfx/leveleditor/leveleditor_powerup_selector.png"), 255, 0, 255, 128, true);
	rm->spr_hidden_marker.init(convertPath("gfx/leveleditor/leveleditor_hidden_marker.png"), 255, 0, 255);

	rm->spr_flagbases.init(convertPath("gfx/packs/Classic/modeobjects/flagbases.png"), 255, 0, 255);
	rm->spr_racegoals.init(convertPath("gfx/packs/Classic/modeobjects/racegoal.png"), 255, 0, 255);

	rm->spr_hazard_fireball[0].init(convertPath("gfx/packs/Classic/hazards/fireball.png"), 255, 0, 255);
	rm->spr_hazard_fireball[1].init(convertPath("gfx/packs/Classic/hazards/fireball_preview.png"), 255, 0, 255);
	rm->spr_hazard_fireball[2].init(convertPath("gfx/packs/Classic/hazards/fireball_thumbnail.png"), 255, 0, 255);

	rm->spr_hazard_rotodisc[0].init(convertPath("gfx/packs/Classic/hazards/rotodisc.png"), 255, 0, 255);
	rm->spr_hazard_rotodisc[1].init(convertPath("gfx/packs/Classic/hazards/rotodisc_preview.png"), 255, 0, 255);
	rm->spr_hazard_rotodisc[2].init(convertPath("gfx/packs/Classic/hazards/rotodisc_thumbnail.png"), 255, 0, 255);

	rm->spr_hazard_bulletbill[0].init(convertPath("gfx/packs/Classic/hazards/bulletbill.png"), 255, 0, 255);
	rm->spr_hazard_bulletbill[1].init(convertPath("gfx/packs/Classic/hazards/bulletbill_preview.png"), 255, 0, 255);
	rm->spr_hazard_bulletbill[2].init(convertPath("gfx/packs/Classic/hazards/bulletbill_thumbnail.png"), 255, 0, 255);

	rm->spr_hazard_flame[0].init(convertPath("gfx/packs/Classic/hazards/flame.png"), 255, 0, 255);
	rm->spr_hazard_flame[1].init(convertPath("gfx/packs/Classic/hazards/flame_preview.png"), 255, 0, 255);
	rm->spr_hazard_flame[2].init(convertPath("gfx/packs/Classic/hazards/flame_thumbnail.png"), 255, 0, 255);

	rm->spr_hazard_pirhanaplant[0].init(convertPath("gfx/packs/Classic/hazards/pirhanaplant.png"), 255, 0, 255);
	rm->spr_hazard_pirhanaplant[1].init(convertPath("gfx/packs/Classic/hazards/pirhanaplant_preview.png"), 255, 0, 255);
	rm->spr_hazard_pirhanaplant[2].init(convertPath("gfx/packs/Classic/hazards/pirhanaplant_thumbnail.png"), 255, 0, 255);

	rm->spr_number_icons.init(convertPath("gfx/packs/Classic/awards/killsinrownumbers.png"), 255, 0, 255);

    for (short i = 0; i < 3; i++) {
		rm->spr_hazard_fireball[i].SetWrap(true, 640 >> i);
		rm->spr_hazard_rotodisc[i].SetWrap(true, 640 >> i);
		rm->spr_hazard_flame[i].SetWrap(true, 640 >> i);
		rm->spr_hazard_pirhanaplant[i].SetWrap(true, 640 >> i);
	}

    if ( SDL_SetColorKey(s_platform, SDL_SRCCOLORKEY, SDL_MapRGB(s_platform->format, 255, 0, 255)) < 0) {
		printf("\n ERROR: Couldn't set ColorKey + RLE: %s\n", SDL_GetError());
	}

    if ( SDL_SetColorKey(s_platformpathbuttons, SDL_SRCCOLORKEY, SDL_MapRGB(s_platformpathbuttons->format, 255, 0, 255)) < 0) {
		printf("\n ERROR: Couldn't set ColorKey + RLE: %s\n", SDL_GetError());
	}

    if ( SDL_SetColorKey(s_maphazardbuttons, SDL_SRCCOLORKEY, SDL_MapRGB(s_maphazardbuttons->format, 255, 0, 255)) < 0) {
		printf("\n ERROR: Couldn't set ColorKey + RLE: %s\n", SDL_GetError());
	}

	rm->menu_font_small.init(convertPath("gfx/packs/Classic/fonts/font_small.png"));
	rm->menu_font_large.init(convertPath("gfx/packs/Classic/fonts/font_large.png"));

	printf("\n---------------- load map ----------------\n");

	//Setup Platforms
    for (short iPlatform = 0; iPlatform < MAX_PLATFORMS; iPlatform++) {
		g_Platforms[iPlatform].rIcon[0].x = (iPlatform % 8) * 32;
		g_Platforms[iPlatform].rIcon[0].y = (iPlatform / 8) * 32 + 224;
		g_Platforms[iPlatform].rIcon[0].w = 32;
		g_Platforms[iPlatform].rIcon[0].h = 32;

		g_Platforms[iPlatform].rIcon[1].x = (iPlatform % 4) * 42 + 240;
		g_Platforms[iPlatform].rIcon[1].y = (iPlatform / 4) * 42 + 180;
		g_Platforms[iPlatform].rIcon[1].w = 32;
		g_Platforms[iPlatform].rIcon[1].h = 32;

        for (short iCol = 0; iCol < MAPWIDTH; iCol++) {
            for (short iRow = 0; iRow < MAPHEIGHT; iRow++) {
				ClearTilesetTile(&g_Platforms[iPlatform].tiles[iCol][iRow]);
				g_Platforms[iPlatform].types[iCol][iRow] = tile_nonsolid;
			}
		}

		g_Platforms[iPlatform].iVelocity = 4;
		g_Platforms[iPlatform].iDrawLayer = 2;  //Default to drawing in the middle layer
	}

	ReadAnimatedTileTypeFile(convertPath("gfx/packs/Classic/tilesets/tile_animation_tileset.tls").c_str());

	maplist->find(findstring);
	loadcurrentmap();
	findstring[0] = 0;  //clear out the find string so that pressing "f" will give you the find dialog

	printf("\n---------------- ready, steady, go! ----------------\n");

	resetselectedtiles();

	printf("entering level editor loop...\n");
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(gameloop_frame, 0, 1);
}

void gameloop_frame()
#else
	done = false;
    while (!done)
#endif
    {
        FPSLimiter::instance().frameStart();

        switch (state) {
			case EDITOR_EDIT:
				move_nodrag = false;
				state = editor_edit();
			break;

			case EDITOR_TILES:
				state = editor_tiles();
			break;

			case EDITOR_BLOCKS:
				state = editor_blocks();
			break;

			case EDITOR_MAPITEMS:
				state = editor_mapitems();
			break;

			case EDITOR_MODEITEMS:
				state = editor_modeitems();
			break;

			case EDITOR_WARP:
				state = editor_warp();
			break;

			case EDITOR_EYECANDY:
				state = editor_eyecandy();
			break;

			case EDITOR_PLATFORM:
				state = editor_platforms();
			break;

			case EDITOR_MAPHAZARDS:
				state = editor_maphazards();
			break;

			case EDITOR_TILETYPE:
				state = editor_tiletype();
			break;

			case EDITOR_BACKGROUNDS:
				state = editor_backgrounds();
			break;

			case EDITOR_ANIMATION:
				state = editor_animation();
			break;

			case EDITOR_QUIT:
			#ifndef __EMSCRIPTEN__
				done = true;
			#endif
			break;

			case DISPLAY_HELP:
				state = display_help();
			break;

			case SAVE_AS:
				state = save_as();
			break;

			case FIND:
				state = find();
			break;

			case CLEAR_MAP:
				state = clearMap();
			break;

			case NEW_MAP:
				state = newmap();
			break;

			case SAVE:
				state = savecurrentmap();
			break;

			default:
				printf(" PANIC: WEIRD STATE: %d\n", state);
			break;
		}

        FPSLimiter::instance().beforeFlip();
        gfx_flipscreen();
        FPSLimiter::instance().afterFlip();

#ifndef __EMSCRIPTEN__
	}


	printf("\n---------------- save map ----------------\n");

	save_map(convertPath("maps/ZZleveleditor.map"));

    BinaryFile editor_settings("leveleditor.bin", "wb");
    if (editor_settings.is_open()) {
        editor_settings.write_bool(g_fFullScreen);
        editor_settings.write_string_long(maplist->currentFilename());
    }

	WriteAnimatedTileTypeFile(convertPath("gfx/packs/Classic/tilesets/tile_animation_tileset.tls").c_str());
	g_tilesetmanager->SaveTilesets();

	printf("\n---------------- shutdown ----------------\n");
	return 0;
#endif
}

TileType CalculateTileType(short x, short y)
{
	TileType type = tile_nonsolid;
    for (short k = MAPLAYERS - 1; k >= 0; k--) {
		TilesetTile * tile = &g_map->mapdata[x][y][k];

		TileType iTileType = tile_nonsolid;
		if (tile->iID >= 0)
			iTileType = g_tilesetmanager->GetTileset(tile->iID)->GetTileType(tile->iCol, tile->iRow);
		else if (tile->iID == TILESETANIMATED)
			iTileType = animatedtiletypes[tile->iRow + (tile->iCol << 5)];

        if (iTileType != tile_nonsolid) {
			type = iTileType;
			break;
		}
	}

	return type;
}

void UpdateTileType(short x, short y)
{
	g_map->mapdatatop[x][y].iType = CalculateTileType(x, y);
}

bool TileTypeIsModified(short x, short y)
{
	return g_map->mapdatatop[x][y].iType != CalculateTileType(x, y);
}

void AdjustMapItems(short iClickX, short iClickY)
{
    for (short j = 0; j < g_map->iNumMapItems; j++) {
        if (g_map->mapitems[j].ix == iClickX && g_map->mapitems[j].iy == iClickY) {
			if ((g_map->mapdatatop[iClickX][iClickY].iType != tile_nonsolid &&
				g_map->mapdatatop[iClickX][iClickY].iType != tile_solid_on_top &&
				g_map->mapdatatop[iClickX][iClickY].iType != tile_ice_on_top) ||
                    g_map->objectdata[iClickX][iClickY].iType != -1) {
				g_map->iNumMapItems--;

                for (short k = j; k < g_map->iNumMapItems - 1; k++) {
					g_map->mapitems[k].itype = g_map->mapitems[k + 1].itype;
					g_map->mapitems[k].ix = g_map->mapitems[k + 1].ix;
					g_map->mapitems[k].iy = g_map->mapitems[k + 1].iy;
				}
			}

			break;
		}
	}
}

void RemoveMapItemAt(short x, short y)
{
    for (short j = 0; j < g_map->iNumMapItems; j++) {
        if (g_map->mapitems[j].ix == x && g_map->mapitems[j].iy == y) {
			g_map->iNumMapItems--;

            for (short k = j; k < g_map->iNumMapItems; k++) {
				g_map->mapitems[k].itype = g_map->mapitems[k + 1].itype;
				g_map->mapitems[k].ix = g_map->mapitems[k + 1].ix;
				g_map->mapitems[k].iy = g_map->mapitems[k + 1].iy;
			}

			break;
		}
	}
}

bool fExiting = false;
bool fSelectedYes = false;

bool editor_edit_initialized = false;
void init_editor_edit()
{
    if (editor_edit_initialized)
        return;

    fExiting = false;
    fSelectedYes = false;
    g_musiccategorydisplaytimer = 0;

    editor_edit_initialized = true;
}

int editor_edit()
{
    init_editor_edit();

	//int iTickStart = 0, iTicks = 0;
	//int iMax = 0, iMin = 100000000, iTotal = 0;
	//int iLoops = 0;
	//bool fTestSwitch = false;

	//maplist->next(false);
	//maplist->next(false);
	//maplist->next(false);
	//maplist->next(false);

		/*
		//Code to print out map loading times (perf test map loader)
		if (iLoops < 100)
		{
			iTickStart = SDL_GetTicks();

			if (fTestSwitch)
				maplist->next(false);
			else
				maplist->prev(false);

			fTestSwitch = !fTestSwitch;

			loadcurrentmap();

			iTicks = SDL_GetTicks() - iTickStart;

			if (iTicks > iMax)
				iMax = iTicks;

			if (iTicks < iMin)
				iMin = iTicks;

			iTotal += iTicks;

			printf("%s: %d\n", maplist->currentShortmapname(), iTicks);

			if (++iLoops >= 100)
			{
				printf("Map Load Timer - Avg: %d  Min: %d  Max: %d", iTotal / 100, iMin, iMax);
			}
		}
		*/

        if (fExiting) {
			//handle messages
            while (SDL_PollEvent(&event)) {
            #if defined(USE_SDL2) || defined(__EMSCRIPTEN__)
                const Uint8 * keystate = SDL_GetKeyboardState(NULL);
            #else
                Uint8 * keystate = SDL_GetKeyState(NULL);
            #endif

                switch (event.type) {
                    case SDL_KEYDOWN: {
                        SDLKey key = event.key.keysym.sym;

                        if (key == SDLK_LEFT) {
                            fSelectedYes = true;
                        } else if (key == SDLK_RIGHT) {
                            fSelectedYes = false;
                        } else if (event.key.keysym.sym == SDLK_KP_ENTER || event.key.keysym.sym == SDLK_RETURN) {
                            if (fSelectedYes)
                                return EDITOR_QUIT;

                            fExiting = false;
                        }
				#ifdef _DEBUG
                        else if (event.key.keysym.sym == SDLK_ESCAPE) {
                            return EDITOR_QUIT;
                        }
				#endif
				    }
				}
			}
        } else {
			//handle messages
            while (SDL_PollEvent(&event)) {
            #if defined(USE_SDL2) || defined(__EMSCRIPTEN__)
                const Uint8 * keystate = SDL_GetKeyboardState(NULL);
            #else
                Uint8 * keystate = SDL_GetKeyState(NULL);
            #endif

                switch (event.type) {
                case SDL_QUIT:
					return EDITOR_QUIT;

                case SDL_KEYDOWN: {
						SDLKey key = event.key.keysym.sym;

                    if (key == SDLK_ESCAPE) {
							if (g_musiccategorydisplaytimer > 0)
								g_musiccategorydisplaytimer = 0;
							else if (edit_mode != 1)
								edit_mode = 1;
                        else {
								fSelectedYes = false;
								fExiting = true;
							}
						}

                    if (event.key.keysym.mod & (KMOD_LALT | KMOD_RALT)) {
                        if (event.key.keysym.sym == SDLK_RETURN) {
								g_fFullScreen = !g_fFullScreen;
								gfx_changefullscreen(g_fFullScreen);
								blitdest = screen;
							}
						}

                    if (key >= SDLK_1 && key <= SDLK_4) {
                        if (edit_mode == 4) { //no spawn zones
								nospawn_mode = key - SDLK_1 + 1;
							}
						}

					if (key == SDLK_INSERT)
						takescreenshot();

					if (key == SDLK_t) {
                        editor_edit_initialized = false;
						return EDITOR_TILES;
                    }

					if (key == SDLK_i) {
                        if (edit_mode == 4) {
								for (short iRow = 0; iRow < MAPHEIGHT; iRow++)
									for (short iCol = 0; iCol < MAPWIDTH; iCol++)
										SetNoSpawn(nospawn_mode, iCol, iRow, !g_map->nospawn[nospawn_mode][iCol][iRow]);
                        } else {
                            editor_edit_initialized = false;
							return EDITOR_BLOCKS;
						}
					}

                    if (key == SDLK_a) {
                        if (edit_mode == 4) {
								for (short iRow = 0; iRow < MAPHEIGHT; iRow++)
									for (short iCol = 0; iCol < MAPWIDTH; iCol++)
										SetNoSpawn(nospawn_mode, iCol, iRow, true);
                        } else {
                                editor_edit_initialized = false;
								return EDITOR_ANIMATION;
							}
						}

						if (key == SDLK_o) {
                            editor_edit_initialized = false;
							return EDITOR_MAPITEMS;
                        }

						if (key == SDLK_j) {
                            editor_edit_initialized = false;
							return EDITOR_MODEITEMS;
                        }

						if (key == SDLK_h) {
                            editor_edit_initialized = false;
							return EDITOR_MAPHAZARDS;
                        }

                    if (key == SDLK_k) {
							int iMouseX, iMouseY;
							SDL_GetMouseState(&iMouseX, &iMouseY);
							iMouseX >>= 5;
							iMouseY >>= 5;

							short iType = g_map->objectdata[iMouseX][iMouseY].iType;
                        if (iType == 1 || iType == 15 || iType == 4 || iType == 5 || iType == 17 || iType == 18 || iType == 3) {
								editor_properties(iMouseX, iMouseY);
							}
						}

						//if 'B' is pressed, rotate backgrounds
						if (key == SDLK_b) {
                            editor_edit_initialized = false;
							return EDITOR_BACKGROUNDS;
                        }

                    if (key == SDLK_g) {
                        backgroundlist->next();

                        rm->spr_background.init(convertPath(backgroundlist->current_name()));
                        strcpy(g_map->szBackgroundFile, getFileFromPath(backgroundlist->current_name()).c_str());

                        if (!keystate[SDLK_LSHIFT] && !keystate[SDLK_RSHIFT]) {
								//Set music to background default
                            for (short iCategory = 0; iCategory < MAXMUSICCATEGORY; iCategory++) {
                                if (!strncmp(g_szMusicCategoryNames[iCategory], g_map->szBackgroundFile, strlen(g_szMusicCategoryNames[iCategory]))) {
										g_map->musicCategoryID = iCategory;
										break;
									}
								}
							}
						}

                    if (key == SDLK_r) {
							if (g_musiccategorydisplaytimer > 0 && ++g_map->musicCategoryID >= MAXMUSICCATEGORY)
								g_map->musicCategoryID = 0;

							g_musiccategorydisplaytimer = 90;
						}

                    if (key == SDLK_s ) {
							if (keystate[SDLK_LSHIFT] || keystate[SDLK_RSHIFT])
								return SAVE_AS;

							return SAVE;
						}

                    if (key == SDLK_f ) {
							if (keystate[SDLK_LSHIFT] || keystate[SDLK_RSHIFT] || findstring[0] == '\0')
								return FIND;

							findcurrentstring();
						}

                    if (key == SDLK_DELETE && (keystate[SDLK_LCTRL] || keystate[SDLK_RCTRL])) {
							return CLEAR_MAP;
						}

                    if (key == SDLK_DELETE || key == SDLK_BACKSPACE) {
                        if (edit_mode == 3) {
								clearselectedmaptiles();
								resetselectedtiles();
                        } else if (edit_mode == 6) {
								for (short iRow = 0; iRow < MAPHEIGHT; iRow++)
									for (short iCol = 0; iCol < MAPWIDTH; iCol++)
										g_map->mapdatatop[iCol][iRow].iType = tile_nonsolid;
							}
						}

                    if (key == SDLK_n) {
                        if (edit_mode == 4) {
								for (short iRow = 0; iRow < MAPHEIGHT; iRow++)
									for (short iCol = 0; iCol < MAPWIDTH; iCol++)
										SetNoSpawn(nospawn_mode, iCol, iRow, false);
                        } else {
								return NEW_MAP;
							}
						}

						if (key == SDLK_v)
							viewblocks = !viewblocks;

						if (key == SDLK_e) {
                            editor_edit_initialized = false;
							return EDITOR_EYECANDY;
                        }

						if (key == SDLK_w) {
                            editor_edit_initialized = false;
							return EDITOR_WARP;
                        }

						if (key == SDLK_l) {
                            editor_edit_initialized = false;
							return EDITOR_TILETYPE;
                        }

						if (key == SDLK_F1) {
                            //editor_edit_initialized = false; //TODO?
							return DISPLAY_HELP;
                        }

                    if (key == SDLK_PAGEUP) {
                        do {
								maplist->prev(false);
                        } while (!maplist->GetValid());

							loadcurrentmap();
						}

                    if (key == SDLK_PAGEDOWN) {
                        do {
								maplist->next(false);
                        } while (!maplist->GetValid());

							loadcurrentmap();

							//Look for a certain tile in maps for testing
							/*
							bool fKeepgoing = true;
							while (fKeepgoing)
							{
								do
								{
									maplist->next(false);
								}
								while (!maplist->GetValid());

								loadcurrentmap();

								for (short i = 0; i < MAPWIDTH; i++)
								{
									for (short j = 0; j < MAPHEIGHT; j++)
									{
										for (short k = 0; k < MAPLAYERS; k++)
										{
											TilesetTile tile = g_map->mapdata[i][j][k];

											///if (tile.iID == 3 && tile.iRow == 28 && (tile.iCol == 3 || tile.iCol == 4))
											if (tile.iID == 0 && tile.iRow == 0 && (tile.iCol == 0 || tile.iCol == 1))
											{
												fKeepgoing = false;
											}
										}
									}
								}
							}
							*/
						}

                    if (key == SDLK_y) {
							if (++selected_layer >= MAPLAYERS)
								selected_layer = 0;
						}

                    if (key == SDLK_u) {
							view_only_layer = !view_only_layer;
						}

	//#ifdef _DEBUG
	#if 0
						if (key == SDLK_HOME)
							convertAll();

						//Move map out of maps dir
                    if (key == SDLK_F12) {
							//printf("Path: %s\n", maplist->currentFilename());

							const char * szCurrentPath = maplist->currentFilename();
							char szNewPath[PATH_MAX];
							strcpy(szNewPath, "maps/moved/");

							const char * psz = strrchr(szCurrentPath, '/');

							if (!psz)
								psz = szCurrentPath;
							else
								psz++;

							strcat(szNewPath, psz);

							//printf("NewPath: %s\n", szNewPath);

                        if (CopyFile(szCurrentPath, szNewPath, false)) {
								_unlink(szCurrentPath);
								maplist->SetValid(false);
							}
						}
	#endif

						//if (key == SDLK_END)
							//g_map->optimize();

                    if (key == SDLK_m) {
							if (edit_mode == 3)
								move_replace = !move_replace;

							edit_mode = 3;
						}

                    if (key == SDLK_x) {
							edit_mode = 4;
							nospawn_mode = 0;
						}

						if (key == SDLK_z)
							edit_mode = 5;

						if (key == SDLK_LCTRL)
							move_nodrag = true;

						if (key == SDLK_p) {
                            editor_edit_initialized = false;
							return EDITOR_PLATFORM;
                        }

                    if (key == SDLK_c) {
                        if (edit_mode == 3) {
                            if (copyselectedtiles()) {
									move_mode = 3;
									getcenterselection(&move_start_x, &move_start_y);
								}
							}
						}

						break;
					}

                case SDL_KEYUP: {
						if (event.key.keysym.sym == SDLK_LCTRL)
							move_nodrag = false;
						break;
					}

                case SDL_MOUSEBUTTONDOWN: {
						short iClickX = event.button.x / TILESIZE;
						short iClickY = event.button.y / TILESIZE;

                    if (event.button.button == SDL_BUTTON_LEFT && !ignoreclick) {
                        if (edit_mode == 0) { //paint selected blocks
								g_map->objectdata[iClickX][iClickY].iType = set_block;
								g_map->objectdata[iClickX][iClickY].fHidden = false;

                            if (set_block == 1 || set_block == 15) {
									for (short iSetting = 0; iSetting < NUM_BLOCK_SETTINGS; iSetting++)
										g_map->objectdata[iClickX][iClickY].iSettings[iSetting] = g_iDefaultPowerupPresets[0][iSetting];
                            } else if (set_block >= 11 && set_block <= 14) {
									g_map->objectdata[iClickX][iClickY].iSettings[0] = set_block_switch_on;
								}

								AdjustMapItems(iClickX, iClickY);
                        } else if (edit_mode == 1) { //paint selected tile(s)
                            for (short i = 0; i < set_tile_cols; i++) {
									short iLocalX = iClickX + i;

                                for (short j = 0; j < set_tile_rows; j++) {
										short iLocalY = iClickY + j;

                                    if (iLocalX >= 0 && iLocalX < MAPWIDTH && iLocalY >= 0 && iLocalY < MAPHEIGHT) {
											bool fNeedUpdate = !TileTypeIsModified(iLocalX, iLocalY);
											SetTilesetTile(&g_map->mapdata[iLocalX][iLocalY][selected_layer], set_tile_tileset, set_tile_start_x + i, set_tile_start_y + j);

											if (fNeedUpdate)
												UpdateTileType(iLocalX, iLocalY);

											AdjustMapItems(iLocalX, iLocalY);
										}
									}
								}
                        } else if (edit_mode == 2) { //warps
								g_map->warpdata[iClickX][iClickY].direction = (WarpEnterDirection)set_direction;
								g_map->warpdata[iClickX][iClickY].connection = set_connection;
                        } else if (edit_mode == 3) { //move tiles
                            if (move_mode == 3) {
									move_mode = 0;

									pasteselectedtiles(move_offset_x, move_offset_y);

									copymoveselection();
									resetselectedtiles();
									pastemoveselection(move_offset_x, move_offset_y);

									move_offset_x = 0;
									move_offset_y = 0;
                            } else {
                                if (selectedtiles[iClickX][iClickY]) {
                                    if (copyselectedtiles()) {
											move_mode = 1;
											clearselectedmaptiles();
										}
                                } else {
										if (!keystate[SDLK_LSHIFT] && !keystate[SDLK_RSHIFT] && !keystate[SDLK_LCTRL])
											resetselectedtiles();

                                    if (move_nodrag) {
											selectedtiles[iClickX][iClickY] = true;
                                    } else {
											move_mode = 2;
											move_drag_start_x = iClickX;
											move_drag_start_y = iClickY;
										}
									}

									move_start_x = iClickX;
									move_start_y = iClickY;
								}
                        } else if (edit_mode == 4) { //no player spawn areas
								SetNoSpawn(nospawn_mode, iClickX, iClickY, true);
                        } else if (edit_mode == 5) { // no item spawn areas
								g_map->nospawn[5][iClickX][iClickY] = true;
                        } else if (edit_mode == 6) { //tile types
								g_map->mapdatatop[iClickX][iClickY].iType = set_tiletype;
								AdjustMapItems(iClickX, iClickY);
                        } else if (edit_mode == 7) { //map items
                            if (g_map->iNumMapItems < MAXMAPITEMS) {
									bool fTileNotAvailable = false;
                                for (short j = 0; j < g_map->iNumMapItems; j++) {
                                    if (g_map->mapitems[j].ix == iClickX && g_map->mapitems[j].iy == iClickY) {
											fTileNotAvailable = true;
											break;
										}
									}

									if (g_map->mapdatatop[iClickX][iClickY].iType != tile_nonsolid &&
										g_map->mapdatatop[iClickX][iClickY].iType != tile_solid_on_top &&
										g_map->mapdatatop[iClickX][iClickY].iType != tile_ice_on_top)
										fTileNotAvailable = true;

                                if (!fTileNotAvailable) {
										MapItem * mapitem = &g_map->mapitems[g_map->iNumMapItems];
										mapitem->itype = set_mapitem;
										mapitem->ix = iClickX;
										mapitem->iy = iClickY;

										g_map->iNumMapItems++;
									}
								}
                        } else if (edit_mode == 8) { //animated tiles
                            for (short i = 0; i < set_tile_cols; i++) {
									short iLocalX = iClickX + i;

                                for (short j = 0; j < set_tile_rows; j++) {
										short iLocalY = iClickY + j;

                                    if (iLocalX >= 0 && iLocalX < MAPWIDTH && iLocalY >= 0 && iLocalY < MAPHEIGHT) {
											bool fNeedUpdate = !TileTypeIsModified(iLocalX, iLocalY);
											SetTilesetTile(&g_map->mapdata[iLocalX][iLocalY][selected_layer], TILESETANIMATED, set_tile_start_y + j, set_tile_start_x + i);

											if (fNeedUpdate)
												UpdateTileType(iLocalX, iLocalY);
										}
									}
								}
							}
                    } else if (event.button.button == SDL_BUTTON_RIGHT) {
							if (edit_mode == 0)
								g_map->objectdata[iClickX][iClickY].iType = -1;
                        else if (edit_mode == 1 || edit_mode == 8) {
								bool fNeedUpdate = !TileTypeIsModified(iClickX, iClickY);
								g_map->mapdata[iClickX][iClickY][selected_layer].iID = TILESETNONE;

								if (fNeedUpdate)
									UpdateTileType(iClickX, iClickY);
                        } else if (edit_mode == 2) {
								g_map->warpdata[iClickX][iClickY].direction = WARP_UNDEFINED;
								g_map->warpdata[iClickX][iClickY].connection = -1;
                        } else if (edit_mode == 3) {
                            if (move_mode == 3) {
									move_mode = 0;
									resetselectedtiles();
									move_offset_x = 0;
									move_offset_y = 0;
                            } else {
                                if (selectedtiles[iClickX][iClickY]) {
										selectedtiles[iClickX][iClickY] = false;
                                } else {
										resetselectedtiles();
									}
								}
                        } else if (edit_mode == 4) {
								SetNoSpawn(nospawn_mode, iClickX, iClickY, false);
                        } else if (edit_mode == 5) {
								g_map->nospawn[5][iClickX][iClickY] = false;
                        } else if (edit_mode == 6) {
								g_map->mapdatatop[iClickX][iClickY].iType = tile_nonsolid;
                        } else if (edit_mode == 7) {
								RemoveMapItemAt(iClickX, iClickY);
							}
						}

						break;
					}

                case SDL_MOUSEMOTION: {
						short iClickX = event.button.x / TILESIZE;
						short iClickY = event.button.y / TILESIZE;

                    if (event.motion.state == SDL_BUTTON(SDL_BUTTON_LEFT) && !ignoreclick) {
                        if (edit_mode == 0) {
								g_map->objectdata[iClickX][iClickY].iType = set_block;
								g_map->objectdata[iClickX][iClickY].fHidden = false;

                            if (set_block == 1 || set_block == 15) {
									for (short iSetting = 0; iSetting < NUM_BLOCK_SETTINGS; iSetting++)
										g_map->objectdata[iClickX][iClickY].iSettings[iSetting] = g_iDefaultPowerupPresets[0][iSetting];
                            } else if (set_block >= 11 && set_block <= 14) {
									g_map->objectdata[iClickX][iClickY].iSettings[0] = set_block_switch_on;
								}

								AdjustMapItems(iClickX, iClickY);
                        } else if (edit_mode == 1) {
                            for (short i = 0; i < set_tile_cols; i++) {
									short iLocalX = iClickX + i;

                                for (short j = 0; j < set_tile_rows; j++) {
										short iLocalY = iClickY + j;

                                    if (iLocalX >= 0 && iLocalX < MAPWIDTH && iLocalY >= 0 && iLocalY < MAPHEIGHT) {
											bool fNeedUpdate = !TileTypeIsModified(iLocalX, iLocalY);

											SetTilesetTile(&g_map->mapdata[iLocalX][iLocalY][selected_layer], set_tile_tileset, set_tile_start_x + i, set_tile_start_y + j);

											if (fNeedUpdate)
												UpdateTileType(iLocalX, iLocalY);

											AdjustMapItems(iLocalY, iLocalY);
										}
									}
								}
                        } else if (edit_mode == 2) {
								g_map->warpdata[iClickX][iClickY].direction = (WarpEnterDirection)set_direction;
								g_map->warpdata[iClickX][iClickY].connection = set_connection;
                        } else if (edit_mode == 3) {
                            if (move_mode == 0) {
									selectedtiles[iClickX][iClickY] = true;
								}
                        } else if (edit_mode == 4) {
								SetNoSpawn(nospawn_mode, iClickX, iClickY, true);
                        } else if (edit_mode == 5) {
								g_map->nospawn[5][iClickX][iClickY] = true;
                        } else if (edit_mode == 6) {
								g_map->mapdatatop[iClickX][iClickY].iType = set_tiletype;
								AdjustMapItems(iClickX, iClickY);
                        } else if (edit_mode == 8) {
                            for (short i = 0; i < set_tile_cols; i++) {
									short iLocalX = iClickX + i;

                                for (short j = 0; j < set_tile_rows; j++) {
										short iLocalY = iClickY + j;

                                    if (iLocalX >= 0 && iLocalX < MAPWIDTH && iLocalY >= 0 && iLocalY < MAPHEIGHT) {
											bool fNeedUpdate = !TileTypeIsModified(iLocalX, iLocalY);
											SetTilesetTile(&g_map->mapdata[iLocalX][iLocalY][selected_layer], TILESETANIMATED, set_tile_start_y + j, set_tile_start_x + i);

											if (fNeedUpdate)
												UpdateTileType(iLocalX, iLocalY);
										}
									}
								}
							}
                    } else if (event.motion.state == SDL_BUTTON(SDL_BUTTON_RIGHT)) {
							if (edit_mode == 0)
								g_map->objectdata[iClickX][iClickY].iType = -1;
                        else if (edit_mode == 1 || edit_mode == 8) {
								bool fNeedUpdate = !TileTypeIsModified(iClickX, iClickY);
								g_map->mapdata[iClickX][iClickY][selected_layer].iID = TILESETNONE;

								if (fNeedUpdate)
									UpdateTileType(iClickX, iClickY);
                        } else if (edit_mode == 2) {
								g_map->warpdata[iClickX][iClickY].direction = WARP_UNDEFINED;
								g_map->warpdata[iClickX][iClickY].connection = -1;
                        } else if (edit_mode == 3) {
								if (move_mode == 0)
									selectedtiles[iClickX][iClickY] = false;
                        } else if (edit_mode == 4) {
								SetNoSpawn(nospawn_mode, iClickX, iClickY, false);
                        } else if (edit_mode == 5) {
								g_map->nospawn[5][iClickX][iClickY] = false;
                        } else if (edit_mode == 6) {
								g_map->mapdatatop[iClickX][iClickY].iType = tile_nonsolid;
							}
						}

						break;
					}

                case SDL_MOUSEBUTTONUP: {
						short iClickX = event.button.x / TILESIZE;
						short iClickY = event.button.y / TILESIZE;

                    if (event.button.button == SDL_BUTTON_LEFT) {
							ignoreclick = false;

                        if (move_mode == 1) {
								pasteselectedtiles(move_offset_x, move_offset_y);

								copymoveselection();
								resetselectedtiles();
								pastemoveselection(move_offset_x, move_offset_y);

								move_offset_x = 0;
								move_offset_y = 0;
                        } else if (move_mode == 2) {
								int left = move_drag_start_x < iClickX ? move_drag_start_x : iClickX;
								int top = move_drag_start_y < iClickY ? move_drag_start_y : iClickY;
								int right = move_drag_start_x < iClickX ? iClickX : move_drag_start_x;
								int bottom = move_drag_start_y < iClickY ? iClickY : move_drag_start_y;

                            for (int k = top; k <= bottom; k++) {
                                for (int j = left; j <= right; j++) {
										selectedtiles[j][k] = true;
									}
								}
							}

							move_mode = 0;
						}

						break;
					}

					default:
						break;
				}
			}

            if (move_mode == 1 || move_mode == 3) {
				int mousex, mousey;
				SDL_GetMouseState(&mousex, &mousey);
				move_offset_x = (mousex / TILESIZE) - move_start_x;
				move_offset_y = (mousey / TILESIZE) - move_start_y;
            } else if (move_mode == 2) {
				int mousex, mousey;
				SDL_GetMouseState(&mousex, &mousey);
				move_drag_offset_x = (mousex / TILESIZE);
				move_drag_offset_y = (mousey / TILESIZE);
			}

		}

        if (maplist->GetValid()) {
			drawmap(false, TILESIZE);
        } else {
			SDL_FillRect(screen, NULL, 0x0);
			rm->menu_font_large.drawCentered(320, 200, "Map has been deleted.");
		}

		//Ask if you are sure you want to exit
        if (fExiting) {
			rm->spr_dialog.draw(224, 176, 0, 0, 192, 128);
			rm->menu_font_large.drawCentered(320, 195, "Exit");
			rm->menu_font_large.drawCentered(320, 220, "Are You Sure?");
			rm->menu_font_large.drawCentered(282, 254, "Yes");
			rm->menu_font_large.drawCentered(356, 254, "No");

			rm->spr_dialog.draw(fSelectedYes ? 250 : 326, 250, 192, 0, 64, 32);
        } else {
            if (edit_mode == 0) {
				rm->menu_font_small.draw(0,0, "Block Mode");
            } else if (edit_mode == 1 || edit_mode == 8) {
				char modestring[128] = "";

				if (edit_mode == 1)
					strcpy(modestring, "Tile Mode - ");
				else
					strcpy(modestring, "Animated Tile Mode - ");

				if (selected_layer == 0)
					strcat(modestring, "Bottom Background");
				else if (selected_layer == 1)
					strcat(modestring, "Top Background");
				else if (selected_layer == 2)
					strcat(modestring, "Bottom Foreground");
				else if (selected_layer == 3)
					strcat(modestring, "Top Foreground");

				if (view_only_layer)
					strcat(modestring, " Only");

				rm->menu_font_small.draw(0,0, modestring);

				if (view_only_layer)
					rm->spr_backgroundlevel.draw(2, 18 + (3 - selected_layer) * 18, selected_layer * 16, (3 - selected_layer) * 18, 16, 16);
				else
					rm->spr_backgroundlevel.draw(2, 18, selected_layer * 16, 0, 16, 70);
            } else if (edit_mode == 2) {
				rm->menu_font_small.draw(0,0, "Warp Mode");
            } else if (edit_mode == 3) {
                for (int k = 0; k < MAPHEIGHT; k++) {
                    for (int j = 0; j < MAPWIDTH; j++) {
						if (selectedtiles[j][k])
							rm->spr_selectedtile.draw((j + move_offset_x) * TILESIZE, (k + move_offset_y) * TILESIZE);
					}
				}

				//Draw dragging selection
                if (move_mode == 2) {
					int left = move_drag_start_x < move_drag_offset_x ? move_drag_start_x : move_drag_offset_x;
					int top = move_drag_start_y < move_drag_offset_y ? move_drag_start_y : move_drag_offset_y;
					int right = move_drag_start_x < move_drag_offset_x ? move_drag_offset_x : move_drag_start_x;
					int bottom = move_drag_start_y < move_drag_offset_y ? move_drag_offset_y : move_drag_start_y;

                    for (int k = top; k <= bottom; k++) {
                        for (int j = left; j <= right; j++) {
							if (!selectedtiles[j][k])
								rm->spr_selectedtile.draw(j * TILESIZE, k * TILESIZE);
						}
					}
				}

				if (move_replace)
					rm->menu_font_small.draw(0,0, "Move Mode - Replace");
				else
					rm->menu_font_small.draw(0,0, "Move Mode - Merge");

				if (view_only_layer)
					rm->spr_backgroundlevel.draw(2, 18 + (3 - selected_layer) * 18, selected_layer * 16, (3 - selected_layer) * 18, 16, 16);
				else
					rm->spr_backgroundlevel.draw(2, 18, selected_layer * 16, 0, 16, 70);
            } else if (edit_mode == 4) {
                for (int k = 0; k < MAPHEIGHT; k++) {
                    for (int j = 0; j < MAPWIDTH; j++) {
						if (g_map->nospawn[nospawn_mode][j][k])
							rm->spr_nospawntile.draw(j * TILESIZE, k * TILESIZE, nospawn_mode * 32, 0, 32, 32);

                        if (nospawn_mode > 0) {
							if (g_map->nospawn[0][j][k])
								rm->spr_nospawntile.draw(j * TILESIZE, k * TILESIZE, 0, 0, 32, 32);
						}
					}
				}

				rm->menu_font_small.draw(0, 480 - (rm->menu_font_small.getHeight() << 1), "No Player Spawn: [x] Global, [1-4] Team Spawn Zone");
				rm->menu_font_small.draw(0, 480 - rm->menu_font_small.getHeight(), "[a] All, [n] None, [i] Invert");
            } else if (edit_mode == 5) {
                for (int k = 0; k < MAPHEIGHT; k++) {
                    for (int j = 0; j < MAPWIDTH; j++) {
						if (g_map->nospawn[5][j][k])
							rm->spr_noitemspawntile.draw(j * TILESIZE, k  * TILESIZE);
					}
				}

				rm->menu_font_small.draw(0, 480 - rm->menu_font_small.getHeight(), "No Item Spawn");
            } else if (edit_mode == 6) {
                for (int k = 0; k < MAPHEIGHT; k++) {
                    for (int j = 0; j < MAPWIDTH; j++) {
						if (g_map->mapdatatop[j][k].iType != tile_nonsolid)
							rm->spr_transparenttiles.draw(j * TILESIZE, k * TILESIZE, (g_map->mapdatatop[j][k].iType - 1) * TILESIZE, 0, TILESIZE, TILESIZE);
					}
				}

				rm->menu_font_small.draw(0, 0, "Tile Type Mode");
				rm->menu_font_small.draw(0, 480 - rm->menu_font_small.getHeight(), "Press [Delete] To Clear All Tile Types");
            } else if (edit_mode == 7) {
				rm->menu_font_small.draw(0, 0, "Map Item Mode");
			}

			rm->menu_font_small.drawRightJustified(640, 0, maplist->currentFilename());

            if (g_musiccategorydisplaytimer > 0) {
				--g_musiccategorydisplaytimer;

				rm->spr_dialog.draw(224, 176, 0, 0, 192, 128);
				rm->menu_font_small.drawCentered(320, 195, "Music Category");
				rm->menu_font_large.drawCentered(320, 220, g_szMusicCategoryNames[g_map->musicCategoryID]);

				rm->menu_font_small.drawCentered(320, 255, "Press 'R' Again");
				rm->menu_font_small.drawCentered(320, 270, "To Change");
			}

			DrawMessage();
		}

		return EDITOR_EDIT;
}

void DrawMessage()
{
    if (g_messagedisplaytimer > 0) {
		--g_messagedisplaytimer;

		rm->spr_dialog.draw(224, 176, 0, 0, 192, 128);
		rm->menu_font_large.drawCentered(320, 195, g_szMessageTitle.c_str());
		rm->menu_font_large.drawCentered(320, 220, g_szMessageLine[0].c_str());
		rm->menu_font_large.drawCentered(320, 240, g_szMessageLine[1].c_str());
		rm->menu_font_large.drawCentered(320, 260, g_szMessageLine[2].c_str());
	}
}

void SetNoSpawn(short nospawnmode, short col, short row, bool value)
{
	if (!value && nospawnmode > 0 && g_map->nospawn[0][col][row])
		return;

	g_map->nospawn[nospawnmode][col][row] = value;

    if (nospawnmode == 0) {
        for (short imode = 1; imode < 5; imode++) {
			g_map->nospawn[imode][col][row] = value;
		}
	}
}

void drawlayer(int layer, bool fUseCopied, short iBlockSize)
{
	short iTilesetIndex = iBlockSize == TILESIZE ? 0 : iBlockSize == PREVIEWTILESIZE ? 1 : 2;

	//draw left to right full vertical
    for (short i = 0; i < MAPWIDTH; i++) {
        for (short j = 0; j < MAPHEIGHT; j++) {
			TilesetTile * tile = NULL;
			if ((move_mode == 1 || move_mode == 3) && i - move_offset_x >= 0 && i - move_offset_x < MAPWIDTH &&
				j - move_offset_y >= 0 && j - move_offset_y < MAPHEIGHT &&
                    selectedtiles[i - move_offset_x][j - move_offset_y]) {
				if (fUseCopied)
					tile = &copiedtiles[i - move_offset_x][j - move_offset_y].tile[layer];
            } else {
				if (!fUseCopied)
					tile = &g_map->mapdata[i][j][layer];
			}

			if (!tile || tile->iID == TILESETNONE)
				continue;

            if (tile->iID >= 0) {
				g_tilesetmanager->Draw(screen, tile->iID, iTilesetIndex, tile->iCol, tile->iRow, i, j);
				//SDL_BlitSurface(g_tilesetmanager->GetTileset(tile->iID)->GetSurface(iTilesetIndex), &g_tilesetmanager->rRects[iTilesetIndex][tile->iCol][tile->iRow], screen, &bltrect);
            } else if (tile->iID == TILESETANIMATED) {
				short iSrcCol = tile->iCol << 2;
				short iSrcRow = tile->iRow;

                if (iSrcCol > 31 || iSrcCol < 0 || iSrcRow > 31 || iSrcRow < 0) {
					iSrcCol = 0;
					iSrcRow = 0;
				}

				SDL_BlitSurface(rm->spr_tileanimation[iTilesetIndex].getSurface(), &g_tilesetmanager->rRects[iTilesetIndex][iSrcCol][iSrcRow], screen, &g_tilesetmanager->rRects[iTilesetIndex][i][j]);
            } else if (tile->iID == TILESETUNKNOWN) {
				SDL_BlitSurface(rm->spr_unknowntile[iTilesetIndex].getSurface(), &g_tilesetmanager->rRects[iTilesetIndex][0][0], screen, &g_tilesetmanager->rRects[iTilesetIndex][i][j]);
			}
		}
	}
}

void drawmap(bool fScreenshot, short iBlockSize, bool fWithPlatforms)
{
    if (iBlockSize != TILESIZE) {
		SDL_Rect srcrect;
		srcrect.x = 0;
		srcrect.y = 0;
		srcrect.w = 640;
		srcrect.h = 480;

		SDL_Rect dstrect;
		dstrect.x = 0;
		dstrect.y = 0;
		dstrect.w = iBlockSize * 20;
		dstrect.h = iBlockSize * 15;

        if (SDL_SCALEBLIT(rm->spr_background.getSurface(), &srcrect, blitdest, &dstrect) < 0) {
			fprintf(stderr, "SDL_SCALEBLIT error: %s\n", SDL_GetError());
			return;
		}
    } else {
		rm->spr_background.draw(0,0);
	}

	if ((view_only_layer && selected_layer == 0) || !view_only_layer)
		drawlayer(0, false, iBlockSize);

    if (!fScreenshot) {
		if ((view_only_layer && copiedlayer == 0) || !view_only_layer)
			drawlayer(0, true, iBlockSize);
	}

	if ((view_only_layer && selected_layer == 1) || !view_only_layer)
		drawlayer(1, false, iBlockSize);

    if (!fScreenshot) {
		if ((view_only_layer && copiedlayer == 1) || !view_only_layer)
			drawlayer(1, true, iBlockSize);
	}

	if (fWithPlatforms)
		g_map->drawPlatforms(0);

    if ((viewblocks && !view_only_layer) || fScreenshot) {
		short iTilesizeIndex = iBlockSize == 32 ? 0 : iBlockSize == 16 ? 1 : 2;

		SDL_Rect rSrc = {0, 0, iBlockSize, iBlockSize};

		MapBlock * block = NULL;
        for (int j = 0; j < MAPHEIGHT; j++) {
            for (int i = 0; i < MAPWIDTH; i++) {
				if ((move_mode == 1 || move_mode == 3) && i - move_offset_x >= 0 && i - move_offset_x < MAPWIDTH &&
					j - move_offset_y >= 0 && j - move_offset_y < MAPHEIGHT &&
                        selectedtiles[i - move_offset_x][j - move_offset_y]) {
					block = &copiedtiles[i - move_offset_x][j - move_offset_y].block;
                } else {
					block = &g_map->objectdata[i][j];
				}

				int blocktype = block->iType;

                if (blocktype > -1) {
					//Don't screenshot hidden blocks
					if (fScreenshot && g_map->objectdata[i][j].fHidden)
						continue;

                    if (blocktype < 7) {
						rSrc.x = blocktype * iBlockSize;
						rSrc.y = 0;
                    } else if (blocktype >= 7 && blocktype <= 10) { //On/Off Blocks
						rSrc.x = blocktype * iBlockSize;
						rSrc.y = iBlockSize * (1 - g_map->iSwitches[(blocktype - 7) % 4]);
                    } else if (blocktype >= 11 && blocktype <= 14) { //Switched Blocks
						rSrc.x = blocktype * iBlockSize;
						rSrc.y = iBlockSize * (1 - block->iSettings[0]);
                    } else if (blocktype >= 15 && blocktype <= 19) {
						rSrc.x = (blocktype - 15) * iBlockSize;
						rSrc.y = iBlockSize;
                    } else if (blocktype >= 20 && blocktype <= 29) {
						rSrc.x = (blocktype - 20) * iBlockSize;
						rSrc.y = iBlockSize << 1;
					}

					SDL_BlitSurface(rm->spr_blocks[iTilesizeIndex].getSurface(), &rSrc, screen, &g_tilesetmanager->rRects[iTilesizeIndex][i][j]);
				}
			}
		}
	}

	if (fWithPlatforms)
		g_map->drawPlatforms(1);

    if (!view_only_layer || fScreenshot) {
        for (short j = 0; j < g_map->iNumMapItems; j++) {
			rm->spr_mapitems[iBlockSize == TILESIZE ? 0 : iBlockSize == PREVIEWTILESIZE ? 1 : 2].draw(g_map->mapitems[j].ix * iBlockSize, g_map->mapitems[j].iy * iBlockSize, g_map->mapitems[j].itype * iBlockSize, 0, iBlockSize, iBlockSize);
		}

        if (!fScreenshot) {
            for (int j = 0; j < MAPHEIGHT; j++) {
                for (int i = 0; i < MAPWIDTH; i++) {
					short iNewX = i - move_offset_x;
					short iNewY = j - move_offset_y;

					if ((move_mode == 1 || move_mode == 3) && iNewX >= 0 && iNewX < MAPWIDTH &&
						iNewY >= 0 && iNewY < MAPHEIGHT &&
                            selectedtiles[iNewX][iNewY]) {
						if (copiedtiles[iNewX][iNewY].item >= 0)
							rm->spr_mapitems[0].draw(i << 5, j << 5, copiedtiles[iNewX][iNewY].item << 5, 0, TILESIZE, TILESIZE);
					}
				}
			}
		}
	}

	if (fWithPlatforms)
		g_map->drawPlatforms(2);

	if ((view_only_layer && selected_layer == 2) || !view_only_layer)
		drawlayer(2, false, iBlockSize);

    if (!fScreenshot) {
		if ((view_only_layer && copiedlayer == 2) || !view_only_layer)
			drawlayer(2, true, iBlockSize);
	}

	if ((view_only_layer && selected_layer == 3) || !view_only_layer)
		drawlayer(3, false, iBlockSize);

    if (!fScreenshot) {
		if ((view_only_layer && copiedlayer == 3) || !view_only_layer)
			drawlayer(3, true, iBlockSize);
	}

	if (fWithPlatforms)
		g_map->drawPlatforms(3);

    if ((viewwarps && !view_only_layer) || fScreenshot) {
        for (int j = 0; j < MAPHEIGHT; j++) {
            for (int i = 0; i < MAPWIDTH; i++) {
				Warp * warp = NULL;
				if ((move_mode == 1 || move_mode == 3) && i - move_offset_x >= 0 && i - move_offset_x < MAPWIDTH &&
					j - move_offset_y >= 0 && j - move_offset_y < MAPHEIGHT &&
                        selectedtiles[i - move_offset_x][j - move_offset_y]) {
					warp = &copiedtiles[i - move_offset_x][j - move_offset_y].warp;
                } else {
					warp = &g_map->warpdata[i][j];
				}

                if (warp->connection != -1) {
					SDL_Rect rSrc = {warp->connection * iBlockSize, warp->direction * iBlockSize, iBlockSize, iBlockSize};
					SDL_Rect rDst = {i * iBlockSize, j * iBlockSize, iBlockSize, iBlockSize};

					SDL_BlitSurface(rm->spr_warps[iBlockSize == TILESIZE ? 0 : iBlockSize == PREVIEWTILESIZE ? 1 : 2].getSurface(), &rSrc, screen, &rDst);
				}
			}
		}
	}

	if (fWithPlatforms)
		g_map->drawPlatforms(4);

	/*
	for (int k = 0; k < g_map->numwarpexits; k++)
	{
		SDL_Rect rSrc = {g_map->warpexits[k].connection * TILESIZE, g_map->warpexits[k].direction * TILESIZE, TILESIZE, TILESIZE};
		SDL_Rect rDst = {g_map->warpexits[k].x, g_map->warpexits[k].y, TILESIZE, TILESIZE};

		SDL_BlitSurface(rm->spr_warps[0].getSurface(), &rSrc, screen, &rDst);
	}
	*/
}

SDL_Rect r;

int editor_warp()
{
		//handle messages
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
				case SDL_QUIT:
					return EDITOR_QUIT;

				case SDL_KEYDOWN:
					edit_mode = 2;  //change to edit mode using warps
					return EDITOR_EDIT;
				break;

				case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    if (event.button.x / TILESIZE < 10 && event.button.y / TILESIZE < 4) {
							set_direction = event.button.y / TILESIZE;
							set_connection = event.button.x / TILESIZE;
						}

						edit_mode = 2;  //change to edit mode using warps

						//The user must release the mouse button before trying to add a tile
						ignoreclick = true;

						return EDITOR_EDIT;
					}
				break;

				default:
					break;
			}
		}


		drawmap(false, TILESIZE);
		rm->menu_shade.draw(0, 0);

        r.x = 0;
        r.y = 0;
        r.w = 640;
        r.h = 480;

		SDL_BlitSurface(rm->spr_warps[0].getSurface(), NULL, screen, &r);
		rm->menu_font_small.drawRightJustified(640, 0, maplist->currentFilename());

		DrawMessage();
        return EDITOR_WARP;
}

#define NUM_EYECANDY 7
const char * szEyecandyNames[NUM_EYECANDY] = {"Clouds", "Ghosts", "Leaves", "Snow", "Fish", "Rain", "Bubbles"};
const char * szLayerNames[3] = {"Back Layer", "Mid Layer", "Top Layer"};
int editor_eyecandy()
{
		//handle messages
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
				return EDITOR_QUIT;

            case SDL_KEYDOWN: {
                if (event.key.keysym.sym == SDLK_ESCAPE || event.key.keysym.sym == SDLK_e) {
						return EDITOR_EDIT;
					}
				}

            case SDL_MOUSEBUTTONDOWN: {
                if (event.button.button == SDL_BUTTON_LEFT) {
						short ix = 165;
                    for (short iLayer = 0; iLayer < 3; iLayer++) {
                        for (short k = 0; k < NUM_EYECANDY; k++) {
								if (event.button.x >= ix && event.button.x < ix + 90 &&
                                    event.button.y >= k * 65 + 20 && event.button.y < k * 65 + 72) {
									short mask = 1 << k;
									if (g_map->eyecandy[iLayer] & mask)
										g_map->eyecandy[iLayer] &= ~mask;
									else
										g_map->eyecandy[iLayer] |= mask;
								}
							}

							ix += 110;
						}
					}

					break;
				}

				default:
					break;
			}
		}


		drawmap(false, TILESIZE);
		rm->menu_shade.draw(0, 0);

		int iMouseX, iMouseY;
		SDL_GetMouseState(&iMouseX, &iMouseY);

		short ix = 165;
        for (short iLayer = 0; iLayer < 3; iLayer++) {
            for (short k = 0; k < NUM_EYECANDY; k++) {
				short iy = k * 65 + 20;

				if (iMouseX >= ix && iMouseX < ix + 90 && iMouseY >= iy && iMouseY < iy + 52)
					rm->spr_powerupselector.draw(ix, iy, 0, 0, 90, 52);
				else
					rm->spr_powerupselector.draw(ix, iy, 0, 52, 90, 52);

				rm->spr_eyecandy.draw(ix + 10, iy + 10, k << 5, 0, 32, 32);

				short mask = 1 << k;
				if (g_map->eyecandy[iLayer] & mask)
					rm->spr_hidden_marker.draw(ix + 57, iy + 16);

				//rm->menu_font_small.drawCentered(320, iy - rm->menu_font_small.getHeight() + 7, szEyecandyNames[k]);
			}

			rm->menu_font_small.drawCentered(ix + 45, 10, szLayerNames[iLayer]);
			ix += 110;
		}

        for (short k = 0; k < NUM_EYECANDY; k++) {
			//rm->menu_font_small.drawRightJustified(160, k * 65 + 40, szEyecandyNames[k]);
			rm->menu_font_small.drawCentered(320, k * 65 + 62, szEyecandyNames[k]);
		}

		rm->menu_font_small.draw(0,480-rm->menu_font_small.getHeight(), "Eyecandy: [e] Exit, [LMB] Choose Eyecandy");
		//rm->menu_font_small.drawRightJustified(640, 0, maplist->currentFilename());

		DrawMessage();
		return EDITOR_EYECANDY;
}

short * GetBlockProperty(short x, short y, short iBlockCol, short iBlockRow, short * iSettingIndex)
{
    for (short iSetting = 0; iSetting < NUM_BLOCK_SETTINGS; iSetting++) {
		short ix = (iSetting % 6) * 100 + 35;
		short iy = (iSetting / 6) * 62 + 65;

        if (x >= ix - 10 && x < ix + 80 && y >= iy - 10 && y < iy + 42) {
			if (iSettingIndex)
				*iSettingIndex = iSetting;

			return &g_map->objectdata[iBlockCol][iBlockRow].iSettings[iSetting];
		}
	}

	return NULL;
}

short iBlockType;
bool editor_properties_initialized = false;
void init_editor_properties(short iBlockCol, short iBlockRow)
{
    if (editor_properties_initialized)
        return;

    iBlockType = g_map->objectdata[iBlockCol][iBlockRow].iType;

    editor_properties_initialized = true;
}

int editor_properties(short iBlockCol, short iBlockRow)
{
    init_editor_properties(iBlockCol, iBlockRow);

		//handle messages
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
				return EDITOR_QUIT;

            case SDL_KEYDOWN: {
                if (event.key.keysym.sym == SDLK_ESCAPE || event.key.keysym.sym == SDLK_k) {
                    editor_properties_initialized = false;
					return EDITOR_EDIT;
                } else if ((iBlockType == 1 || iBlockType == 15) && ((event.key.keysym.sym >= SDLK_0 && event.key.keysym.sym <= SDLK_9) || event.key.keysym.sym == SDLK_BACKQUOTE || event.key.keysym.sym == SDLK_d)) {
						int iMouseX, iMouseY;
						SDL_GetMouseState(&iMouseX, &iMouseY);

						short iSettingIndex = 0;
						short * piSetting = GetBlockProperty(iMouseX, iMouseY, iBlockCol, iBlockRow, &iSettingIndex);

						//If shift is held, set all powerups to this setting
						short iValue = event.key.keysym.sym - SDLK_0;

						if (event.key.keysym.sym == SDLK_0)
							iValue = 10;
						else if (event.key.keysym.sym == SDLK_BACKQUOTE)
							iValue = 0;
						else if (event.key.keysym.sym == SDLK_d)
							iValue = g_iDefaultPowerupPresets[0][iSettingIndex];

                    #if defined(USE_SDL2) || defined(__EMSCRIPTEN__)
                        const Uint8 * keystate = SDL_GetKeyboardState(NULL);
                    #else
                        Uint8 * keystate = SDL_GetKeyState(NULL);
                    #endif
                    if (keystate[SDLK_LSHIFT] || keystate[SDLK_RSHIFT]) {
                        for (short iSetting = 0; iSetting < NUM_BLOCK_SETTINGS; iSetting++) {
								if (event.key.keysym.sym == SDLK_d)
									iValue = g_iDefaultPowerupPresets[0][iSetting];

								g_map->objectdata[iBlockCol][iBlockRow].iSettings[iSetting] = iValue;
							}
                    } else if (piSetting) {
							*piSetting = iValue;
						}
					}
				}

            case SDL_MOUSEBUTTONDOWN: {
					short iHiddenCheckboxY = 0;
                if (iBlockType == 1 || iBlockType == 15) {
						short iSettingIndex;
						short * piSetting = GetBlockProperty(event.button.x, event.button.y, iBlockCol, iBlockRow, &iSettingIndex);

                    if (piSetting) {
							int iMouseX, iMouseY;
							Uint8 iMouseState = SDL_GetMouseState(&iMouseX, &iMouseY);

							if ((event.button.button == SDL_BUTTON_RIGHT && (iMouseState & SDL_BUTTON_LMASK)) ||
                                (event.button.button == SDL_BUTTON_LEFT && (iMouseState & SDL_BUTTON_RMASK))) {
								*piSetting = g_iDefaultPowerupPresets[0][iSettingIndex];
                        } else if (event.button.button == SDL_BUTTON_LEFT) {
								if (*piSetting < 10)
									(*piSetting)++;
                        } else if (event.button.button == SDL_BUTTON_RIGHT) {
								if (*piSetting > 0)
									(*piSetting)--;
							}
						}

						iHiddenCheckboxY = 365;
                } else if (iBlockType == 4 || iBlockType == 5 || iBlockType == 17 || iBlockType == 18 || iBlockType == 3) {
						iHiddenCheckboxY = 214;
					}

                if (event.button.x >= 270 && event.button.x < 370 && event.button.y >= iHiddenCheckboxY && event.button.y < iHiddenCheckboxY + 52) {
                    if (event.button.button == SDL_BUTTON_LEFT) {
							g_map->objectdata[iBlockCol][iBlockRow].fHidden = !g_map->objectdata[iBlockCol][iBlockRow].fHidden;
						}
					}

                if (iBlockType == 1 || iBlockType == 15) {
                    if (event.button.x >= 390 && event.button.x < 490 && event.button.y >= iHiddenCheckboxY && event.button.y < iHiddenCheckboxY + 52) {
                        if (event.button.button == SDL_BUTTON_LEFT) {
								if (g_map->objectdata[iBlockCol][iBlockRow].iSettings[0] >= 0)
									g_map->objectdata[iBlockCol][iBlockRow].iSettings[0] = -1;
								else
									g_map->objectdata[iBlockCol][iBlockRow].iSettings[0] = g_iDefaultPowerupPresets[0][0];
							}
						}
					}


					break;
				}

				default:
					break;
			}
		}


		drawmap(false, TILESIZE);
		rm->menu_shade.draw(0, 0);

		int iMouseX, iMouseY;
		SDL_GetMouseState(&iMouseX, &iMouseY);

		short iHiddenCheckboxY = 0;

        if (iBlockType == 1 || iBlockType == 15) {
			bool fUseGame = g_map->objectdata[iBlockCol][iBlockRow].iSettings[0] == -1;

            for (short iSetting = 0; iSetting < NUM_BLOCK_SETTINGS; iSetting++) {
				short ix = (iSetting % 6) * 100 + 35;
				short iy = (iSetting / 6) * 62 + 65;

				if (iMouseX >= ix - 10 && iMouseX < ix + 80 && iMouseY >= iy - 10 && iMouseY < iy + 42 && !fUseGame)
					rm->spr_powerupselector.draw(ix - 10, iy - 10, 0, 0, 90, 52);
				else
					rm->spr_powerupselector.draw(ix - 10, iy - 10, 0, 52, 90, 52);

				rm->spr_powerups.draw(ix, iy, iSetting << 5, 0, 32, 32);

				char szNum[8];
				if (fUseGame)
					sprintf(szNum, "X");
				else
					sprintf(szNum, "%d", g_map->objectdata[iBlockCol][iBlockRow].iSettings[iSetting]);

				rm->menu_font_large.drawCentered(ix + 55, iy + 5, szNum);
			}

			iHiddenCheckboxY = 365;

			rm->menu_font_small.draw(0,480-rm->menu_font_small.getHeight() * 3, "Block Property Mode");
			rm->menu_font_small.draw(0,480-rm->menu_font_small.getHeight() * 2, "[~, 0-9] Set Value [LMB] Increase [RMB] Decrease [D] Default");
			rm->menu_font_small.draw(0,480-rm->menu_font_small.getHeight(), "[Shift] + [0-9 or D] Set All To Value");
        } else if (iBlockType == 4 || iBlockType == 5 || iBlockType == 17 || iBlockType == 18 || iBlockType == 3) {
			rm->menu_font_small.draw(0,480-rm->menu_font_small.getHeight(), "Block Property Mode");
			iHiddenCheckboxY = 214;
		}

		if (iMouseX >= 270 && iMouseX < 370 && iMouseY >= iHiddenCheckboxY && iMouseY < iHiddenCheckboxY + 52)
			rm->spr_powerupselector.draw(270, iHiddenCheckboxY, 90, 0, 100, 52);
		else
			rm->spr_powerupselector.draw(270, iHiddenCheckboxY, 90, 52, 100, 52);

		rm->menu_font_large.drawCentered(320, iHiddenCheckboxY + 3, "Hidden");

		if (g_map->objectdata[iBlockCol][iBlockRow].fHidden)
			rm->spr_hidden_marker.draw(310, iHiddenCheckboxY + 27);

		//Display "Use Game" option
        if (iBlockType == 1 || iBlockType == 15) {
			if (iMouseX >= 390 && iMouseX < 490 && iMouseY >= iHiddenCheckboxY && iMouseY < iHiddenCheckboxY + 52)
				rm->spr_powerupselector.draw(390, iHiddenCheckboxY, 90, 0, 100, 52);
			else
				rm->spr_powerupselector.draw(390, iHiddenCheckboxY, 90, 52, 100, 52);

			rm->menu_font_large.drawCentered(440, iHiddenCheckboxY + 3, "Use Game");

			if (g_map->objectdata[iBlockCol][iBlockRow].iSettings[0] == -1)
				rm->spr_hidden_marker.draw(430, iHiddenCheckboxY + 27);
		}


		rm->menu_font_small.drawRightJustified(640, 0, maplist->currentFilename());

		DrawMessage();
		return EDITOR_PROPERTIES;
}

//SDL_Rect r;
SDL_Rect rNewButton[2];
SDL_Rect rTypeButton[8][4];

const char * szPathNames[3] = {"Line Segment", "Continuous", "Ellipse"};

enum {PLATFORM_EDIT_STATE_SELECT, PLATFORM_EDIT_STATE_PATH_TYPE, PLATFORM_EDIT_STATE_CHANGE_PATH_TYPE, PLATFORM_EDIT_STATE_EDIT, PLATFORM_EDIT_STATE_PATH, PLATFORM_EDIT_STATE_TEST, PLATFORM_EDIT_STATE_TILETYPE, PLATFORM_EDIT_STATE_ANIMATED, PLATFORM_EDIT_STATE_MOVE};

short iPlatformEditState = PLATFORM_EDIT_STATE_SELECT;
short iPlatformSwitchState = 0, iPlatformSwitchIndex = 0;
short iEditPlatform = 0;
short iPlatformTop, iPlatformLeft, iPlatformWidth, iPlatformHeight;

short iPlatformPreview = -1;

bool editor_platforms_initialized = false;
void init_editor_platforms()
{
    if (editor_platforms_initialized)
        return;

    r.x = 192;
    r.y = 128;
    r.w = 256;
    r.h = 224;

    rNewButton[0].x = 0;
    rNewButton[0].y = 352;
    rNewButton[0].w = 76;
    rNewButton[0].h = 32;

    rNewButton[1].x = r.x + (r.w >> 1) - (rNewButton[0].w >> 1);
    rNewButton[1].y = r.y + r.h - 64;
    rNewButton[1].w = 76;
    rNewButton[1].h = 32;

    for (short iType = 0; iType < 3; iType++) {
        rTypeButton[iType][0].x = 0;
        rTypeButton[iType][0].y = 0;
        rTypeButton[iType][0].w = 192;
        rTypeButton[iType][0].h = 32;

        rTypeButton[iType][1].x = 320 - (rTypeButton[iType][0].w >> 1);
        rTypeButton[iType][1].y = 180 + iType * 40;
        rTypeButton[iType][1].w = 192;
        rTypeButton[iType][1].h = 32;

        rTypeButton[iType][2].x = 24 * iType;
        rTypeButton[iType][2].y = 32;
        rTypeButton[iType][2].w = 24;
        rTypeButton[iType][2].h = 24;

        rTypeButton[iType][3].x = rTypeButton[iType][1].x + 8;
        rTypeButton[iType][3].y = rTypeButton[iType][1].y + 4;
        rTypeButton[iType][3].w = 24;
        rTypeButton[iType][3].h = 24;
    }

    editor_platforms_initialized = true;
}

// TODO: Check this for problems.
int editor_platforms()
{
    init_editor_platforms();

		//handle messages
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
				return EDITOR_QUIT;

            case SDL_KEYDOWN: {
                if (event.key.keysym.sym == SDLK_s) {
						savecurrentmap();
                } else if (event.key.keysym.sym >= SDLK_1 && event.key.keysym.sym <= SDLK_9) {
                    if (PLATFORM_EDIT_STATE_SELECT == iPlatformEditState && event.key.keysym.sym - SDLK_1 < g_iNumPlatforms) {
							iEditPlatform = event.key.keysym.sym - SDLK_1;
							iPlatformEditState = PLATFORM_EDIT_STATE_EDIT;
                    } else if ((PLATFORM_EDIT_STATE_PATH_TYPE == iPlatformEditState || PLATFORM_EDIT_STATE_CHANGE_PATH_TYPE == iPlatformEditState) && event.key.keysym.sym >= SDLK_1 && event.key.keysym.sym <= SDLK_3) {
							g_Platforms[iEditPlatform].iPathType = event.key.keysym.sym - SDLK_1;

                        if (PLATFORM_EDIT_STATE_PATH_TYPE == iPlatformEditState) {
								iPlatformEditState = PLATFORM_EDIT_STATE_EDIT;
								SetPlatformToDefaults(iEditPlatform);
                        } else {
								iPlatformEditState = PLATFORM_EDIT_STATE_PATH;
							}
                    } else if (PLATFORM_EDIT_STATE_MOVE == iPlatformEditState && event.key.keysym.sym - SDLK_1 < g_iNumPlatforms) {
                        if (iPlatformSwitchState == 1) {
								SwitchPlatforms(iPlatformSwitchIndex, event.key.keysym.sym - SDLK_1);

								iPlatformPreview = -1;

								iPlatformSwitchState = 0;
								iPlatformEditState = PLATFORM_EDIT_STATE_SELECT;
                        } else {
								iPlatformSwitchState = 1;
								iPlatformSwitchIndex = event.key.keysym.sym - SDLK_1;
							}
						}
                } else if (event.key.keysym.sym == SDLK_n) {
                    if (PLATFORM_EDIT_STATE_SELECT == iPlatformEditState && g_iNumPlatforms < MAX_PLATFORMS) {
							iEditPlatform = g_iNumPlatforms;
							g_iNumPlatforms++;
							iPlatformEditState = PLATFORM_EDIT_STATE_PATH_TYPE;
						}
                } else if (event.key.keysym.sym == SDLK_m) {
                    if (PLATFORM_EDIT_STATE_SELECT == iPlatformEditState) {
							iPlatformPreview = -1;
							iPlatformEditState = PLATFORM_EDIT_STATE_MOVE;
							iPlatformSwitchState = 0;
						}
                } else if (event.key.keysym.sym == SDLK_t) {
                    if (PLATFORM_EDIT_STATE_EDIT == iPlatformEditState || PLATFORM_EDIT_STATE_ANIMATED == iPlatformEditState || PLATFORM_EDIT_STATE_TILETYPE == iPlatformEditState) {
							editor_tiles();
							iPlatformEditState = PLATFORM_EDIT_STATE_EDIT;
                    } else if (PLATFORM_EDIT_STATE_PATH == iPlatformEditState) {
							iPlatformEditState = PLATFORM_EDIT_STATE_CHANGE_PATH_TYPE;
						}
                } else if (event.key.keysym.sym == SDLK_a) {
                    if (PLATFORM_EDIT_STATE_EDIT == iPlatformEditState || PLATFORM_EDIT_STATE_ANIMATED == iPlatformEditState || PLATFORM_EDIT_STATE_TILETYPE == iPlatformEditState) {
							editor_animation();
							iPlatformEditState = PLATFORM_EDIT_STATE_ANIMATED;
						}
                } else if (event.key.keysym.sym == SDLK_l) {
                    if (PLATFORM_EDIT_STATE_EDIT == iPlatformEditState || PLATFORM_EDIT_STATE_ANIMATED == iPlatformEditState || PLATFORM_EDIT_STATE_TILETYPE == iPlatformEditState) {
							editor_tiletype();
							iPlatformEditState = PLATFORM_EDIT_STATE_TILETYPE;
						}
                } else if (event.key.keysym.sym == SDLK_y) { //Change the draw layer
                    if (PLATFORM_EDIT_STATE_EDIT == iPlatformEditState || PLATFORM_EDIT_STATE_ANIMATED == iPlatformEditState || PLATFORM_EDIT_STATE_TILETYPE == iPlatformEditState) {
							if (++g_Platforms[iEditPlatform].iDrawLayer > 4)
								g_Platforms[iEditPlatform].iDrawLayer = 0;
						}
                } else if (event.key.keysym.sym == SDLK_c) {
                    if (PLATFORM_EDIT_STATE_SELECT == iPlatformEditState) {
							iPlatformEditState = PLATFORM_EDIT_STATE_TEST;
							insert_platforms_into_map();
							g_map->resetPlatforms();
						}
                } else if (event.key.keysym.sym == SDLK_DELETE) {
                    if (PLATFORM_EDIT_STATE_EDIT == iPlatformEditState || PLATFORM_EDIT_STATE_ANIMATED == iPlatformEditState || PLATFORM_EDIT_STATE_TILETYPE == iPlatformEditState) {
							//Copy platforms into empty spot
                        for (short iPlatform = iEditPlatform; iPlatform < g_iNumPlatforms - 1; iPlatform++) {
                            for (short iCol = 0; iCol < MAPWIDTH; iCol++) {
                                for (short iRow = 0; iRow < MAPHEIGHT; iRow++) {
										CopyTilesetTile(&g_Platforms[iPlatform].tiles[iCol][iRow], &g_Platforms[iPlatform + 1].tiles[iCol][iRow]);
										g_Platforms[iPlatform].types[iCol][iRow] = g_Platforms[iPlatform + 1].types[iCol][iRow];
									}
								}

								g_Platforms[iPlatform].iPathType = g_Platforms[iPlatform + 1].iPathType;

								g_Platforms[iPlatform].iVelocity = g_Platforms[iPlatform + 1].iVelocity;
								g_Platforms[iPlatform].iStartX = g_Platforms[iPlatform + 1].iStartX;
								g_Platforms[iPlatform].iStartY = g_Platforms[iPlatform + 1].iStartY;
								g_Platforms[iPlatform].iEndX = g_Platforms[iPlatform + 1].iEndX;
								g_Platforms[iPlatform].iEndY = g_Platforms[iPlatform + 1].iEndY;

								g_Platforms[iPlatform].fAngle = g_Platforms[iPlatform + 1].fAngle;
								g_Platforms[iPlatform].fRadiusX = g_Platforms[iPlatform + 1].fRadiusX;
								g_Platforms[iPlatform].fRadiusY = g_Platforms[iPlatform + 1].fRadiusY;

								g_Platforms[iPlatform].iDrawLayer = g_Platforms[iPlatform + 1].iDrawLayer;
							}

							g_iNumPlatforms--;
							iPlatformEditState = PLATFORM_EDIT_STATE_SELECT;
						}
                } else if (event.key.keysym.sym == SDLK_ESCAPE) {
                    if (PLATFORM_EDIT_STATE_SELECT == iPlatformEditState) {
                            editor_platforms_initialized = false;
							return EDITOR_EDIT;
                    } else if (PLATFORM_EDIT_STATE_EDIT == iPlatformEditState || PLATFORM_EDIT_STATE_ANIMATED == iPlatformEditState || PLATFORM_EDIT_STATE_MOVE == iPlatformEditState || PLATFORM_EDIT_STATE_TEST == iPlatformEditState) {
							iPlatformPreview = -1;
							g_Platforms[iEditPlatform].UpdatePreview();
							iPlatformEditState = PLATFORM_EDIT_STATE_SELECT;
                    } else if (PLATFORM_EDIT_STATE_PATH == iPlatformEditState || PLATFORM_EDIT_STATE_TILETYPE == iPlatformEditState) {
							iPlatformEditState = PLATFORM_EDIT_STATE_EDIT;
						}
                } else if (event.key.keysym.sym == SDLK_KP_MINUS || event.key.keysym.sym == SDLK_MINUS) {
                    if (PLATFORM_EDIT_STATE_EDIT == iPlatformEditState || PLATFORM_EDIT_STATE_ANIMATED == iPlatformEditState || PLATFORM_EDIT_STATE_TILETYPE == iPlatformEditState) {
                        if (g_Platforms[iEditPlatform].iPathType == 2) {
								if (g_Platforms[iEditPlatform].iVelocity > -10)
									g_Platforms[iEditPlatform].iVelocity--;

								if (g_Platforms[iEditPlatform].iVelocity == 0)
									g_Platforms[iEditPlatform].iVelocity--;
                        } else {
								if (g_Platforms[iEditPlatform].iVelocity > 2)
									g_Platforms[iEditPlatform].iVelocity--;
							}
						}
                } else if (event.key.keysym.sym == SDLK_KP_PLUS || event.key.keysym.sym == SDLK_EQUALS) {
                    if (PLATFORM_EDIT_STATE_EDIT == iPlatformEditState || PLATFORM_EDIT_STATE_ANIMATED == iPlatformEditState || PLATFORM_EDIT_STATE_TILETYPE == iPlatformEditState) {
                        if (g_Platforms[iEditPlatform].iPathType == 2) {
								if (g_Platforms[iEditPlatform].iVelocity < 10)
									g_Platforms[iEditPlatform].iVelocity++;

								if (g_Platforms[iEditPlatform].iVelocity == 0)
									g_Platforms[iEditPlatform].iVelocity++;
                        } else {
								if (g_Platforms[iEditPlatform].iVelocity < MAX_PLATFORM_VELOCITY)
									g_Platforms[iEditPlatform].iVelocity++;
							}
						}
                } else if (event.key.keysym.sym == SDLK_p) {
                    if (PLATFORM_EDIT_STATE_EDIT == iPlatformEditState || PLATFORM_EDIT_STATE_ANIMATED == iPlatformEditState || PLATFORM_EDIT_STATE_TILETYPE == iPlatformEditState) {
							iPlatformEditState = PLATFORM_EDIT_STATE_PATH;
							CalculatePlatformDims(iEditPlatform, &iPlatformLeft, &iPlatformTop, &iPlatformWidth, &iPlatformHeight);
							insert_platforms_into_map();
						}
					}

					break;
				}
            case SDL_MOUSEBUTTONDOWN: {
                if (event.button.button == SDL_BUTTON_LEFT && !ignoreclick) {
                    if (PLATFORM_EDIT_STATE_SELECT == iPlatformEditState || PLATFORM_EDIT_STATE_MOVE == iPlatformEditState) {
							//check clicks on existing platforms
                        for (int iPlatform = 0; iPlatform < g_iNumPlatforms; iPlatform++) {
								if (event.button.x >= g_Platforms[iPlatform].rIcon[1].x && event.button.x < g_Platforms[iPlatform].rIcon[1].x + g_Platforms[iPlatform].rIcon[1].w &&
                                    event.button.y >= g_Platforms[iPlatform].rIcon[1].y && event.button.y < g_Platforms[iPlatform].rIcon[1].y + g_Platforms[iPlatform].rIcon[1].h) {
                                if (PLATFORM_EDIT_STATE_SELECT == iPlatformEditState) {
										iEditPlatform = iPlatform;
										iPlatformEditState = PLATFORM_EDIT_STATE_EDIT;
										ignoreclick = true;
                                } else if (PLATFORM_EDIT_STATE_MOVE == iPlatformEditState) {
                                    if (iPlatformSwitchState == 1) {
											SwitchPlatforms(iPlatformSwitchIndex, iPlatform);

											iPlatformPreview = -1;

											iPlatformSwitchState = 0;
											iPlatformEditState = PLATFORM_EDIT_STATE_SELECT;
                                    } else {
											iPlatformSwitchState = 1;
											iPlatformSwitchIndex = iPlatform;
										}
									}
								}
							}

							if (PLATFORM_EDIT_STATE_SELECT == iPlatformEditState && g_iNumPlatforms < MAX_PLATFORMS && event.button.x >= rNewButton[1].x &&
                                event.button.x < rNewButton[1].x + rNewButton[1].w && event.button.y >= rNewButton[1].y && event.button.y < rNewButton[1].y + rNewButton[1].h) {
								//Create a new platform then edit it

								iEditPlatform = g_iNumPlatforms;
								g_iNumPlatforms++;
								iPlatformEditState = PLATFORM_EDIT_STATE_PATH_TYPE;
								ignoreclick = true;
							}
                    } else if (PLATFORM_EDIT_STATE_PATH_TYPE == iPlatformEditState || PLATFORM_EDIT_STATE_CHANGE_PATH_TYPE == iPlatformEditState) {
                        if (!ignoreclick) {
								short iClickX = event.button.x;
								short iClickY = event.button.y;

                            for (int iType = 0; iType < 3; iType++) {
									if (iClickX >= rTypeButton[iType][1].x && iClickX < rTypeButton[iType][1].x + rTypeButton[iType][1].w &&
                                        iClickY >= rTypeButton[iType][1].y && iClickY < rTypeButton[iType][1].y + rTypeButton[iType][1].h) {
										ignoreclick = true;

										g_Platforms[iEditPlatform].iPathType = iType;

                                    if (PLATFORM_EDIT_STATE_PATH_TYPE == iPlatformEditState) {
											iPlatformEditState = PLATFORM_EDIT_STATE_EDIT;
											SetPlatformToDefaults(iEditPlatform);
                                    } else {
											iPlatformEditState = PLATFORM_EDIT_STATE_PATH;
										}

										break;
									}
								}
							}
                    } else if (PLATFORM_EDIT_STATE_EDIT == iPlatformEditState) {
                        if (!ignoreclick) {
								short ix = event.button.x / TILESIZE;
								short iy = event.button.y / TILESIZE;

                            for (short i = 0; i < set_tile_cols; i++) {
                                for (short j = 0; j < set_tile_rows; j++) {
                                    if (ix + i >= 0 && ix + i < MAPWIDTH && iy + j >= 0 && iy + j < MAPHEIGHT) {
											TilesetTile * tile = &g_Platforms[iEditPlatform].tiles[ix + i][iy + j];
											SetTilesetTile(tile, set_tile_tileset, set_tile_start_x + i, set_tile_start_y + j);
											g_Platforms[iEditPlatform].types[ix + i][iy + j] = g_tilesetmanager->GetTileset(tile->iID)->GetTileType(tile->iCol, tile->iRow);
										}
									}
								}
							}
                    } else if (PLATFORM_EDIT_STATE_ANIMATED == iPlatformEditState) { //animated tiles
                        if (!ignoreclick) {
								short ix = event.button.x / TILESIZE;
								short iy = event.button.y / TILESIZE;

                            for (short i = 0; i < set_tile_cols; i++) {
                                for (short j = 0; j < set_tile_rows; j++) {
                                    if (ix + i >= 0 && ix + i < MAPWIDTH && iy + j >= 0 && iy + j < MAPHEIGHT) {
											TilesetTile * tile = &g_Platforms[iEditPlatform].tiles[ix + i][iy + j];
											SetTilesetTile(tile, TILESETANIMATED, set_tile_start_y + j, set_tile_start_x + i);
											g_Platforms[iEditPlatform].types[ix + i][iy + j] = animatedtiletypes[tile->iRow + (tile->iCol << 5)];
										}
									}
								}
							}
                    } else if (PLATFORM_EDIT_STATE_TILETYPE == iPlatformEditState) {
                        if (!ignoreclick) {
								short ix = event.button.x / TILESIZE;
								short iy = event.button.y / TILESIZE;

								g_Platforms[iEditPlatform].types[ix][iy] = set_tiletype;
							}
                    } else if (PLATFORM_EDIT_STATE_PATH == iPlatformEditState) {
                    #if defined(USE_SDL2) || defined(__EMSCRIPTEN__)
                        const Uint8 * keystate = SDL_GetKeyboardState(NULL);
                    #else
                        Uint8 * keystate = SDL_GetKeyState(NULL);
                    #endif
                        if (g_Platforms[iEditPlatform].iPathType == 2 && (keystate[SDLK_z] || keystate[SDLK_x] || keystate[SDLK_c])) {
								UpdatePlatformPathRadius(iEditPlatform, event.button.x, event.button.y, keystate[SDLK_LSHIFT] || keystate[SDLK_RSHIFT], keystate[SDLK_z] != 0, keystate[SDLK_c] != 0);
                        } else {
								UpdatePlatformPathStart(iEditPlatform, event.button.x, event.button.y, keystate[SDLK_LSHIFT] || keystate[SDLK_RSHIFT]);
							}
						}

						break;
                } else if (event.button.button == SDL_BUTTON_RIGHT) {
						short ix = event.button.x / TILESIZE;
						short iy = event.button.y / TILESIZE;

                    if (PLATFORM_EDIT_STATE_EDIT == iPlatformEditState || PLATFORM_EDIT_STATE_ANIMATED == iPlatformEditState) {
							ClearTilesetTile(&g_Platforms[iEditPlatform].tiles[ix][iy]);
							g_Platforms[iEditPlatform].types[ix][iy] = tile_nonsolid;
                    } else if (PLATFORM_EDIT_STATE_TILETYPE == iPlatformEditState) {
							g_Platforms[iEditPlatform].types[ix][iy] = tile_nonsolid;
                    } else if (PLATFORM_EDIT_STATE_PATH == iPlatformEditState) {
                    #if defined(USE_SDL2) || defined(__EMSCRIPTEN__)
                        const Uint8 * keystate = SDL_GetKeyboardState(NULL);
                    #else
                        Uint8 * keystate = SDL_GetKeyState(NULL);
                    #endif
                        if (g_Platforms[iEditPlatform].iPathType == 0) {
								UpdatePlatformPathEnd(iEditPlatform, event.button.x, event.button.y, keystate[SDLK_LSHIFT] || keystate[SDLK_RSHIFT]);
                        } else if (g_Platforms[iEditPlatform].iPathType == 1 || g_Platforms[iEditPlatform].iPathType == 2) {
								UpdatePlatformPathAngle(iEditPlatform, event.button.x, event.button.y, keystate[SDLK_LSHIFT] || keystate[SDLK_RSHIFT]);
							}
						}
					}
				}
            case SDL_MOUSEMOTION: {
					short ix = event.button.x / TILESIZE;
					short iy = event.button.y / TILESIZE;

                if (PLATFORM_EDIT_STATE_EDIT == iPlatformEditState || PLATFORM_EDIT_STATE_ANIMATED == iPlatformEditState) {
                    if (event.motion.state == SDL_BUTTON(SDL_BUTTON_LEFT) && !ignoreclick) {
                        for (short i = 0; i < set_tile_cols; i++) {
                            for (short j = 0; j < set_tile_rows; j++) {
                                if (ix + i >= 0 && ix + i < MAPWIDTH && iy + j >= 0 && iy + j < MAPHEIGHT) {
										TilesetTile * tile = &g_Platforms[iEditPlatform].tiles[ix + i][iy + j];

                                    if (PLATFORM_EDIT_STATE_EDIT == iPlatformEditState) {
											SetTilesetTile(tile, set_tile_tileset, set_tile_start_x + i, set_tile_start_y + j);
											g_Platforms[iEditPlatform].types[ix + i][iy + j] = g_tilesetmanager->GetTileset(tile->iID)->GetTileType(tile->iCol, tile->iRow);
                                    } else {
											SetTilesetTile(tile, TILESETANIMATED, set_tile_start_y + j, set_tile_start_x + i);
											g_Platforms[iEditPlatform].types[ix + i][iy + j] = animatedtiletypes[tile->iRow + (tile->iCol << 5)];
										}
									}
								}
							}
                    } else if (event.motion.state == SDL_BUTTON(SDL_BUTTON_RIGHT)) {
							ClearTilesetTile(&g_Platforms[iEditPlatform].tiles[ix][iy]);
							g_Platforms[iEditPlatform].types[ix][iy] = tile_nonsolid;
						}
                } else if (PLATFORM_EDIT_STATE_TILETYPE == iPlatformEditState) {
						if (event.motion.state == SDL_BUTTON(SDL_BUTTON_LEFT) && !ignoreclick)
							g_Platforms[iEditPlatform].types[ix][iy] = set_tiletype;
						else if (event.motion.state == SDL_BUTTON(SDL_BUTTON_RIGHT))
							g_Platforms[iEditPlatform].types[ix][iy] = tile_nonsolid;
                } else if (PLATFORM_EDIT_STATE_PATH == iPlatformEditState) {
                    if (event.motion.state == SDL_BUTTON(SDL_BUTTON_LEFT)) {
                    #if defined(USE_SDL2) || defined(__EMSCRIPTEN__)
                        const Uint8 * keystate = SDL_GetKeyboardState(NULL);
                    #else
                        Uint8 * keystate = SDL_GetKeyState(NULL);
                    #endif
                        if (g_Platforms[iEditPlatform].iPathType == 2 && (keystate[SDLK_z] || keystate[SDLK_x] || keystate[SDLK_c])) {
								UpdatePlatformPathRadius(iEditPlatform, event.button.x, event.button.y, keystate[SDLK_LSHIFT] || keystate[SDLK_RSHIFT], keystate[SDLK_z] != 0, keystate[SDLK_c] != 0);
                        } else {
								UpdatePlatformPathStart(iEditPlatform, event.button.x, event.button.y, keystate[SDLK_LSHIFT] || keystate[SDLK_RSHIFT]);
							}
                    } else if (event.motion.state == SDL_BUTTON(SDL_BUTTON_RIGHT)) {
                    #if defined(USE_SDL2) || defined(__EMSCRIPTEN__)
                        const Uint8 * keystate = SDL_GetKeyboardState(NULL);
                    #else
                        Uint8 * keystate = SDL_GetKeyState(NULL);
                    #endif
                        if (g_Platforms[iEditPlatform].iPathType == 0) {
								UpdatePlatformPathEnd(iEditPlatform, event.button.x, event.button.y, keystate[SDLK_LSHIFT] || keystate[SDLK_RSHIFT]);
                        } else if (g_Platforms[iEditPlatform].iPathType == 1 || g_Platforms[iEditPlatform].iPathType == 2) {
								UpdatePlatformPathAngle(iEditPlatform, event.button.x, event.button.y, keystate[SDLK_LSHIFT] || keystate[SDLK_RSHIFT]);
							}
						}
					}
					//Display platform preview
                else if (PLATFORM_EDIT_STATE_SELECT == iPlatformEditState || PLATFORM_EDIT_STATE_MOVE == iPlatformEditState) {
						iPlatformPreview = -1;
                    for (int iPlatform = 0; iPlatform < g_iNumPlatforms; iPlatform++) {
							if (event.button.x >= g_Platforms[iPlatform].rIcon[1].x && event.button.x < g_Platforms[iPlatform].rIcon[1].x + g_Platforms[iPlatform].rIcon[1].w &&
                                event.button.y >= g_Platforms[iPlatform].rIcon[1].y && event.button.y < g_Platforms[iPlatform].rIcon[1].y + g_Platforms[iPlatform].rIcon[1].h) {
								iPlatformPreview = iPlatform;
							}
						}
					}


					break;
				}
            case SDL_MOUSEBUTTONUP: {
                if (event.button.button == SDL_BUTTON_LEFT) {
						ignoreclick = false;
					}

					break;
				}

				default:
					break;
			}
		}

        if (PLATFORM_EDIT_STATE_TEST != iPlatformEditState) {
			drawmap(false, TILESIZE);
			rm->menu_shade.draw(0, 0);
		}

        if (PLATFORM_EDIT_STATE_SELECT == iPlatformEditState || PLATFORM_EDIT_STATE_MOVE == iPlatformEditState) {
			SDL_Rect rp;
			rp.x = 0;
			rp.y = 0;
			rp.w = 256;
			rp.h = 224;

			SDL_BlitSurface(s_platform, &rp, screen, &r);
			rm->menu_font_small.drawCentered(320, 146, "Platforms");

			rm->menu_font_small.draw(0, 480 - rm->menu_font_small.getHeight(), "Platform Mode: [esc] Exit  [c] Check Paths, [1-8] Select, [n] New");
			rm->menu_font_small.drawRightJustified(640, 0, maplist->currentFilename());

			for (int iPlatform = 0; iPlatform < g_iNumPlatforms; iPlatform++)
				SDL_BlitSurface(s_platform, &g_Platforms[iPlatform].rIcon[0], screen, &g_Platforms[iPlatform].rIcon[1]);

			if (g_iNumPlatforms < MAX_PLATFORMS && PLATFORM_EDIT_STATE_SELECT == iPlatformEditState)
				SDL_BlitSurface(s_platform, &rNewButton[0], screen, &rNewButton[1]);

            if (PLATFORM_EDIT_STATE_MOVE == iPlatformEditState) {
                if (iPlatformSwitchState == 0) {
					rm->menu_font_small.drawCentered(320, 280, "Select First");
					rm->menu_font_small.drawCentered(320, 300, "Platform To Switch");
                } else {
					rm->menu_font_small.drawCentered(320, 280, "Select Second");
					rm->menu_font_small.drawCentered(320, 300, "Platform To Switch");
				}
			}

            if (iPlatformPreview >= 0) {
				DisplayPlatformPreview(iPlatformPreview, event.button.x, event.button.y);
			}
        } else if (PLATFORM_EDIT_STATE_PATH_TYPE == iPlatformEditState || PLATFORM_EDIT_STATE_CHANGE_PATH_TYPE == iPlatformEditState) {
			//Draw path options
            for (short iType = 0; iType < 3; iType++) {
				SDL_BlitSurface(s_platformpathbuttons, &rTypeButton[iType][0], screen, &rTypeButton[iType][1]);
				SDL_BlitSurface(s_platformpathbuttons, &rTypeButton[iType][2], screen, &rTypeButton[iType][3]);

				rm->menu_font_large.draw(rTypeButton[iType][1].x + 36, rTypeButton[iType][1].y + 6, szPathNames[iType]);
			}

			rm->menu_font_small.draw(0, 480 - rm->menu_font_small.getHeight(), "Path Type");
        } else if (PLATFORM_EDIT_STATE_EDIT == iPlatformEditState || PLATFORM_EDIT_STATE_ANIMATED == iPlatformEditState || PLATFORM_EDIT_STATE_TILETYPE == iPlatformEditState) {
			rm->menu_font_small.draw(0, 480 - rm->menu_font_small.getHeight() * 3, "Edit Platform");
			rm->menu_font_small.draw(0, 480 - rm->menu_font_small.getHeight() * 2, "[esc] Exit  [t] Tiles  [a] Animation [l] Types [del] Delete  [p] Path");
			rm->menu_font_small.draw(0, 480 - rm->menu_font_small.getHeight(), "[+/-] Velocity  [y] Draw Layer");
			draw_platform(iEditPlatform, PLATFORM_EDIT_STATE_TILETYPE == iPlatformEditState);

            if (g_Platforms[iEditPlatform].iPathType == 2) {
				short iVelMarkerX = 198 + (g_Platforms[iEditPlatform].iVelocity + 10) * 12;

				SDL_Rect rVel[2] = {{0, 400, 244, 17},{198, 10, 244, 17}};
				SDL_BlitSurface(s_platform, &rVel[0], screen, &rVel[1]);

				SDL_Rect rMarker[2] = {{244,400,8,18},{iVelMarkerX,10,8,18}};
				SDL_BlitSurface(s_platform, &rMarker[0], screen, &rMarker[1]);

				rm->menu_font_small.drawRightJustified(198, 10, "Counter");
				rm->menu_font_small.draw(442, 10, "Clockwise");
            } else {
				short iVelMarkerX = 220 + (g_Platforms[iEditPlatform].iVelocity - 1) * 12;

				SDL_Rect rVel[2] = {{12, 384, 172, 13},{234, 10, 172, 13}};
				SDL_BlitSurface(s_platform, &rVel[0], screen, &rVel[1]);

				SDL_Rect rMarker[2] = {{184, 384, 8, 16},{iVelMarkerX, 8, 8, 16}};
				SDL_BlitSurface(s_platform, &rMarker[0], screen, &rMarker[1]);

				rm->menu_font_small.drawRightJustified(234, 10, "Slow");
				rm->menu_font_small.draw(406, 10, "Fast");
			}

			rm->spr_number_icons.draw(619, 5, g_Platforms[iEditPlatform].iDrawLayer << 4, 48, 16, 16);

        } else if (PLATFORM_EDIT_STATE_PATH == iPlatformEditState) {
            if (g_Platforms[iEditPlatform].iPathType == 0) { //line segment path
				MapPlatform * platform = &g_Platforms[iEditPlatform];
				DrawPlatform(platform->iPathType, g_map->platforms[iEditPlatform]->iTileData, platform->iStartX, platform->iStartY, platform->iEndX, platform->iEndY, platform->fAngle, platform->fRadiusX, platform->fRadiusY, 0, iPlatformWidth, iPlatformHeight, false, true);

				rm->menu_font_small.draw(0, 480 - (rm->menu_font_small.getHeight() << 1), "Edit Path");
				rm->menu_font_small.draw(0, 480 - rm->menu_font_small.getHeight(), "[esc] Exit  [LMB] Set Start Point  [RMB] Set End Point [t] Path Type");
            } else if (g_Platforms[iEditPlatform].iPathType == 1) { //continuous path
				MapPlatform * platform = &g_Platforms[iEditPlatform];
				DrawPlatform(platform->iPathType, g_map->platforms[iEditPlatform]->iTileData, platform->iStartX, platform->iStartY, platform->iEndX, platform->iEndY, platform->fAngle, platform->fRadiusX, platform->fRadiusY, 0, iPlatformWidth, iPlatformHeight, false, true);

				rm->menu_font_small.draw(0, 480 - (rm->menu_font_small.getHeight() << 1), "Edit Path: [esc] Exit  [LMB] Set Start Point  [RMB] Set Angle");
				rm->menu_font_small.draw(0, 480 - rm->menu_font_small.getHeight(), "[SHIFT + LMB] Location Snap [SHIFT + RMB] Angle Snap [t] Path Type");
            } else if (g_Platforms[iEditPlatform].iPathType == 2) { //ellipse path
				MapPlatform * platform = &g_Platforms[iEditPlatform];
				DrawPlatform(platform->iPathType, g_map->platforms[iEditPlatform]->iTileData, platform->iStartX, platform->iStartY, platform->iEndX, platform->iEndY, platform->fAngle, platform->fRadiusX, platform->fRadiusY, 0, iPlatformWidth, iPlatformHeight, false, true);

				rm->menu_font_small.draw(0, 480 - rm->menu_font_small.getHeight() * 4, "Edit Path: [esc] Exit  [LMB] Set Center [SHIFT + LMB] Center Snap");
				rm->menu_font_small.draw(0, 480 - rm->menu_font_small.getHeight() * 3, "[X + LMB] Set X Radius [SHIFT + X + LMB] X Radius Snap");
				rm->menu_font_small.draw(0, 480 - rm->menu_font_small.getHeight() * 2, "[Z + LMB] Set Y Radius [SHIFT + Z + LMB] Y Radius Snap");
				rm->menu_font_small.draw(0, 480 - rm->menu_font_small.getHeight(), "[C + LMB] Set Circular Radius [SHIFT + C + LMB] Circular Radius Snap [t] Path Type");
			}
        } else if (PLATFORM_EDIT_STATE_TEST == iPlatformEditState) {
			g_map->updatePlatforms();

			//Platforms are drawn inside drawmap(...)
			drawmap(false, TILESIZE, true);

			rm->menu_font_small.draw(0, 480 - rm->menu_font_small.getHeight(), "Check Paths: [esc] Exit");
		}

		DrawMessage();
		return EDITOR_PLATFORM;
}

void DisplayPlatformPreview(short iPlatformId, short iMouseX, short iMouseY)
{
	SDL_Rect srcRect = {0, 0, 160, 120};
	SDL_Rect dstRect = {iMouseX, iMouseY, 160, 120};
	SDL_BlitSurface(g_Platforms[iPlatformId].preview, &srcRect, screen, &dstRect);
}

void SwitchPlatforms(short iPlatformId1, short iPlatformId2)
{
	MapPlatform tempPlatform;
	CopyPlatform(&tempPlatform, &g_Platforms[iPlatformId1]);
	CopyPlatform(&g_Platforms[iPlatformId1], &g_Platforms[iPlatformId2]);
	CopyPlatform(&g_Platforms[iPlatformId2], &tempPlatform);

	g_Platforms[iPlatformId1].UpdatePreview();
	g_Platforms[iPlatformId2].UpdatePreview();
}

void CopyPlatform(MapPlatform * toPlatform, MapPlatform * fromPlatform)
{
	toPlatform->fAngle = fromPlatform->fAngle;
	toPlatform->fRadiusX = fromPlatform->fRadiusX;
	toPlatform->fRadiusY = fromPlatform->fRadiusY;
	toPlatform->iDrawLayer = fromPlatform->iDrawLayer;
	toPlatform->iEndX = fromPlatform->iEndX;
	toPlatform->iEndY = fromPlatform->iEndY;
	toPlatform->iPathType = fromPlatform->iPathType;
	toPlatform->iStartX = fromPlatform->iStartX;
    toPlatform->iStartY = fromPlatform->iStartY;
    toPlatform->iVelocity = fromPlatform->iVelocity;

    for (short iRow = 0; iRow < MAPHEIGHT; iRow++) {
        for (short iCol = 0; iCol < MAPWIDTH; iCol++) {
			CopyTilesetTile(&toPlatform->tiles[iCol][iRow], &fromPlatform->tiles[iCol][iRow]);
			toPlatform->types[iCol][iRow] = fromPlatform->types[iCol][iRow];
		}
	}
}

void UpdatePlatformPathStart(short iEditPlatform, short iClickX, short iClickY, bool fSnapToTile)
{
    if (!ignoreclick) {
        if (fSnapToTile) {
			//Round off coord to nearest tile
			iClickX &= ~15;
			iClickY &= ~15;
		}

		g_Platforms[iEditPlatform].iStartX = iClickX;
		g_Platforms[iEditPlatform].iStartY = iClickY;
	}
}

void UpdatePlatformPathEnd(short iEditPlatform, short iClickX, short iClickY, bool fSnapToTile)
{
    if (!ignoreclick) {
        if (fSnapToTile) {
			//Round off coord to nearest tile
			iClickX &= ~15;
			iClickY &= ~15;
		}

		g_Platforms[iEditPlatform].iEndX = iClickX;
		g_Platforms[iEditPlatform].iEndY = iClickY;
	}
}

void UpdatePlatformPathAngle(short iEditPlatform, short iClickX, short iClickY, bool fSnapToAngle)
{
    if (!ignoreclick) {
		int iDiffX = iClickX - g_Platforms[iEditPlatform].iStartX;
		int iDiffY = iClickY - g_Platforms[iEditPlatform].iStartY;

		float fAngle = atan2((float)iDiffY, (float)iDiffX);

		if (fAngle < 0.0f)
			fAngle += TWO_PI;

        if (fSnapToAngle) {
			float dSector = TWO_PI / 16;
			fAngle += TWO_PI / 32;

			if (fAngle > TWO_PI)
				fAngle -= TWO_PI;

            for (short iSector = 0; iSector < 16; iSector++) {
                if (fAngle >= dSector * iSector && fAngle < dSector * (iSector + 1)) {
					fAngle = dSector * iSector;
					break;
				}
			}
		}

		g_Platforms[iEditPlatform].fAngle = fAngle;
	}
}

void UpdatePlatformPathRadius(short iEditPlatform, short iClickX, short iClickY, bool fSnapToRadius, bool fIsRadiusX, bool fSetCircle)
{
    if (!ignoreclick) {
        if (fSetCircle) {
			short iDiffX = iClickX - g_Platforms[iEditPlatform].iStartX;
			short iDiffY = iClickY - g_Platforms[iEditPlatform].iStartY;

			short iRadius = (short)sqrt((double)(iDiffX * iDiffX + iDiffY * iDiffY));

			if (fSnapToRadius)
				iRadius &= ~15;

			g_Platforms[iEditPlatform].fRadiusX = (float)iRadius;
			g_Platforms[iEditPlatform].fRadiusY = g_Platforms[iEditPlatform].fRadiusX;
        } else {
            if (fIsRadiusX) {
				short iRadiusX = abs(iClickX - g_Platforms[iEditPlatform].iStartX);

				if (fSnapToRadius)
					iRadiusX &= ~15;

				g_Platforms[iEditPlatform].fRadiusX = (float)iRadiusX;
            } else {
				short iRadiusY = abs(iClickY - g_Platforms[iEditPlatform].iStartY);

				if (fSnapToRadius)
					iRadiusY &= ~15;

				g_Platforms[iEditPlatform].fRadiusY = (float)iRadiusY;
			}
		}
	}
}

//Draws the unfinished platform (complete map width x height grid)
void draw_platform(short iPlatform, bool fDrawTileTypes)
{
    for (short iCol = 0; iCol < MAPWIDTH; iCol++) {
        for (short iRow = 0; iRow < MAPHEIGHT; iRow++) {
			TilesetTile * tile = &g_Platforms[iPlatform].tiles[iCol][iRow];

            if (tile->iID >= 0) {
				g_tilesetmanager->Draw(screen, tile->iID, 0, tile->iCol, tile->iRow, iCol, iRow);
				//SDL_BlitSurface(g_tilesetmanager->GetTileset(tile->iID)->GetSurface(0), &g_tilesetmanager->rRects[0][tile->iCol][tile->iRow], screen, &bltrect);
            } else if (tile->iID == TILESETANIMATED) {
				short iSrcCol = tile->iCol << 2;
				short iSrcRow = tile->iRow;

                if (iSrcCol > 31 || iSrcCol < 0 || iSrcRow > 31 || iSrcRow < 0) {
					iSrcCol = 0;
					iSrcRow = 0;
				}

				SDL_BlitSurface(rm->spr_tileanimation[0].getSurface(), &g_tilesetmanager->rRects[0][iSrcCol][iSrcRow], screen, &g_tilesetmanager->rRects[0][iCol][iRow]);
            } else if (tile->iID == TILESETUNKNOWN) {
				SDL_BlitSurface(rm->spr_unknowntile[0].getSurface(), &g_tilesetmanager->rRects[0][0][0], screen, &g_tilesetmanager->rRects[0][iCol][iRow]);
			}

            if (fDrawTileTypes) {
				TileType type = g_Platforms[iPlatform].types[iCol][iRow];
				rm->spr_transparenttiles.draw(iCol * TILESIZE, iRow * TILESIZE, (type - 1) * TILESIZE, 0, TILESIZE, TILESIZE);
			}
		}
	}
}

enum {MAPHAZARD_EDIT_STATE_SELECT, MAPHAZARD_EDIT_STATE_TYPE, MAPHAZARD_EDIT_STATE_LOCATION, MAPHAZARD_EDIT_STATE_PROPERTIES};

short iEditState = MAPHAZARD_EDIT_STATE_SELECT;
short iEditMapHazard = 0;

SDL_Rect rBackground[2];
//SDL_Rect rNewButton[2];
//SDL_Rect rTypeButton[8][4];
SDL_Rect rIconRects[MAXMAPHAZARDS][2];

const char * szHazardNames[8] = {"Fireballs", "Rotodisc", "Bullet Bill", "Flame Thrower", "Green Pirhana", "Red Pirhana", "Tall Pirhana", "Short Pirhana"};

bool editor_maphazards_initialized = false;
void init_editor_maphazards()
{
    if (editor_maphazards_initialized)
        return;

    rBackground[0].x = 0;
    rBackground[0].y = 0;
    rBackground[0].w = 256;
    rBackground[0].h = 224;

    rBackground[1].x = 320 - (rBackground[0].w >> 1);
    rBackground[1].y = 240 - (rBackground[0].h >> 1);
    rBackground[1].w = 256;
    rBackground[1].h = 224;

    rNewButton[0].x = 0;
    rNewButton[0].y = 352;
    rNewButton[0].w = 76;
    rNewButton[0].h = 32;

    rNewButton[1].x = rBackground[1].x + (rBackground[1].w >> 1) - (rNewButton[0].w >> 1);
    rNewButton[1].y = rBackground[1].y + rBackground[1].h + 8;
    rNewButton[1].w = 76;
    rNewButton[1].h = 32;

    for (short iType = 0; iType < 8; iType++) {
        rTypeButton[iType][0].x = 0;
        rTypeButton[iType][0].y = 0;
        rTypeButton[iType][0].w = 192;
        rTypeButton[iType][0].h = 32;

        rTypeButton[iType][1].x = 320 - (rTypeButton[iType][0].w >> 1);
        rTypeButton[iType][1].y = 84 + iType * 40;
        rTypeButton[iType][1].w = 192;
        rTypeButton[iType][1].h = 32;

        rTypeButton[iType][2].x = 24 * iType;
        rTypeButton[iType][2].y = 32;
        rTypeButton[iType][2].w = 24;
        rTypeButton[iType][2].h = 24;

        rTypeButton[iType][3].x = rTypeButton[iType][1].x + 8;
        rTypeButton[iType][3].y = rTypeButton[iType][1].y + 4;
        rTypeButton[iType][3].w = 24;
        rTypeButton[iType][3].h = 24;
    }

    //Setup Map Hazard Icons
    for (short iMapHazard = 0; iMapHazard < MAXMAPHAZARDS; iMapHazard++) {
        rIconRects[iMapHazard][0].x = (iMapHazard % 8) * 32;
        rIconRects[iMapHazard][0].y = (iMapHazard / 8) * 32 + 224;
        rIconRects[iMapHazard][0].w = 32;
        rIconRects[iMapHazard][0].h = 32;

        rIconRects[iMapHazard][1].x = (iMapHazard % 6) * 40 + 204;
        rIconRects[iMapHazard][1].y = (iMapHazard / 6) * 40 + rBackground[1].y + 16;
        rIconRects[iMapHazard][1].w = 32;
        rIconRects[iMapHazard][1].h = 32;
    }

    editor_maphazards_initialized = true;
}

int editor_maphazards()
{
	init_editor_maphazards();

        //handle messages
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
				return EDITOR_QUIT;

            case SDL_KEYDOWN: {
                if (event.key.keysym.sym == SDLK_s) {
						savecurrentmap();
                } else if (event.key.keysym.sym == SDLK_l) {
                    if (MAPHAZARD_EDIT_STATE_SELECT != iEditState) {
							iEditState = MAPHAZARD_EDIT_STATE_LOCATION;
						}
                } else if (event.key.keysym.sym == SDLK_p) {
                    if (MAPHAZARD_EDIT_STATE_SELECT != iEditState) {
							iEditState = MAPHAZARD_EDIT_STATE_PROPERTIES;
						}
                } else if (event.key.keysym.sym >= SDLK_1 && event.key.keysym.sym <= SDLK_9) {
                    if (MAPHAZARD_EDIT_STATE_SELECT == iEditState) {
							short iHazard = event.key.keysym.sym - SDLK_1;
                        if (iHazard < g_map->iNumMapHazards) {
								iEditMapHazard = iHazard;
								iEditState = MAPHAZARD_EDIT_STATE_PROPERTIES;
							}
                    } else if (MAPHAZARD_EDIT_STATE_PROPERTIES == iEditState) {
							MapHazard * hazard = &g_map->maphazards[iEditMapHazard];

							if (hazard->itype == 0 || hazard->itype == 1)
								hazard->iparam[0] = event.key.keysym.sym - SDLK_1 + 1;
						}
                } else if (event.key.keysym.sym == SDLK_DELETE) {
                    if (MAPHAZARD_EDIT_STATE_PROPERTIES == iEditState || MAPHAZARD_EDIT_STATE_LOCATION == iEditState) {
							//Copy platforms into empty spot
                        for (short iMapHazard = iEditMapHazard; iMapHazard < g_map->iNumMapHazards - 1; iMapHazard++) {
								g_map->maphazards[iMapHazard].itype = g_map->maphazards[iMapHazard + 1].itype;
								g_map->maphazards[iMapHazard].ix = g_map->maphazards[iMapHazard + 1].ix;
								g_map->maphazards[iMapHazard].iy = g_map->maphazards[iMapHazard + 1].iy;

                            for (short iParam = 0; iParam < NUMMAPHAZARDPARAMS; iParam++) {
									g_map->maphazards[iMapHazard].iparam[iParam] = g_map->maphazards[iMapHazard + 1].iparam[iParam];
									g_map->maphazards[iMapHazard].dparam[iParam] = g_map->maphazards[iMapHazard + 1].dparam[iParam];
								}
							}

							g_map->iNumMapHazards--;
							iEditState = MAPHAZARD_EDIT_STATE_SELECT;
						}
                } else if (event.key.keysym.sym == SDLK_ESCAPE) {
                    if (MAPHAZARD_EDIT_STATE_SELECT == iEditState) {
                            editor_maphazards_initialized = false;
							return EDITOR_EDIT;
                    } else if (MAPHAZARD_EDIT_STATE_LOCATION == iEditState || MAPHAZARD_EDIT_STATE_PROPERTIES == iEditState) {
							iEditState = MAPHAZARD_EDIT_STATE_SELECT;
						}
                } else if (event.key.keysym.sym == SDLK_KP_MINUS || event.key.keysym.sym == SDLK_MINUS) {
                    if (MAPHAZARD_EDIT_STATE_PROPERTIES == iEditState) {
							MapHazard * hazard = &g_map->maphazards[iEditMapHazard];

                        if (hazard->itype == 0 || hazard->itype == 1) {
								if (g_map->maphazards[iEditMapHazard].dparam[0] > -0.05f)
									g_map->maphazards[iEditMapHazard].dparam[0] -= 0.005f;
                        } else if (hazard->itype == 2) {
								if (g_map->maphazards[iEditMapHazard].dparam[0] > -10.0f)
									g_map->maphazards[iEditMapHazard].dparam[0] -= 1.0f;

								if (g_map->maphazards[iEditMapHazard].dparam[0] == 0.0f)
									g_map->maphazards[iEditMapHazard].dparam[0] -= 1.0f;
							}
						}
                } else if (event.key.keysym.sym == SDLK_KP_PLUS || event.key.keysym.sym == SDLK_EQUALS) {
                    if (MAPHAZARD_EDIT_STATE_PROPERTIES == iEditState) {
							MapHazard * hazard = &g_map->maphazards[iEditMapHazard];

                        if (hazard->itype == 0 || hazard->itype == 1) {
								if (g_map->maphazards[iEditMapHazard].dparam[0] < 0.05f)
									g_map->maphazards[iEditMapHazard].dparam[0] += 0.005f;
                        } else if (hazard->itype == 2) {
								if (g_map->maphazards[iEditMapHazard].dparam[0] < 10.0f)
									g_map->maphazards[iEditMapHazard].dparam[0] += 1.0f;

								if (g_map->maphazards[iEditMapHazard].dparam[0] == 0.0f)
									g_map->maphazards[iEditMapHazard].dparam[0] += 1.0f;
							}
						}
                } else if (event.key.keysym.sym == SDLK_COMMA || event.key.keysym.sym == SDLK_LEFTBRACKET) {
                    if (MAPHAZARD_EDIT_STATE_PROPERTIES == iEditState) {
							MapHazard * hazard = &g_map->maphazards[iEditMapHazard];

                        if (hazard->itype >= 2 && hazard->itype <= 7) {
								if (g_map->maphazards[iEditMapHazard].iparam[0] > 30)
									g_map->maphazards[iEditMapHazard].iparam[0] -= 30;
							}
						}
                } else if (event.key.keysym.sym == SDLK_PERIOD || event.key.keysym.sym == SDLK_RIGHTBRACKET) {
                    if (MAPHAZARD_EDIT_STATE_PROPERTIES == iEditState) {
							MapHazard * hazard = &g_map->maphazards[iEditMapHazard];

                        if (hazard->itype >= 2 && hazard->itype <= 7) {
								if (g_map->maphazards[iEditMapHazard].iparam[0] < 480)
									g_map->maphazards[iEditMapHazard].iparam[0] += 30;
							}
						}
                } else if (event.key.keysym.sym == SDLK_d) {
                    if (MAPHAZARD_EDIT_STATE_PROPERTIES == iEditState) {
							MapHazard * hazard = &g_map->maphazards[iEditMapHazard];

                        if (hazard->itype >= 3 && hazard->itype <= 7) {
								if (++hazard->iparam[1] > 3)
									hazard->iparam[1] = 0;
                        } else if (hazard->itype == 2) {
								g_map->maphazards[iEditMapHazard].dparam[0] = -g_map->maphazards[iEditMapHazard].dparam[0];
							}
						}
                } else if (event.key.keysym.sym == SDLK_n) {
                    if (MAPHAZARD_EDIT_STATE_SELECT == iEditState) {
							iEditMapHazard = NewMapHazard();
							iEditState = MAPHAZARD_EDIT_STATE_TYPE;
						}
					}

					break;
				}
            case SDL_MOUSEBUTTONDOWN: {
                if (event.button.button == SDL_BUTTON_LEFT && !ignoreclick) {
						short iClickX = event.button.x;
						short iClickY = event.button.y;

                    if (MAPHAZARD_EDIT_STATE_SELECT == iEditState) {
							//check clicks on existing platforms
                        for (int iMapHazard = 0; iMapHazard < g_map->iNumMapHazards; iMapHazard++) {
								if (iClickX >= rIconRects[iMapHazard][1].x && iClickX < rIconRects[iMapHazard][1].x + rIconRects[iMapHazard][1].w &&
                                    iClickY >= rIconRects[iMapHazard][1].y && iClickY < rIconRects[iMapHazard][1].y + rIconRects[iMapHazard][1].h) {
									iEditMapHazard = iMapHazard;
									iEditState = MAPHAZARD_EDIT_STATE_PROPERTIES;
									ignoreclick = true;

									break;
								}
							}

							//check click on the new button
							if (g_map->iNumMapHazards < MAXMAPHAZARDS && iClickX >= rNewButton[1].x && iClickX < rNewButton[1].x + rNewButton[1].w &&
                                iClickY >= rNewButton[1].y && iClickY < rNewButton[1].y + rNewButton[1].h) {
								iEditMapHazard = NewMapHazard();
								iEditState = MAPHAZARD_EDIT_STATE_TYPE;
								ignoreclick = true;
							}
                    } else if (MAPHAZARD_EDIT_STATE_TYPE == iEditState && !ignoreclick) {
                        for (int iType = 0; iType < 8; iType++) {
								if (iClickX >= rTypeButton[iType][1].x && iClickX < rTypeButton[iType][1].x + rTypeButton[iType][1].w &&
                                    iClickY >= rTypeButton[iType][1].y && iClickY < rTypeButton[iType][1].y + rTypeButton[iType][1].h) {
									MapHazard * hazard = &g_map->maphazards[iEditMapHazard];
									hazard->itype = iType;
									iEditState = MAPHAZARD_EDIT_STATE_LOCATION;
									ignoreclick = true;

									//Set some default values for the selected type
                                if (iType == 0) {
										hazard->iparam[0] = 5; //Number of fireballs in string
										hazard->dparam[0] = 0.02f; //Angular velocity
										hazard->dparam[1] = 0.0f; //Start Angle
                                } else if (iType == 1) {
										hazard->iparam[0] = 2; //Number of fireballs in string
										hazard->dparam[0] = 0.02f; //Angular velocity
										hazard->dparam[1] = 0.0f; //Start Angle
										hazard->dparam[2] = 112.0f; //Radius
                                } else if (iType == 2) {
										hazard->iparam[0] = 120; //Frequency
										hazard->dparam[0] = 2.0f; //Velocity
                                } else if (iType == 3) {
										hazard->iparam[0] = 120; //Frequency
										hazard->iparam[1] = 0; //Direction
                                } else if (iType >= 4 && iType <= 7) {
										hazard->iparam[0] = 300; //Frequency
										hazard->iparam[1] = 0; //Direction
									}

									break;
								}
							}
                    } else if (MAPHAZARD_EDIT_STATE_LOCATION == iEditState && !ignoreclick) {
							MapHazard * hazard = &g_map->maphazards[iEditMapHazard];
							hazard->ix = iClickX / 16;
							hazard->iy = iClickY / 16;
                    } else if (MAPHAZARD_EDIT_STATE_PROPERTIES == iEditState && !ignoreclick) {
							MapHazard * hazard = &g_map->maphazards[iEditMapHazard];
							short iType = hazard->itype;
                        if (iType == 0) { //Edit fireball string
								AdjustMapHazardRadius(hazard, iClickX, iClickY);
                        } else if (iType == 1) { //rotodisc
								AdjustMapHazardRadius(hazard, iClickX, iClickY);
							}
						}

						break;
                } else if (event.button.button == SDL_BUTTON_RIGHT) {
                    if (MAPHAZARD_EDIT_STATE_PROPERTIES == iEditState) {
							short iType = g_map->maphazards[iEditMapHazard].itype;
                        if (iType == 0) { //Edit fireball string

                        } else if (iType == 1) { //rotodisc

							}
						}
					}
				}
            case SDL_MOUSEMOTION: {
					short iClickX = event.button.x;
					short iClickY = event.button.y;

                if (event.motion.state == SDL_BUTTON(SDL_BUTTON_LEFT) && !ignoreclick) {
                    if (MAPHAZARD_EDIT_STATE_LOCATION == iEditState && !ignoreclick) {
							MapHazard * hazard = &g_map->maphazards[iEditMapHazard];
							hazard->ix = iClickX / 16;
							hazard->iy = iClickY / 16;
                    } else if (MAPHAZARD_EDIT_STATE_PROPERTIES == iEditState) {
							MapHazard * hazard = &g_map->maphazards[iEditMapHazard];
							short iType = hazard->itype;
                        if (iType == 0) { //Edit fireball string
								AdjustMapHazardRadius(hazard, iClickX, iClickY);
                        } else if (iType == 1) { //rotodisc
								AdjustMapHazardRadius(hazard, iClickX, iClickY);
							}
						}
					}

					break;
				}
            case SDL_MOUSEBUTTONUP: {
                if (event.button.button == SDL_BUTTON_LEFT) {
						ignoreclick = false;
					}

					break;
				}

				default:
					break;
			}
		}

		//Draw platform editing
		drawmap(false, TILESIZE);

		rm->menu_shade.draw(0, 0);

        if (MAPHAZARD_EDIT_STATE_SELECT == iEditState) {
			SDL_BlitSurface(s_platform, &rBackground[0], screen, &rBackground[1]);

			rm->menu_font_small.draw(0, 480 - rm->menu_font_small.getHeight(), "Map Hazard Mode: [esc] Exit");

			for (int iMapHazard = 0; iMapHazard < g_map->iNumMapHazards; iMapHazard++)
				SDL_BlitSurface(s_platform, &rIconRects[iMapHazard][0], screen, &rIconRects[iMapHazard][1]);

			if (g_map->iNumMapHazards < MAXMAPHAZARDS)
				SDL_BlitSurface(s_platform, &rNewButton[0], screen, &rNewButton[1]);

			rm->menu_font_small.drawCentered(320, rBackground[1].y - 18, "Hazards");
        } else if (MAPHAZARD_EDIT_STATE_TYPE == iEditState) {
			//SDL_BlitSurface(s_platform, &rBackground[0], screen, &rBackground[1]);

			//Draw map hazard options
            for (short iType = 0; iType < 8; iType++) {
				SDL_BlitSurface(s_maphazardbuttons, &rTypeButton[iType][0], screen, &rTypeButton[iType][1]);
				SDL_BlitSurface(s_maphazardbuttons, &rTypeButton[iType][2], screen, &rTypeButton[iType][3]);

				rm->menu_font_large.draw(rTypeButton[iType][1].x + 36, rTypeButton[iType][1].y + 6, szHazardNames[iType]);
			}

			rm->menu_font_small.draw(0, 480 - rm->menu_font_small.getHeight(), "Choose Hazard Type");
        } else if (MAPHAZARD_EDIT_STATE_LOCATION == iEditState) {
			MapHazard * hazard = &g_map->maphazards[iEditMapHazard];
			DrawMapHazard(hazard, 0, true);
			DrawMapHazardControls(hazard);

			rm->menu_font_small.draw(0, 480 - rm->menu_font_small.getHeight(), "Location: [esc] Exit, [p] Properties, [LMB] Set Location");
        } else if (MAPHAZARD_EDIT_STATE_PROPERTIES == iEditState) {
			MapHazard * hazard = &g_map->maphazards[iEditMapHazard];
			DrawMapHazard(hazard, 0, true);
			DrawMapHazardControls(hazard);

            if (hazard->itype == 0 || hazard->itype == 1) {
				rm->menu_font_small.draw(0, 480 - rm->menu_font_small.getHeight() * 3, "Properties");
				rm->menu_font_small.draw(0, 480 - rm->menu_font_small.getHeight() * 2, "[esc] Exit, [l] Location, [+/-] Velocity, [LMB] Angle and Radius");

				if (hazard->itype == 1)
					rm->menu_font_small.draw(0, 480 - rm->menu_font_small.getHeight(), "[Shift + LMB] Snap To Angle, [1-9] Number of Rotodiscs");
				else
					rm->menu_font_small.draw(0, 480 - rm->menu_font_small.getHeight(), "[Shift + LMB] Snap To Angle");
            } else if (hazard->itype == 2) {
				rm->menu_font_small.draw(0, 480 - (rm->menu_font_small.getHeight() << 1), "Properties: [esc] Exit, [l] Location, [d] Direction");
				rm->menu_font_small.draw(0, 480 - rm->menu_font_small.getHeight(), "[-/+] Velocity, [[/]] or [</>] Frequency");
            } else if (hazard->itype >= 3 && hazard->itype <= 7) {
				rm->menu_font_small.draw(0, 480 - (rm->menu_font_small.getHeight() << 1), "Properties: [esc] Exit, [l] Location");
				rm->menu_font_small.draw(0, 480 - rm->menu_font_small.getHeight(), "[[/]] or [</>] Frequency, [d] direction");
            } else {
				rm->menu_font_small.draw(0, 480 - rm->menu_font_small.getHeight(), "Properties: [esc] Exit, [l] Location");
			}
		}

		rm->menu_font_small.drawRightJustified(640, 0, maplist->currentFilename());

		DrawMessage();
		return EDITOR_MAPHAZARDS;
}

short NewMapHazard()
{
	//Clear new map hazard
	short iEditMapHazard = g_map->iNumMapHazards;

	MapHazard * hazard = &g_map->maphazards[iEditMapHazard];
	hazard->itype = 0;
	hazard->ix = 10;
	hazard->iy = 7;

    for (short iMapHazard = 0; iMapHazard < NUMMAPHAZARDPARAMS; iMapHazard++) {
		hazard->iparam[iMapHazard] = 0;
		hazard->dparam[iMapHazard] = 0.0f;
	}

	g_map->iNumMapHazards++;

	return iEditMapHazard;
}

void DrawMapHazardControls(MapHazard * hazard)
{
    if (hazard->itype == 0 || hazard->itype == 1 || hazard->itype == 2) {
		short iVelMarkerX = 0;

		if (hazard->itype == 2)
			iVelMarkerX = (short)(hazard->dparam[0] + 10.0f) * 12 + 196;
		else
			iVelMarkerX = (short)((hazard->dparam[0] + 0.05f) / 0.005f) * 12 + 196;

		SDL_Rect rVel[2] = {{0, 400, 244, 17},{198, 420, 244, 17}};
		SDL_BlitSurface(s_platform, &rVel[0], screen, &rVel[1]);

		SDL_Rect rMarker[2] = {{244,400,8,18},{iVelMarkerX,418,8,18}};
		SDL_BlitSurface(s_platform, &rMarker[0], screen, &rMarker[1]);

        if (hazard->itype == 2) {
			rm->menu_font_small.drawRightJustified(190, 420, "Left");
			rm->menu_font_small.draw(450, 420, "Right");
        } else {
			rm->menu_font_small.drawRightJustified(190, 420, "Counter Clockwise");
			rm->menu_font_small.draw(450, 420, "Clockwise");
		}
	}

    if (hazard->itype >= 2 && hazard->itype <= 7) { // Draw frequency for bullet bill and flame cannon
		short iFreqMarkerX = ((hazard->iparam[0] / 30) - 1) * 12 + 196;

		SDL_Rect rVel[2] = {{0, 384, 184, 13},{198, 390, 184, 13}};
		SDL_BlitSurface(s_platform, &rVel[0], screen, &rVel[1]);

		SDL_Rect rMarker[2] = {{244,400,8,18},{iFreqMarkerX,388,8,18}};
		SDL_BlitSurface(s_platform, &rMarker[0], screen, &rMarker[1]);

		rm->menu_font_small.drawRightJustified(190, 390, "More Frequent");
		rm->menu_font_small.draw(388, 390, "Less Frequent");
	}
}

void AdjustMapHazardRadius(MapHazard * hazard, short iClickX, short iClickY)
{
	short iDiffX = iClickX - ((hazard->ix << 4) + 16);
	short iDiffY = iClickY - ((hazard->iy << 4) + 16);

	float radius = (float)sqrt((double)(iDiffX * iDiffX + iDiffY * iDiffY));
	float angle = (float)atan2((double)iDiffY, (double)iDiffX);

	if (angle < 0.0f)
		angle += TWO_PI;

    #if defined(USE_SDL2) || defined(__EMSCRIPTEN__)
        const Uint8 * keystate = SDL_GetKeyboardState(NULL);
    #else
        Uint8 * keystate = SDL_GetKeyState(NULL);
    #endif
    if (keystate[SDLK_LSHIFT] || keystate[SDLK_RSHIFT]) {
		float dSector = TWO_PI / 16;
		angle += TWO_PI / 32;

		if (angle > TWO_PI)
			angle -= TWO_PI;

        for (short iSector = 0; iSector < 16; iSector++) {
            if (angle >= dSector * iSector && angle < dSector * (iSector + 1)) {
				angle = dSector * iSector;
				break;
			}
		}
	}

    if (hazard->itype == 0) {
        if (radius >= 24.0f) {
			hazard->iparam[0] = (short)radius / 24 + 1;
			hazard->dparam[1] = angle;
        } else {
			hazard->iparam[0] = 1;
			hazard->dparam[1] = 0.0f;
		}
    } else if (hazard->itype == 1) {
		hazard->dparam[1] = angle;

        if (radius > 32.0f) {
            if (keystate[SDLK_LSHIFT] || keystate[SDLK_RSHIFT]) {
				//Snap radius to every 16 pixels
				hazard->dparam[2] = (float)(((int)(radius - 16.0f) >> 4) << 4);
            } else {
				hazard->dparam[2] = radius - 16.0f;
			}
        } else {
			hazard->dparam[2] = 16.0f;
		}
	}
}

CTileset * tileset;
short view_tileset_repeat_direction = -1;
short view_tileset_repeat_timer = 0;

bool editor_tiles_initialized = false;
void init_editor_tiles()
{
    if (editor_tiles_initialized)
        return;

    set_tile_drag = false;
    view_tileset_repeat_direction = -1;
    view_tileset_repeat_timer = 0;
    tileset = g_tilesetmanager->GetTileset(set_tile_tileset);

    editor_tiles_initialized = true;
}

int editor_tiles()
{
    init_editor_tiles();

	/*
	short iCurrentTile = 0;
	short iConvertedTile[300];

	for (short iTile = 0; iTile < 300; iTile++)
		iConvertedTile[iTile] = g_iTileConversion[iTile];
	*/

		//handle messages
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
				case SDL_QUIT:
					return EDITOR_QUIT;

            case SDL_KEYDOWN: {
                if (!set_tile_drag) {
                    if (event.key.keysym.sym >= SDLK_1 && event.key.keysym.sym <= SDLK_9 && event.key.keysym.sym < SDLK_1 + g_tilesetmanager->GetCount()) {
							set_tile_tileset = event.key.keysym.sym - SDLK_1;
							tileset = g_tilesetmanager->GetTileset(set_tile_tileset);
							view_tileset_x = 0;
							view_tileset_y = 0;
                    } else if (event.key.keysym.sym == SDLK_PAGEUP) {
                        if (set_tile_tileset > 0) {
								set_tile_tileset--;
								tileset = g_tilesetmanager->GetTileset(set_tile_tileset);
								view_tileset_x = 0;
								view_tileset_y = 0;
							}
                    } else if (event.key.keysym.sym == SDLK_PAGEDOWN) {
                        if (set_tile_tileset < g_tilesetmanager->GetCount() - 1) {
								set_tile_tileset++;
								tileset = g_tilesetmanager->GetTileset(set_tile_tileset);
								view_tileset_x = 0;
								view_tileset_y = 0;
							}
                    } else if (event.key.keysym.sym == SDLK_UP) {
                        if (view_tileset_y > 0) {
								view_tileset_y--;
								view_tileset_repeat_direction = 0;
								view_tileset_repeat_timer = 30;
							}
                    } else if (event.key.keysym.sym == SDLK_DOWN) {
                        if (view_tileset_y < g_tilesetmanager->GetTileset(set_tile_tileset)->GetHeight() - 15) {
								view_tileset_y++;
								view_tileset_repeat_direction = 1;
								view_tileset_repeat_timer = 30;
							}
                    } else if (event.key.keysym.sym == SDLK_LEFT) {
                        if (view_tileset_x > 0) {
								view_tileset_x--;
								view_tileset_repeat_direction = 2;
								view_tileset_repeat_timer = 30;
							}
                    } else if (event.key.keysym.sym == SDLK_RIGHT) {
                        if (view_tileset_x < g_tilesetmanager->GetTileset(set_tile_tileset)->GetWidth() - 20) {
								view_tileset_x++;
								view_tileset_repeat_direction = 3;
								view_tileset_repeat_timer = 30;
							}
                    } else {
							edit_mode = 1;  //change to edit mode using tiles
							set_tile_drag = false;
                            editor_tiles_initialized = false;
							return EDITOR_EDIT;
						}
					}

					break;
				}

            case SDL_KEYUP: {
					if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_DOWN ||
                        event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_RIGHT) {
						view_tileset_repeat_direction = -1;
						view_tileset_repeat_timer = 0;
					}

					break;
				}

            case SDL_MOUSEBUTTONDOWN: {
					short iCol = event.button.x / TILESIZE + view_tileset_x;
					short iRow = event.button.y / TILESIZE + view_tileset_y;

                if (event.button.button == SDL_BUTTON_LEFT) {
						CTileset * tileset = g_tilesetmanager->GetTileset(set_tile_tileset);

                    if (iCol < tileset->GetWidth() && iRow < tileset->GetHeight()) {
							set_tile_start_x = iCol;
							set_tile_start_y = iRow;
							set_tile_end_x = set_tile_start_x;
							set_tile_end_y = set_tile_start_y;

							set_tile_drag = true;
						}
                } else if (event.button.button == SDL_BUTTON_RIGHT) {
						set_type = tileset->IncrementTileType(iCol, iRow);
					}

					break;
				}

            case SDL_MOUSEBUTTONUP: {
					short iCol = event.button.x / TILESIZE + view_tileset_x;
					short iRow = event.button.y / TILESIZE + view_tileset_y;

                if (event.button.button == SDL_BUTTON_LEFT) {
                    if (iCol < tileset->GetWidth() && iRow < tileset->GetHeight()) {
							set_tile_cols = set_tile_end_x - set_tile_start_x + 1;
							set_tile_rows = set_tile_end_y - set_tile_start_y + 1;

							set_tile_drag = false;
							edit_mode = 1;  //change to edit mode using tiles
                            editor_tiles_initialized = false;
							return EDITOR_EDIT;

						}
					}

					break;
				}

            case SDL_MOUSEMOTION: {
					short iCol = event.button.x / TILESIZE + view_tileset_x;
					short iRow = event.button.y / TILESIZE + view_tileset_y;

                if (iCol < tileset->GetWidth() && iRow < tileset->GetHeight()) {
                    if (event.motion.state == SDL_BUTTON(SDL_BUTTON_LEFT)) {
							if (iCol < set_tile_start_x)
								set_tile_start_x = iCol;

							if (iCol > set_tile_end_x)
								set_tile_end_x = iCol;

							if (iRow < set_tile_start_y)
								set_tile_start_y = iRow;

							if (iRow > set_tile_end_y)
								set_tile_end_y = iRow;
                    } else if (event.motion.state == SDL_BUTTON(SDL_BUTTON_RIGHT)) {
							tileset->SetTileType(iCol, iRow, set_type);
						}
					}

					break;
				}


				default:
					break;
			}
		}

		//Allow auto-scrolling of tilesets when the arrow keys are held down
        if (view_tileset_repeat_direction >= 0 && view_tileset_repeat_timer > 0) {
            if (--view_tileset_repeat_timer <= 0) {
				view_tileset_repeat_timer = 5;

                if (view_tileset_repeat_direction == 0 && view_tileset_y > 0) {
					view_tileset_y--;
                } else if (view_tileset_repeat_direction == 1 && view_tileset_y < g_tilesetmanager->GetTileset(set_tile_tileset)->GetHeight() - 15) {
					view_tileset_y++;
                } else if (view_tileset_repeat_direction == 2 && view_tileset_x > 0) {
					view_tileset_x--;
                } else if (view_tileset_repeat_direction == 3 && view_tileset_x < g_tilesetmanager->GetTileset(set_tile_tileset)->GetWidth() - 20) {
					view_tileset_x++;
				}
			}
		}


		//drawmap(false, TILESIZE);
		//rm->menu_shade.draw(0, 0);
		SDL_FillRect(screen, NULL, 0xFF888888);

		SDL_Rect rectSrc;
		rectSrc.x = view_tileset_x << 5;
		rectSrc.y = view_tileset_y << 5;
		rectSrc.w = tileset->GetWidth() > 20 ? 640 : tileset->GetWidth() << 5;
		rectSrc.h = tileset->GetHeight() > 15 ? 480 : tileset->GetHeight() << 5;

        r.x = 0;
        r.y = 0;
        r.w = 640;
        r.h = 480;

		SDL_BlitSurface(g_tilesetmanager->GetTileset(set_tile_tileset)->GetSurface(0), &rectSrc, screen, &r);
		//rm->menu_font_small.drawRightJustified(640, 0, maplist->currentFilename());
		rm->menu_font_small.draw(0, 480 - rm->menu_font_small.getHeight(), tileset->GetName());

        int i, j;
        for (i = view_tileset_x; i < view_tileset_x + 20 && i < tileset->GetWidth(); i++) {
            for (j = view_tileset_y; j < view_tileset_y + 15 && j < tileset->GetHeight(); j++) {
				TileType t = tileset->GetTileType(i, j);
				if (t != tile_nonsolid)
					rm->spr_tiletypes.draw((i - view_tileset_x) << 5, (j - view_tileset_y) << 5, (t-1) << 3, 0, 8, 8);
			}
		}

        if (set_tile_drag) {
            for (i = set_tile_start_x; i <= set_tile_end_x; i++) {
                for (j = set_tile_start_y; j <= set_tile_end_y; j++) {
					rm->spr_selectedtile.draw((i - view_tileset_x) << 5, (j - view_tileset_y) << 5);
				}
			}
		}
		/*

		//Test code to help convert old tilesets into new tilesets

		rm->menu_font_small.drawRightJustified(640, 0, "%d", iCurrentTile);
		rm->spr_OldTileSet.draw(576, 224, iCurrentTile % 20 * 32, iCurrentTile / 20 * 32, 32, 32);


		short iTileCol = iConvertedTile[iCurrentTile] % 32;
		short iTileRow = iConvertedTile[iCurrentTile] / 32;

		if (selected_tileset == 0 && iTileCol < 16 && iTileRow < 15)
			rm->spr_selectedtile.draw(iTileCol * 32, iTileRow * 32, 0, 0, 32, 32);
		else if (selected_tileset == 1 && iTileCol >= 16 && iTileRow < 15)
			rm->spr_selectedtile.draw((iTileCol - 16) * 32, iTileRow * 32, 0, 0, 32, 32);
		else if (selected_tileset == 2 && iTileCol < 16 && iTileRow >= 15)
			rm->spr_selectedtile.draw(iTileCol * 32, (iTileRow - 15) * 32, 0, 0, 32, 32);
		else if (selected_tileset == 3 && iTileCol >= 16 && iTileRow >= 15)
			rm->spr_selectedtile.draw((iTileCol - 16) * 32, (iTileRow - 15) * 32, 0, 0, 32, 32);


		*/


		DrawMessage();
		return EDITOR_TILES;
}

int editor_blocks()
{
		//handle messages
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
				case SDL_QUIT:
					return EDITOR_QUIT;

				case SDL_KEYDOWN:
					edit_mode = 0;
					return EDITOR_EDIT;
				break;

				case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT) {
						short set_block_x = event.button.x / TILESIZE;
						short set_block_y = event.button.y / TILESIZE;

						//Set the selected block to one of the interaction blocks
						if (set_block_y == 0 && set_block_x >= 0 && set_block_x <= 6)
							set_block = set_block_x;
						else if (set_block_y == 0 && set_block_x >= 7 && set_block_x <= 11)
							set_block = set_block_x + 8;
                    else if (set_block_y >= 1 && set_block_y <= 2 && set_block_x >= 0 && set_block_x <= 3) {
                        //set the selected block to an on/off switch block

							set_block = set_block_x + 7;
							g_map->iSwitches[set_block_x] = 2 - set_block_y;
                    } else if (set_block_y >= 1 && set_block_y <= 2 && set_block_x >= 4 && set_block_x <= 7) {
                        //set the selected block to a switch block

							set_block = set_block_x + 7;
							set_block_switch_on = set_block_y == 1;
                    } else if (set_block_y == 3 && set_block_x >= 0 && set_block_x <= 9) {
                        //set the selected block to a weapon breakable block

							set_block = set_block_x + 20;
						}

						edit_mode = 0;

						//The user must release the mouse button before trying to add a tile
						ignoreclick = true;
						return EDITOR_EDIT;
					}
				break;

				default:
					break;
			}
		}

		drawmap(false, TILESIZE);
		rm->menu_shade.draw(0, 0);

		SDL_Rect rSrc = {0, 0, 224, 32};
		SDL_Rect rDst = {0, 0, 224, 32};

		SDL_BlitSurface(rm->spr_blocks[0].getSurface(), &rSrc, screen, &rDst);

		SDL_Rect rOnOffSrc = {224, 0, 128, 64};
		SDL_Rect rOnOffDst = {0, 32, 128, 64};

		SDL_BlitSurface(rm->spr_blocks[0].getSurface(), &rOnOffSrc, screen, &rOnOffDst);

		SDL_Rect rOnOffBlockSrc = {352, 0, 128, 64};
		SDL_Rect rOnOffBlockDst = {128, 32, 128, 64};

		SDL_BlitSurface(rm->spr_blocks[0].getSurface(), &rOnOffBlockSrc, screen, &rOnOffBlockDst);

		SDL_Rect rBlocksRow2Src = {0, 32, 160, 32};
		SDL_Rect rBlocksRow2Dst = {224, 0, 160, 32};

		SDL_BlitSurface(rm->spr_blocks[0].getSurface(), &rBlocksRow2Src, screen, &rBlocksRow2Dst);

		SDL_Rect rBlocksRow3Src = {0, 64, 320, 32};
		SDL_Rect rBlocksRow3Dst = {0, 96, 320, 32};

		SDL_BlitSurface(rm->spr_blocks[0].getSurface(), &rBlocksRow3Src, screen, &rBlocksRow3Dst);

		rm->menu_font_small.drawRightJustified(640, 0, maplist->currentFilename());

		DrawMessage();
		return EDITOR_BLOCKS;
}


int editor_mapitems()
{
		//handle messages
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
				case SDL_QUIT:
					return EDITOR_QUIT;

				case SDL_KEYDOWN:
					edit_mode = 7;
					return EDITOR_EDIT;
				break;

				case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT) {
						short set_item_x = event.button.x / TILESIZE;
						short set_item_y = event.button.y / TILESIZE;

						//Set the selected block to one of the interaction blocks
                    if (set_item_y == 0 && set_item_x >= 0 && set_item_x <= 5) {
							set_mapitem = set_item_x;

							edit_mode = 7;

							//The user must release the mouse button before trying to add a tile
							ignoreclick = true;
							return EDITOR_EDIT;
						}
					}
				break;

				default:
					break;
			}
		}

		drawmap(false, TILESIZE);
		rm->menu_shade.draw(0, 0);

		rm->spr_mapitems[0].draw(0, 0, 0, 0, 192, 32);

		rm->menu_font_small.drawRightJustified(640, 0, maplist->currentFilename());
		rm->menu_font_small.drawRightJustified(0, 480 - rm->menu_font_small.getHeight(), "Map Items");

		DrawMessage();
		return EDITOR_MAPITEMS;
}

short modeitemmode = 0;
short dragmodeitem = -1;
short dragoffsetx = 0;
short dragoffsety = 0;

bool editor_modeitems_initialized = false;
void init_editor_modeitems()
{
    if (editor_modeitems_initialized)
        return;

    modeitemmode = 0;
    dragmodeitem = -1;
    dragoffsetx = 0;
    dragoffsety = 0;

    editor_modeitems_initialized = true;
}

int editor_modeitems()
{
    init_editor_modeitems();

		//handle messages
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
				return EDITOR_QUIT;

            case SDL_KEYDOWN: {
					dragmodeitem = -1;

                if (event.key.keysym.sym == SDLK_s) {
						savecurrentmap();
                } else if (event.key.keysym.sym == SDLK_ESCAPE || event.key.keysym.sym == SDLK_j) {
                    editor_modeitems_initialized = false;
					return EDITOR_EDIT;
                } else if (event.key.keysym.sym >= SDLK_1 && event.key.keysym.sym <= SDLK_2) {
						modeitemmode = event.key.keysym.sym - SDLK_1;
                } else if (event.key.keysym.sym == SDLK_r) {
						//Set this mode item set to random
                    if (modeitemmode == 0) {
							if (g_map->iNumRaceGoals == 0)
								g_map->iNumRaceGoals = MAXRACEGOALS;
							else
								g_map->iNumRaceGoals = 0;
                    } else if (modeitemmode == 1) {
							if (g_map->iNumFlagBases == 0)
								g_map->iNumFlagBases = 4;
							else
								g_map->iNumFlagBases = 0;
						}
					}
				}

            case SDL_MOUSEBUTTONDOWN: {
                if (event.button.button == SDL_BUTTON_LEFT) {
						short iMouseX = event.button.x;
						short iMouseY = event.button.y;

						dragmodeitem = -1;
                    if (modeitemmode == 0) {
                        for (short iGoal = 0; iGoal < g_map->iNumRaceGoals; iGoal++) {
								if (iMouseX >= g_map->racegoallocations[iGoal].x && iMouseX < g_map->racegoallocations[iGoal].x + 36 &&
                                    iMouseY >= g_map->racegoallocations[iGoal].y && iMouseY < g_map->racegoallocations[iGoal].y + 36) {
									dragmodeitem = iGoal;
									dragoffsetx = iMouseX - g_map->racegoallocations[iGoal].x;
									dragoffsety = iMouseY - g_map->racegoallocations[iGoal].y;
								}
							}
                    } else if (modeitemmode == 1) {
                        for (short iBase = 0; iBase < g_map->iNumFlagBases; iBase++) {
								if (iMouseX >= g_map->flagbaselocations[iBase].x && iMouseX < g_map->flagbaselocations[iBase].x + 32 &&
                                    iMouseY >= g_map->flagbaselocations[iBase].y && iMouseY < g_map->flagbaselocations[iBase].y + 32) {
									dragmodeitem = iBase;
									dragoffsetx = iMouseX - g_map->flagbaselocations[iBase].x;
									dragoffsety = iMouseY - g_map->flagbaselocations[iBase].y;
								}
							}
						}

						//Move mode items around with mouse dragging
					}

					break;
				}

            case SDL_MOUSEBUTTONUP: {
                if (event.button.button == SDL_BUTTON_LEFT) {
						dragmodeitem = -1;
					}

					break;
				}

            case SDL_MOUSEMOTION: {
                if (dragmodeitem >= 0 && event.motion.state == SDL_BUTTON(SDL_BUTTON_LEFT)) {
                        #if defined(USE_SDL2) || defined(__EMSCRIPTEN__)
                            const Uint8 * keystate = SDL_GetKeyboardState(NULL);
                        #else
                            Uint8 * keystate = SDL_GetKeyState(NULL);
                        #endif
						bool fShiftDown = keystate[SDLK_LSHIFT] || keystate[SDLK_RSHIFT];

                    if (modeitemmode == 0) {
							g_map->racegoallocations[dragmodeitem].x = event.motion.x - dragoffsetx;
							g_map->racegoallocations[dragmodeitem].y = event.motion.y - dragoffsety;

                        if (fShiftDown) {
								g_map->racegoallocations[dragmodeitem].x = (event.motion.x >> 5) << 5;
								g_map->racegoallocations[dragmodeitem].y = (event.motion.y >> 5) << 5;
							}
                    } else if (modeitemmode == 1) {
							g_map->flagbaselocations[dragmodeitem].x = event.motion.x - dragoffsetx;
							g_map->flagbaselocations[dragmodeitem].y = event.motion.y - dragoffsety;

                        if (fShiftDown) {
								g_map->flagbaselocations[dragmodeitem].x = (event.motion.x >> 5) << 5;
								g_map->flagbaselocations[dragmodeitem].y = (event.motion.y >> 5) << 5;
							}
						}
					}
				}

				default:
					break;
			}
		}

		drawmap(false, TILESIZE);
		rm->menu_shade.draw(0, 0);

		//rm->spr_mapitems[0].draw(0, 0, 0, 0, 96, 32);

		//draw race goals
        if (modeitemmode == 0) {
            if (g_map->iNumRaceGoals == 0) {
				rm->menu_font_large.drawCentered(320, 200, "Race goals are set to random.");
				rm->menu_font_large.drawCentered(320, 220, "Press 'R' to manually set them.");
            } else {
                for (short iGoal = 0; iGoal < g_map->iNumRaceGoals; iGoal++) {
					rm->spr_racegoals.draw(g_map->racegoallocations[iGoal].x - 16, g_map->racegoallocations[iGoal].y - 18, 0, 0, 68, 54);
					char szNum[4];
					sprintf(szNum, "%d", iGoal + 1);
					rm->menu_font_large.drawCentered(g_map->racegoallocations[iGoal].x + 18, g_map->racegoallocations[iGoal].y + 6, szNum);
				}
			}

			rm->menu_font_small.draw(0, 480 - rm->menu_font_small.getHeight(), "Set Race Goal Locations - Press [2] for Flag Bases");
        } else if (modeitemmode == 1) {
            if (g_map->iNumFlagBases == 0) {
				rm->menu_font_large.drawCentered(320, 200, "Flag bases are set to random.");
				rm->menu_font_large.drawCentered(320, 220, "Press 'R' to manually set them.");
            } else {
                for (short iBase = 0; iBase < g_map->iNumFlagBases; iBase++) {
					rm->spr_flagbases.draw(g_map->flagbaselocations[iBase].x - 8, g_map->flagbaselocations[iBase].y - 8, iBase * 48, 0, 48, 48);
				}
			}

			rm->menu_font_small.draw(0, 480 - rm->menu_font_small.getHeight(), "Set Flag Base Locations - Press [1] for Race Goals");
		}

		rm->menu_font_small.drawRightJustified(640, 0, maplist->currentFilename());

		DrawMessage();
		return EDITOR_MODEITEMS;
}


int editor_tiletype()
{
		//handle messages
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
				case SDL_QUIT:
					return EDITOR_QUIT;

            case SDL_KEYDOWN: {
					edit_mode = 6;
					return EDITOR_EDIT;

					break;
				}

            case SDL_MOUSEBUTTONDOWN: {
                if (event.button.button == SDL_BUTTON_LEFT) {
						short iCol = event.button.x / TILESIZE;
						short iRow = event.button.y / TILESIZE;

                    if (iCol < NUMTILETYPES - 1 && iRow == 0) {
							set_tiletype = (TileType)(iCol + 1);

							edit_mode = 6;
							ignoreclick = true;
							return EDITOR_EDIT;
						}
					}

					break;
				}

				default:
					break;
			}
		}


		drawmap(false, TILESIZE);
		rm->menu_shade.draw(0, 0);

		rm->spr_transparenttiles.draw(0, 0);

		rm->menu_font_small.drawRightJustified(640, 0, maplist->currentFilename());

		DrawMessage();
		return EDITOR_TILETYPE;
}

short iPage;
SDL_Surface * sBackgrounds[16];
SDL_Rect rSrc = {0, 0, 160, 120};
SDL_Rect rDst[16];

bool editor_backgrounds_initialized = false;
void init_editor_backgrounds()
{
    if (editor_backgrounds_initialized)
        return;

    iPage = backgroundlist->GetCurrentIndex() / 16;

    for (short iRectY = 0; iRectY < 4; iRectY++) {
        for (short iRectX = 0; iRectX < 4; iRectX++) {
            rDst[iRectY * 4 + iRectX].x = iRectX * 160;
            rDst[iRectY * 4 + iRectX].y = iRectY * 120;
            rDst[iRectY * 4 + iRectX].w = 160;
            rDst[iRectY * 4 + iRectX].h = 120;
        }
    }

    for (short iSurface = 0; iSurface < 16; iSurface++)
        sBackgrounds[iSurface] = SDL_CreateRGBSurface(screen->flags, 160, 120, 16, 0, 0, 0, 0);

    LoadBackgroundPage(sBackgrounds, iPage);

    editor_backgrounds_initialized = true;
}

int editor_backgrounds()
{
    init_editor_backgrounds();

		//handle messages
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
				case SDL_QUIT:
                    for (short iSurface = 0; iSurface < 16; iSurface++)
                        SDL_FreeSurface(sBackgrounds[iSurface]);

                    editor_backgrounds_initialized = false;
                    return EDITOR_EDIT;
				break;

				case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
					for (short iSurface = 0; iSurface < 16; iSurface++)
                        SDL_FreeSurface(sBackgrounds[iSurface]);

                    editor_backgrounds_initialized = false;
                    return EDITOR_EDIT;
                } else if (event.key.keysym.sym == SDLK_PAGEDOWN || event.key.keysym.sym == SDLK_DOWN) {
                    if ((iPage + 1) * 16 < backgroundlist->GetCount()) {
							iPage++;
							LoadBackgroundPage(sBackgrounds, iPage);
						}
                } else if (event.key.keysym.sym == SDLK_PAGEUP || event.key.keysym.sym == SDLK_UP) {
                    if ((iPage - 1) * 16 >= 0) {
							iPage--;
							LoadBackgroundPage(sBackgrounds, iPage);
						}
					}

				break;

				case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT || event.button.button == SDL_BUTTON_RIGHT) {
                    for (short iBackground = 0; iBackground < 16; iBackground++) {
                        if (iPage * 16 + iBackground >= backgroundlist->GetCount())
								break;

							if (event.button.x >= rDst[iBackground].x && event.button.x < rDst[iBackground].x + rDst[iBackground].w &&
                                event.button.y >= rDst[iBackground].y && event.button.y < rDst[iBackground].y + rDst[iBackground].h)
                            {
                                backgroundlist->SetCurrent(iPage * 16 + iBackground);

                                rm->spr_background.init(convertPath(backgroundlist->current_name()));
                                strcpy(g_map->szBackgroundFile, getFileFromPath(backgroundlist->current_name()).c_str());

                                if (event.button.button == SDL_BUTTON_LEFT) {
									//Set music to background default
                                    for (short iCategory = 0; iCategory < MAXMUSICCATEGORY; iCategory++) {
                                        if (!strncmp(g_szMusicCategoryNames[iCategory], g_map->szBackgroundFile, strlen(g_szMusicCategoryNames[iCategory]))) {
											g_map->musicCategoryID = iCategory;
											break;
										}
									}
								}

                                for (short iSurface = 0; iSurface < 16; iSurface++)
                                    SDL_FreeSurface(sBackgrounds[iSurface]);

                                editor_backgrounds_initialized = false;
                                return EDITOR_EDIT;
							}
						}
					}
				break;

				default:
					break;
			}
		}

		SDL_Rect rect = {0, 0, 640, 480};
		SDL_FillRect(screen, &rect, 0x0);

        for (short iBackground = 0; iBackground < 16; iBackground++) {
            if (iPage * 16 + iBackground >= backgroundlist->GetCount())
				break;

			SDL_BlitSurface(sBackgrounds[iBackground], &rSrc, screen, &rDst[iBackground]);
		}

		rm->menu_font_small.draw(0,480-rm->menu_font_small.getHeight() * 2, "[Page Up] next page, [Page Down] previous page");
		rm->menu_font_small.draw(0,480-rm->menu_font_small.getHeight(), "[LMB] choose background with music category, [RMB] choose just background");

		int x, y;

		SDL_GetMouseState(&x, &y);

		int iID = x / 160 + y / 120 * 4 + iPage * 16;

        if (iID < backgroundlist->GetCount())
            rm->menu_font_small.draw(0, 0, backgroundlist->GetIndex(iID));

		DrawMessage();
		return EDITOR_BACKGROUNDS;
}

//short view_tileset_repeat_direction = -1;
//short view_tileset_repeat_timer = 0;

bool editor_animation_initialized = false;
void init_editor_animation()
{
    if (editor_animation_initialized)
        return;

    set_tile_drag = false;
    view_tileset_repeat_direction = -1;
    view_tileset_repeat_timer = 0;

    editor_animation_initialized = true;
}

int editor_animation()
{
    init_editor_animation();

		//handle messages
        while (SDL_PollEvent(&event)) {
			short iCol = event.button.x / TILESIZE + view_animated_tileset_x;
			short iRow = event.button.y / TILESIZE;

			bool fInValidTile = iRow >= 0 && iRow <= 7;

            switch (event.type) {
            case SDL_QUIT:
                return EDITOR_QUIT;

            case SDL_KEYDOWN: {
                if (event.key.keysym.sym == SDLK_ESCAPE || event.key.keysym.sym == SDLK_a) {
                    if (!set_tile_drag) {
							edit_mode = 8;
                            editor_animation_initialized = false;
							return EDITOR_EDIT;
						}
                } else if (event.key.keysym.sym == SDLK_LEFT) {
                    if (view_animated_tileset_x > 0) {
							view_animated_tileset_x--;
							view_tileset_repeat_direction = 2;
							view_tileset_repeat_timer = 30;
						}
                } else if (event.key.keysym.sym == SDLK_RIGHT) {
                    if (view_animated_tileset_x < 12) {
							view_animated_tileset_x++;
							view_tileset_repeat_direction = 3;
							view_tileset_repeat_timer = 30;
						}
					}

					break;
				}

            case SDL_KEYUP: {
                if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_RIGHT) {
						view_tileset_repeat_direction = -1;
						view_tileset_repeat_timer = 0;
					}

					break;
				}

            case SDL_MOUSEBUTTONDOWN: {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    if (fInValidTile) {
							set_tile_start_x = iCol;
							set_tile_start_y = iRow;
							set_tile_end_x = set_tile_start_x;
							set_tile_end_y = set_tile_start_y;

							set_tile_drag = true;
						}
                } else if (event.button.button == SDL_BUTTON_RIGHT) {
						TileType * type = &animatedtiletypes[iCol + (iRow << 5)];
						set_type = *type = GetIncrementedTileType(*type);
					}

					break;
				}

            case SDL_MOUSEBUTTONUP: {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    if (fInValidTile) {
							set_tile_cols = set_tile_end_x - set_tile_start_x + 1;
							set_tile_rows = set_tile_end_y - set_tile_start_y + 1;

							set_tile_drag = false;
							edit_mode = 8;  //change to edit mode using tiles
                            editor_animation_initialized = false;
							return EDITOR_EDIT;

						}
					}

					break;
				}

            case SDL_MOUSEMOTION: {
                if (fInValidTile) {
                    if (event.motion.state == SDL_BUTTON(SDL_BUTTON_LEFT)) {
							if (iCol < set_tile_start_x)
								set_tile_start_x = iCol;

							if (iCol > set_tile_end_x)
								set_tile_end_x = iCol;

							if (iRow < set_tile_start_y)
								set_tile_start_y = iRow;

							if (iRow > set_tile_end_y)
								set_tile_end_y = iRow;
                    } else if (event.motion.state == SDL_BUTTON(SDL_BUTTON_RIGHT)) {
							animatedtiletypes[iCol + (iRow << 5)] = set_type;
						}
					}

					break;
				}

				default:
					break;
			}
		}

		//Allow auto-scrolling of tilesets when the arrow keys are held down
        if (view_tileset_repeat_direction >= 0 && view_tileset_repeat_timer > 0) {
            if (--view_tileset_repeat_timer <= 0) {
				view_tileset_repeat_timer = 5;

                if (view_tileset_repeat_direction == 2 && view_animated_tileset_x > 0) {
					view_animated_tileset_x--;
                } else if (view_tileset_repeat_direction == 3 && view_animated_tileset_x < 12) {
					view_animated_tileset_x++;
				}
			}
		}

		//drawmap(false, TILESIZE);
		//rm->menu_shade.draw(0, 0);
		SDL_FillRect(screen, NULL, 0xFF888888);

        for (short iCol = view_animated_tileset_x; iCol < view_animated_tileset_x + 20; iCol++) {
            for (short iRow = 0; iRow < 8; iRow++) {
				short iDestX = (iCol - view_animated_tileset_x) << 5;
				short iDestY = iRow << 5;
				short iSrcX = iRow << 7;
				short iSrcY = iCol << 5;

				rm->spr_tileanimation[0].draw(iDestX, iDestY, iSrcX, iSrcY, TILESIZE, TILESIZE);

				TileType t = animatedtiletypes[iCol + (iRow << 5)];
				if (t != tile_nonsolid)
					rm->spr_tiletypes.draw(iDestX, iDestY, (t-1) << 3, 0, 8, 8);
			}
		}

        if (set_tile_drag) {
            for (short i = set_tile_start_x - view_animated_tileset_x; i <= set_tile_end_x - view_animated_tileset_x; i++) {
                for (short j = set_tile_start_y; j <= set_tile_end_y; j++) {
					rm->spr_selectedtile.draw(i << 5, j << 5);
				}
			}
		}

		rm->menu_font_small.drawRightJustified(640, 0, maplist->currentFilename());

		rm->menu_font_small.draw(0, 480 - rm->menu_font_small.getHeight(), "Use Arrow Keys To Scroll");

		DrawMessage();
		return EDITOR_ANIMATION;
}

void LoadBackgroundPage(SDL_Surface ** sBackgrounds, short iPage)
{
	SDL_Rect srcRectBackground = {0, 0, 640, 480};
	SDL_Rect dstRectBackground = {0, 0, 160, 120};

    for (short iIndex = 0; iIndex < 16; iIndex++) {
        const char * szFileName = backgroundlist->GetIndex(iPage * 16 + iIndex);

		if (!szFileName)
			return;

		SDL_Surface * temp = IMG_Load(szFileName);

		SDL_Surface * sBackground = SDL_DisplayFormat(temp);
        if (!sBackground) {
			printf("ERROR: Couldn't convert thumbnail background to diplay pixel format: %s\n", SDL_GetError());
			return;
		}

		SDL_FreeSurface(temp);

        if (SDL_SCALEBLIT(sBackground, &srcRectBackground, sBackgrounds[iIndex], &dstRectBackground) < 0) {
			fprintf(stderr, "SDL_SCALEBLIT error: %s\n", SDL_GetError());
			return;
		}

		SDL_FreeSurface(sBackground);
	}
}

int display_help()
{
	//unsigned int currentChar = 0;

	drawmap(false, TILESIZE);
	rm->menu_shade.draw(0, 0);
	//rm->menu_font_large.drawCentered(320, 15, "Help");

	int offsety = 10;
	int offsetx = 20;
	rm->menu_font_small.draw(offsetx, offsety, "Modes:");
	offsety += rm->menu_font_small.getHeight() + 2;
	rm->menu_font_small.draw(offsetx, offsety, "[t] - Tile Mode");
	offsety += rm->menu_font_small.getHeight() + 2;
	rm->menu_font_small.draw(offsetx, offsety, "[i] - Block Mode");
	offsety += rm->menu_font_small.getHeight() + 2;
	rm->menu_font_small.draw(offsetx, offsety, "[o] - Map Item Mode");
	offsety += rm->menu_font_small.getHeight() + 2;
	rm->menu_font_small.draw(offsetx, offsety, "[w] - Warp Mode");
	offsety += rm->menu_font_small.getHeight() + 2;
	rm->menu_font_small.draw(offsetx, offsety, "[m] - Move Mode");
	offsety += rm->menu_font_small.getHeight() + 2;
	rm->menu_font_small.draw(offsetx, offsety, "[l] - Tile Type Mode");
	offsety += rm->menu_font_small.getHeight() + 2;
	rm->menu_font_small.draw(offsetx, offsety, "[p] - Platform Mode");
	offsety += rm->menu_font_small.getHeight() + 2;
	rm->menu_font_small.draw(offsetx, offsety, "[x] - No Player Spawn Area");
	offsety += rm->menu_font_small.getHeight() + 2;
	rm->menu_font_small.draw(offsetx, offsety, "[z] - No Item Spawn Area");
	offsety += rm->menu_font_small.getHeight() + 2;
	rm->menu_font_small.draw(offsetx, offsety, "[h] - Hazard Mode");
	offsety += rm->menu_font_small.getHeight() + 2;
	rm->menu_font_small.draw(offsetx, offsety, "[a] - Animated Tile Mode");
	offsety += rm->menu_font_small.getHeight() + 2;
	rm->menu_font_small.draw(offsetx, offsety, "[k] - Block Properties");
	offsety += rm->menu_font_small.getHeight() + 20;

	rm->menu_font_small.draw(offsetx, offsety, "Layers:");
	offsety += rm->menu_font_small.getHeight() + 2;
	rm->menu_font_small.draw(offsetx, offsety, "[v] - Hide Blocks");
	offsety += rm->menu_font_small.getHeight() + 2;
	rm->menu_font_small.draw(offsetx, offsety, "[y] - Select Active Tile Layer");
	offsety += rm->menu_font_small.getHeight() + 2;
	rm->menu_font_small.draw(offsetx, offsety, "[u] - Hide Inactive Tile Layers");
	offsety += rm->menu_font_small.getHeight() + 2;
	rm->menu_font_small.draw(offsetx, offsety, "[end] - Optimize Layers");
	offsety += rm->menu_font_small.getHeight() + 20;

	rm->menu_font_small.draw(offsetx, offsety, "Miscellaneous:");
	offsety += rm->menu_font_small.getHeight() + 2;
	rm->menu_font_small.draw(offsetx, offsety, "[b] - Background Thumbnails");
	offsety += rm->menu_font_small.getHeight() + 2;
	rm->menu_font_small.draw(offsetx, offsety, "[g] - Change Backgrounds");
	offsety += rm->menu_font_small.getHeight() + 2;
	rm->menu_font_small.draw(offsetx, offsety, "[r] - Change Music Category");
	offsety += rm->menu_font_small.getHeight() + 2;
	rm->menu_font_small.draw(offsetx, offsety, "[e] - Change Floating Eyecandy");
	offsety += rm->menu_font_small.getHeight() + 2;
	rm->menu_font_small.draw(offsetx, offsety, "[ctrl] + [delete] - Clear All");
	offsety += rm->menu_font_small.getHeight() + 2;
	rm->menu_font_small.draw(offsetx, offsety, "[insert] - Take Screenshot");
	offsety += rm->menu_font_small.getHeight() + 2;


	offsetx = 305;
	offsety = 10;

	rm->menu_font_small.draw(offsetx, offsety, "File:");
	offsety += rm->menu_font_small.getHeight() + 2;
	rm->menu_font_small.draw(offsetx, offsety, "[n] - New Map");
	offsety += rm->menu_font_small.getHeight() + 2;
	rm->menu_font_small.draw(offsetx, offsety, "[s] - Save Map");
	offsety += rm->menu_font_small.getHeight() + 2;
	rm->menu_font_small.draw(offsetx, offsety, "[shift] + [s] - Save As");
	offsety += rm->menu_font_small.getHeight() + 2;
	rm->menu_font_small.draw(offsetx, offsety, "[f] - Find Map");
	offsety += rm->menu_font_small.getHeight() + 2;
	rm->menu_font_small.draw(offsetx, offsety, "[shift] + [f] - New Search");
	offsety += rm->menu_font_small.getHeight() + 2;
	rm->menu_font_small.draw(offsetx, offsety, "[pageup] - Go To Previous Map");
	offsety += rm->menu_font_small.getHeight() + 2;
	rm->menu_font_small.draw(offsetx, offsety, "[pagedown] - Go To Next Map");
	offsety += rm->menu_font_small.getHeight() + 20;

	rm->menu_font_small.draw(offsetx, offsety, "Tile, Warp and Block Modes:");
	offsety += rm->menu_font_small.getHeight() + 2;
	rm->menu_font_small.draw(offsetx, offsety, "[Left Mouse Button] - Place Item");
	offsety += rm->menu_font_small.getHeight() + 2;
	rm->menu_font_small.draw(offsetx, offsety, "[Right Mouse Button] - Remove Item");
	offsety += rm->menu_font_small.getHeight() + 20;

	rm->menu_font_small.draw(offsetx, offsety, "Move Mode:");
	offsety += rm->menu_font_small.getHeight() + 2;
	rm->menu_font_small.draw(offsetx, offsety, "[Left Mouse Button] - Select Area");
	offsety += rm->menu_font_small.getHeight() + 2;
	rm->menu_font_small.draw(offsetx, offsety, "[Right Mouse Button] - Unselect Area");
	offsety += rm->menu_font_small.getHeight() + 2;
	rm->menu_font_small.draw(offsetx, offsety, "Select And Drag - Move Selections");
	offsety += rm->menu_font_small.getHeight() + 2;
	rm->menu_font_small.draw(offsetx, offsety, "Hold [shift] - Multiple Selections");
	offsety += rm->menu_font_small.getHeight() + 2;
	rm->menu_font_small.draw(offsetx, offsety, "Hold [ctrl] - Freehand Selections");
	offsety += rm->menu_font_small.getHeight() + 2;
	rm->menu_font_small.draw(offsetx, offsety, "[delete] - Delete Selection");
	offsety += rm->menu_font_small.getHeight() + 2;
	rm->menu_font_small.draw(offsetx, offsety, "[c] - Copy Selection");
	offsety += rm->menu_font_small.getHeight() + 20;

	rm->menu_font_small.draw(offsetx, offsety, "Platforms:");
	offsety += rm->menu_font_small.getHeight() + 2;
	rm->menu_font_small.draw(offsetx, offsety, "[p] - Path");
	offsety += rm->menu_font_small.getHeight() + 2;
	rm->menu_font_small.draw(offsetx, offsety, "[+/-] - Change Speed");
	offsety += rm->menu_font_small.getHeight() + 2;
	rm->menu_font_small.draw(offsetx, offsety, "[delete] - Delete");
	offsety += rm->menu_font_small.getHeight() + 2;
	rm->menu_font_small.draw(offsetx, offsety, "[alt] + [enter] - Full Screen/Window");


		//handle messages
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
				case SDL_QUIT:
					return 0;
				break;

				case SDL_KEYDOWN:
					return 0;
				break;

				default:
					break;
			}
		}

	return DISPLAY_HELP;
}

int save_as()
{
	char fileName[FILEBUFSIZE] = "";
	char mapLocation[FILEBUFSIZE] = "maps/";

    if (dialog("Save As", "Enter name:", fileName, 64)) {
		save_map(convertPath(strcat(strcat(mapLocation, fileName),".map")));
		maplist->add(strcat(fileName, ".map"));
		maplist->find(fileName);
		loadcurrentmap();
	}

	return 0;
}

bool dialog(const char * title, const char * instructions, char * input, int inputsize)
{
// FIXME
#ifdef __EMSCRIPTEN__
    return false;
#endif
	unsigned int currentChar = 0;

	drawmap(false, TILESIZE);
	rm->menu_shade.draw(0, 0);
	rm->spr_dialog.draw(224, 176, 0, 0, 192, 128);
	rm->menu_font_large.drawCentered(320, 200, title);
	rm->menu_font_small.draw(240, 235, instructions);
	rm->menu_font_small.drawRightJustified(640, 0, maplist->currentFilename());
	SDL_Flip(screen);

    while (true) {
		int framestart = SDL_GetTicks();

		//handle messages
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
				case SDL_QUIT:
					return false;
				break;

				case SDL_KEYDOWN:

                if (event.key.keysym.sym == SDLK_KP_ENTER || event.key.keysym.sym == SDLK_RETURN) {
						return true;
                } else if (event.key.keysym.sym == SDLK_ESCAPE) {
						return false;
                } else if (event.key.keysym.sym == SDLK_BACKSPACE) {
                    if (currentChar > 0) {
							input[currentChar-1] = '\0';

							drawmap(false, TILESIZE);
							rm->menu_shade.draw(0, 0);
							rm->spr_dialog.draw(224, 176, 0, 0, 192, 128);
							rm->menu_font_large.drawCentered(320, 200, title);
							rm->menu_font_small.draw(240, 235, instructions);
							rm->menu_font_small.draw(240, 255, input);
							rm->menu_font_small.drawRightJustified(640, 0, maplist->currentFilename());
							SDL_Flip(screen);

							currentChar--;
						}
                } else {

						/* I realize the if statement below is long and can be substituted with
						the function isalnum(event.key.keysym.sym) but I did it this way because
						isalnum acts funny (ie wrong) when the number pad is pressed. */
                    if ((isdigit(event.key.keysym.sym) || event.key.keysym.sym == 45 || event.key.keysym.sym == 32 || event.key.keysym.sym == 61 || (event.key.keysym.sym >= 95 && event.key.keysym.sym <= 122)) && currentChar < (unsigned)inputsize - 1) {
							//insert character into fileName and onScreenText and increment current char
							Uint8 key = event.key.keysym.sym;

                        #if defined(USE_SDL2) || defined(__EMSCRIPTEN__)
                            const Uint8 * keystate = SDL_GetKeyboardState(NULL);
                        #else
                            Uint8 * keystate = SDL_GetKeyState(NULL);
                        #endif
                        if (keystate[SDLK_LSHIFT] || keystate[SDLK_RSHIFT]) {
								if (event.key.keysym.sym == 45)
									key = 95;
								else if (event.key.keysym.sym >= 95 && event.key.keysym.sym <= 122)
									key-= 32;  //Capitalize
								else if (event.key.keysym.sym == 48)
									key = 41;
								else if (event.key.keysym.sym == 49)
									key = 33;
								else if (event.key.keysym.sym == 50)
									key = 64;
								else if (event.key.keysym.sym == 51)
									key = 35;
								else if (event.key.keysym.sym == 52)
									key = 36;
								else if (event.key.keysym.sym == 53)
									key = 37;
								else if (event.key.keysym.sym == 54)
									key = 94;
								else if (event.key.keysym.sym == 55)
									key = 38;
								else if (event.key.keysym.sym == 57)
									key = 40;
								else if (event.key.keysym.sym == 61)
									key = 43;
							}

							input[currentChar] = key;
							currentChar++;
							input[currentChar] = '\0';

							drawmap(false, TILESIZE);
							rm->menu_shade.draw(0, 0);
							rm->spr_dialog.draw(224, 176, 0, 0, 192, 128);
							rm->menu_font_large.drawCentered(320, 200, title);
							rm->menu_font_small.draw(240, 235, instructions);
							rm->menu_font_small.draw(240, 255, input);
							rm->menu_font_small.drawRightJustified(640, 0, maplist->currentFilename());
							SDL_Flip(screen);
						}
					}
				break;

				default:
					break;
			}
		}

		int delay = WAITTIME - (SDL_GetTicks() - framestart);
		if (delay < 0)
			delay = 0;
		else if (delay > WAITTIME)
			delay = WAITTIME;

		SDL_Delay(delay);
	}

	return false;
}

int find()
{
	char fileName[FILEBUFSIZE] = "";
	//char mapLocation[FILEBUFSIZE] = "maps/";

    if (dialog("Find Map", "Enter name:", fileName, 64)) {
		strcpy(findstring, fileName);

        if (maplist->find(findstring)) {
			loadcurrentmap();
		}
	}

	return 0;
}

int clearMap()
{
	g_map->clearMap();
	g_iNumPlatforms = 0;


	printf("Map Cleared\n");
	return 0;
}

#ifdef _DEBUG
void convertAll()
{
    for (int k = 0; k < maplist->GetCount(); k++) {
		//g_map->convertMap();
		g_map->saveMap(maplist->currentFilename());
		maplist->next(false);
		g_map->loadMap(maplist->currentFilename(), read_type_full);
	}
}
#endif


void loadcurrentmap()
{
	g_map->loadMap(maplist->currentFilename(), read_type_full);

    if (g_map->iNumRaceGoals == 0) {
        for (short iGoal = 0; iGoal < MAXRACEGOALS; iGoal++) {
			g_map->racegoallocations[iGoal].x = iGoal * 80 + 20;
			g_map->racegoallocations[iGoal].y = 18;
		}
	}

    if (g_map->iNumFlagBases == 0) {
        for (short iBase = 0; iBase < 4; iBase++) {
			g_map->flagbaselocations[iBase].x = iBase * 80 + 20;
			g_map->flagbaselocations[iBase].y = 18;
		}
	}

	char filename[128];
	sprintf(filename, "gfx/packs/Classic/backgrounds/%s", g_map->szBackgroundFile);
	std::string path = convertPath(filename);
    backgroundlist->SetCurrentName(filename);

    if (!File_Exists(path)) {
		path = convertPath("gfx/packs/Classic/backgrounds/Land_Classic.png");
        backgroundlist->SetCurrentName("gfx/packs/Classic/backgrounds/Land_Classic.png");
	}

	rm->spr_background.init(path);

	g_iNumPlatforms = g_map->iNumPlatforms;

    for (short iPlatform = 0; iPlatform < g_iNumPlatforms; iPlatform++) {
        for (short iCol = 0; iCol < MAPWIDTH; iCol++) {
            for (short iRow = 0; iRow < MAPHEIGHT; iRow++) {
                if (iCol < g_map->platforms[iPlatform]->iTileWidth && iRow < g_map->platforms[iPlatform]->iTileHeight) {
					CopyTilesetTile(&g_Platforms[iPlatform].tiles[iCol][iRow], &g_map->platforms[iPlatform]->iTileData[iCol][iRow]);
					g_Platforms[iPlatform].types[iCol][iRow] = g_map->platforms[iPlatform]->iTileType[iCol][iRow].iType;
                } else {
					ClearTilesetTile(&g_Platforms[iPlatform].tiles[iCol][iRow]);
					g_Platforms[iPlatform].types[iCol][iRow] = tile_nonsolid;
				}
			}
		}

		short iDrawLayer = g_map->platforms[iPlatform]->iDrawLayer;
		g_Platforms[iPlatform].iDrawLayer = iDrawLayer;

		short iPathType = g_map->platforms[iPlatform]->pPath->GetType();
		g_Platforms[iPlatform].iPathType = iPathType;

        if (iPathType == 0) {
			StraightPath * path = (StraightPath*)g_map->platforms[iPlatform]->pPath;
			g_Platforms[iPlatform].iVelocity = (int)(path->dVelocity * 4.0f);
			g_Platforms[iPlatform].iStartX = (int)(path->dPathPointX[0]);
			g_Platforms[iPlatform].iStartY = (int)(path->dPathPointY[0]);
			g_Platforms[iPlatform].iEndX = (int)(path->dPathPointX[1]);
			g_Platforms[iPlatform].iEndY = (int)(path->dPathPointY[1]);
        } else if (iPathType == 1) {
			StraightPathContinuous * path = (StraightPathContinuous*)g_map->platforms[iPlatform]->pPath;
			g_Platforms[iPlatform].iVelocity = (int)(path->dVelocity * 4.0f);
			g_Platforms[iPlatform].iStartX = (int)(path->dPathPointX[0]);
			g_Platforms[iPlatform].iStartY = (int)(path->dPathPointY[0]);
			g_Platforms[iPlatform].fAngle = path->dAngle;
        } else if (iPathType == 2) {
			EllipsePath * path = (EllipsePath*)g_map->platforms[iPlatform]->pPath;

			g_Platforms[iPlatform].iVelocity = (short)(path->dVelocity / 0.0030f);
			g_Platforms[iPlatform].fRadiusX = path->dRadiusX;
			g_Platforms[iPlatform].fRadiusY = path->dRadiusY;
			g_Platforms[iPlatform].iStartX = (int)(path->dPathPointX[0]);
			g_Platforms[iPlatform].iStartY = (int)(path->dPathPointY[0]);
			g_Platforms[iPlatform].fAngle = path->dStartAngle;
		}

		g_Platforms[iPlatform].UpdatePreview();
	}
}

void SetPlatformToDefaults(short iPlatform)
{
    for (short iCol = 0; iCol < MAPWIDTH; iCol++) {
        for (short iRow = 0; iRow < MAPHEIGHT; iRow++) {
			ClearTilesetTile(&g_Platforms[iPlatform].tiles[iCol][iRow]);
			g_Platforms[iPlatform].types[iCol][iRow] = tile_nonsolid;
		}
	}

	g_Platforms[iPlatform].iVelocity = 4;
	g_Platforms[iPlatform].iStartX = 320;
	g_Platforms[iPlatform].iStartY = 240;
	g_Platforms[iPlatform].iEndX = 352;
	g_Platforms[iPlatform].iEndY = 240;

	g_Platforms[iPlatform].fAngle = 0.0f;
	g_Platforms[iPlatform].fRadiusX = 128.0f;
	g_Platforms[iPlatform].fRadiusY = 128.0f;

	g_Platforms[iPlatform].iDrawLayer = 2;
}

int savecurrentmap()
{
	g_messagedisplaytimer = 60;
	g_szMessageTitle = "Saved";
	g_szMessageLine[0] = "Your map has";
	g_szMessageLine[1] = "been saved.";
	g_szMessageLine[2] = "";

	save_map(maplist->currentFilename());
	return 0;
}

void insert_platforms_into_map()
{
	//First take the created platforms and move them into the actual map
	g_map->clearPlatforms();

	//g_map->iNumPlatforms = g_iNumPlatforms;
	g_map->platforms = new MovingPlatform*[g_iNumPlatforms];

    for (short iPlatform = 0; iPlatform < g_iNumPlatforms; iPlatform++) {
		short iTop, iLeft, iWidth, iHeight;
		CalculatePlatformDims(iPlatform, &iLeft, &iTop, &iWidth, &iHeight);

		TilesetTile ** tiles = new TilesetTile*[iWidth];
		MapTile ** types = new MapTile*[iWidth];

        for (short iCol = 0; iCol < iWidth; iCol++) {
			tiles[iCol] = new TilesetTile[iHeight];
			types[iCol] = new MapTile[iHeight];

            for (short iRow = 0; iRow < iHeight; iRow++) {
				CopyTilesetTile(&tiles[iCol][iRow], &g_Platforms[iPlatform].tiles[iCol + iLeft][iRow + iTop]);
				types[iCol][iRow].iType = g_Platforms[iPlatform].types[iCol + iLeft][iRow + iTop];
			}
		}

		short iDrawLayer = g_Platforms[iPlatform].iDrawLayer;

		float fStartX = (float)(g_Platforms[iPlatform].iStartX);
		float fStartY = (float)(g_Platforms[iPlatform].iStartY);

		MovingPlatformPath * path = NULL;

        if (g_Platforms[iPlatform].iPathType == 0) {
			float fVelocity = (float)g_Platforms[iPlatform].iVelocity * 0.26f;
			float fEndX = (float)(g_Platforms[iPlatform].iEndX);
			float fEndY = (float)(g_Platforms[iPlatform].iEndY);

			path = new StraightPath(fVelocity, fStartX, fStartY, fEndX, fEndY, false);
        } else if (g_Platforms[iPlatform].iPathType == 1) {
			float fVelocity = (float)g_Platforms[iPlatform].iVelocity * 0.26f;
			path = new StraightPathContinuous(fVelocity, fStartX, fStartY, g_Platforms[iPlatform].fAngle, false);
        } else if (g_Platforms[iPlatform].iPathType == 2) {
			float fVelocity = (float)g_Platforms[iPlatform].iVelocity * 0.0030f;
			path = new EllipsePath(fVelocity, g_Platforms[iPlatform].fAngle, g_Platforms[iPlatform].fRadiusX, g_Platforms[iPlatform].fRadiusY, fStartX, fStartY, false);
		}

		g_map->AddPermanentPlatform(new MovingPlatform(tiles, types, iWidth, iHeight, iDrawLayer, path, false));
	}
}

void save_map(const std::string &file)
{
	insert_platforms_into_map();

	//Then save the rest of the map
	g_map->saveMap(file);
}

void CalculatePlatformDims(short iPlatform, short * ix, short * iy, short * iw, short * ih)
{
	short iTop = MAPHEIGHT, iRight = -1, iBottom = -1, iLeft = MAPWIDTH;
	//Calculate the height and width of the platform
    for (short iCol = 0; iCol < MAPWIDTH; iCol++) {
        for (short iRow = 0; iRow < MAPHEIGHT; iRow++) {
            if (g_Platforms[iPlatform].tiles[iCol][iRow].iID != TILESETNONE) {
				if (iTop > iRow)
					iTop = iRow;

				if (iLeft > iCol)
					iLeft = iCol;

				if (iBottom < iRow)
					iBottom = iRow;

				if (iRight < iCol)
					iRight = iCol;
			}
		}
	}

	short iWidth = iRight - iLeft + 1;
	short iHeight = iBottom - iTop + 1;

    if (iRight == -1 || iBottom == -1) {
		iWidth = 1;
		iHeight = 1;
		iLeft = 0;
		iTop = 0;
	}

	*ix = iLeft;
	*iy = iTop;

	*iw = iWidth;
	*ih = iHeight;
}

int findcurrentstring()
{
    if (findstring[0] != '\0') {
        if (maplist->find(findstring)) {
			loadcurrentmap();
		}
	}

	return 0;
}

int newmap()
{
	char fileName[FILEBUFSIZE] = "";
	char mapLocation[FILEBUFSIZE] = "maps/";

    if (dialog("New Map", "Enter name:", fileName, 64)) {
		g_map->clearMap();
		g_map->clearPlatforms();
		g_map->saveMap(convertPath(strcat(strcat(mapLocation, fileName),".map")));
		maplist->add(strcat(fileName, ".map"));
		maplist->find(fileName);
		loadcurrentmap();
	}

	return 0;
}

void resetselectedtiles()
{
    for (int k = 0; k < MAPHEIGHT; k++) {
        for (int j = 0; j < MAPWIDTH; j++) {
			selectedtiles[j][k] = false;
		}
	}
}

void copymoveselection()
{
    for (int k = 0; k < MAPHEIGHT; k++) {
        for (int j = 0; j < MAPWIDTH; j++) {
			moveselectedtiles[j][k] = selectedtiles[j][k];
		}
	}

	copiedlayer = selected_layer;
}

void pastemoveselection(int movex, int movey)
{
    for (int k = 0; k < MAPHEIGHT; k++) {
        for (int j = 0; j < MAPWIDTH; j++) {
            if (moveselectedtiles[j][k]) {
				if (j + movex >= 0 && j + movex < MAPWIDTH &&
                        k + movey >= 0 && k + movey < MAPHEIGHT) {
					selectedtiles[j + movex][k + movey] = moveselectedtiles[j][k];
				}
			}
		}
	}
}

bool copyselectedtiles()
{
	//Copy the selected tiles and remove tiles from map
	bool ret = false;
    for (int k = 0; k < MAPHEIGHT; k++) {
        for (int j = 0; j < MAPWIDTH; j++) {
            if (selectedtiles[j][k]) {
				ret = true;
                for (short iLayer = 0; iLayer < MAPLAYERS; iLayer++) {
					CopyTilesetTile(&copiedtiles[j][k].tile[iLayer], &g_map->mapdata[j][k][iLayer]);
				}

				copiedtiles[j][k].block.iType = g_map->objectdata[j][k].iType;
				for (short iSetting = 0; iSetting < NUM_BLOCK_SETTINGS; iSetting++)
					copiedtiles[j][k].block.iSettings[iSetting] = g_map->objectdata[j][k].iSettings[iSetting];
				copiedtiles[j][k].block.fHidden = g_map->objectdata[j][k].fHidden;

				copiedtiles[j][k].warp.connection = g_map->warpdata[j][k].connection;
				copiedtiles[j][k].warp.direction = g_map->warpdata[j][k].direction;
				copiedtiles[j][k].warp.id = g_map->warpdata[j][k].id;

				copiedtiles[j][k].tiletype = g_map->mapdatatop[j][k].iType;

				for (short iType = 0; iType < NUMSPAWNAREATYPES; iType++)
					copiedtiles[j][k].nospawn[iType] = g_map->nospawn[iType][j][k];

				copiedtiles[j][k].item = -1;
                for (short iMapItem = 0; iMapItem < g_map->iNumMapItems; iMapItem++) {
                    if (g_map->mapitems[iMapItem].ix == j && g_map->mapitems[iMapItem].iy == k) {
						copiedtiles[j][k].item = g_map->mapitems[iMapItem].itype;
						break;
					}
				}
			}
		}
	}

	copiedlayer = selected_layer;

	return ret;
}

void clearselectedmaptiles()
{
    for (int k = 0; k < MAPHEIGHT; k++) {
        for (int j = 0; j < MAPWIDTH; j++) {
            if (selectedtiles[j][k]) {
                if (view_only_layer) {
					g_map->mapdata[j][k][selected_layer].iID = TILESETNONE;
                } else {
					for (short iLayer = 0; iLayer < MAPLAYERS; iLayer++)
						g_map->mapdata[j][k][iLayer].iID = TILESETNONE;

					g_map->objectdata[j][k].iType = -1;

					g_map->warpdata[j][k].connection = -1;
					g_map->warpdata[j][k].direction = WARP_UNDEFINED;
					g_map->warpdata[j][k].id = -1;

					for (short iType = 0; iType < NUMSPAWNAREATYPES; iType++)
						g_map->nospawn[iType][j][k] = false;

					RemoveMapItemAt(j, k);
				}

				UpdateTileType(j, k);
			}
		}
	}
}

void replacetile(short * iDstTile, short iSrcTile, bool fAllowReplace)
{
    if (move_replace) {
		*iDstTile = iSrcTile;
    } else {
		if (fAllowReplace)
			*iDstTile = iSrcTile;
	}
}

void copytilesettile(TilesetTile * dst, TilesetTile * src)
{
	dst->iID = src->iID;
	dst->iCol = src->iCol;
	dst->iRow = src->iRow;
}

void replacetile(TilesetTile * dstTile, TilesetTile * srcTile)
{
    if (move_replace) {
		copytilesettile(dstTile, srcTile);
    } else {
		if (srcTile->iID != TILESETNONE)
			copytilesettile(dstTile, srcTile);
	}
}

void copymapblock(MapBlock * dst, MapBlock * src)
{
	dst->iType = src->iType;
	dst->fHidden = src->fHidden;

	for (short iSetting = 0; iSetting < NUM_BLOCK_SETTINGS; iSetting++)
		dst->iSettings[iSetting] = src->iSettings[iSetting];
}

void replacetile(MapBlock * dstTile, MapBlock * srcTile)
{
    if (move_replace) {
		copymapblock(dstTile, srcTile);
    } else {
		if (srcTile->iType != -1)
			copymapblock(dstTile, srcTile);
	}
}

void pasteselectedtiles(int movex, int movey)
{
	//Paste the tiles into their new location
	//Removing tiles that hang over the edges of the map
    for (int k = 0; k < MAPHEIGHT; k++) {
        for (int j = 0; j < MAPWIDTH; j++) {
            if (selectedtiles[j][k]) {
				if (j + movex >= 0 && j + movex < MAPWIDTH &&
                        k + movey >= 0 && k + movey < MAPHEIGHT) {
					short iNewX = j + movex;
					short iNewY = k + movey;

                    if (view_only_layer) {
						replacetile(&g_map->mapdata[iNewX][iNewY][selected_layer], &copiedtiles[j][k].tile[copiedlayer]);
                    } else {
						for (short iLayer = 0; iLayer < MAPLAYERS; iLayer++)
							replacetile(&g_map->mapdata[iNewX][iNewY][iLayer], &copiedtiles[j][k].tile[iLayer]);

						replacetile(&g_map->objectdata[iNewX][iNewY], &copiedtiles[j][k].block);

						replacetile(&g_map->warpdata[iNewX][iNewY].connection, copiedtiles[j][k].warp.connection, copiedtiles[j][k].warp.connection != -1);
						replacetile((short*)&g_map->warpdata[iNewX][iNewY].direction, (short)copiedtiles[j][k].warp.direction, copiedtiles[j][k].warp.connection != -1);
						replacetile(&g_map->warpdata[iNewX][iNewY].id, copiedtiles[j][k].warp.id, copiedtiles[j][k].warp.connection != -1);

						if (move_replace)
							g_map->mapdatatop[iNewX][iNewY].iType = copiedtiles[j][k].tiletype;
						else
							UpdateTileType(j, k);

						for (short iType = 0; iType < NUMSPAWNAREATYPES; iType++)
							g_map->nospawn[iType][iNewX][iNewY] = copiedtiles[j][k].nospawn[iType];

						//if (move_replace)
						//	RemoveMapItemAt(j, k);

                        if (g_map->iNumMapItems < MAXMAPITEMS && copiedtiles[j][k].item >= 0) {
						//	if (!move_replace)
						//		RemoveMapItemAt(j, k);

							MapItem * mapitem = &g_map->mapitems[g_map->iNumMapItems];
							mapitem->itype = copiedtiles[j][k].item;
							mapitem->ix = iNewX;
							mapitem->iy = iNewY;

							g_map->iNumMapItems++;
						}
					}
				}
			}
		}
	}
}

void getcenterselection(int * x, int * y)
{
	int left = -1;
	int top = -1;
	int right = -1;
	int bottom = -1;

    for (int k = 0; k < MAPHEIGHT; k++) {
        for (int j = 0; j < MAPWIDTH; j++) {
            if (selectedtiles[j][k]) {
				if (left == -1)
					left = j;

				if (top == -1)
					top = k;

				right = j;
				bottom = k;
			}
		}
	}

    if (right == -1) {
		*x = 0;
		*y = 0;
		return;
	}

	*x = ((right - left) >> 1) + left;
	*y = ((bottom - top) >> 1) + top;
}

//take screenshots in full and thumbnail sizes
void takescreenshot()
{
	short iTileSizes[3] = {TILESIZE, PREVIEWTILESIZE, THUMBTILESIZE};
	SDL_Surface * old_screen = screen;

    for (short iScreenshotSize = 0; iScreenshotSize < 3; iScreenshotSize++) {
		short iTileSize = iTileSizes[iScreenshotSize];

		//Allow wrapping of path dots
		rm->spr_platformpath.SetWrap(true, 640 >> iScreenshotSize);

		//Create new screenshot surface
		SDL_Surface * screenshot = SDL_CreateRGBSurface(old_screen->flags, iTileSize * 20, iTileSize * 15, old_screen->format->BitsPerPixel, 0, 0, 0, 0);
		blitdest = screenshot;
		screen = screenshot;

		//Draw map to screenshot
		drawmap(true, iTileSize);

		//Draw platforms to screenshot
        for (short iPlatform = 0; iPlatform < g_iNumPlatforms; iPlatform++) {
			MapPlatform * platform = &g_Platforms[iPlatform];
			DrawPlatform(platform->iPathType, platform->tiles, platform->iStartX, platform->iStartY, platform->iEndX, platform->iEndY, platform->fAngle, platform->fRadiusX, platform->fRadiusY, iScreenshotSize, g_map->platforms[iPlatform]->iTileWidth, g_map->platforms[iPlatform]->iTileHeight, true, true);
		}

		//Draw map hazards
		for (short iHazard = 0; iHazard < g_map->iNumMapHazards; iHazard++)
			DrawMapHazard(&g_map->maphazards[iHazard], iScreenshotSize, false);

		//Save the screenshot with the same name as the map file
		char szSaveFile[256];
		strcpy(szSaveFile, "maps/screenshots/");
		char * pszSaveFile = szSaveFile + strlen(szSaveFile);
		GetNameFromFileName(pszSaveFile, maplist->currentFilename());

		if (iTileSize == PREVIEWTILESIZE)
			strcat(szSaveFile, "_preview");
		else if (iTileSize == THUMBTILESIZE)
			strcat(szSaveFile, "_thumb");

#ifdef PNG_SAVE_FORMAT
		strcat(szSaveFile, ".png");
		IMG_SavePNG(screenshot, szSaveFile);
#else
		strcat(szSaveFile, ".bmp");
		SDL_SaveBMP(screenshot, szSaveFile);
#endif

		SDL_FreeSurface(screenshot);

		printf("Screenshot taken: %s\n", szSaveFile);
	}

	screen = old_screen;
	blitdest = screen;
}

bool ReadAnimatedTileTypeFile(const char * szFile)
{
	//Detect if the tiletype file already exists, if not create it
    if (File_Exists(szFile)) {
		BinaryFile tsf(szFile, "rb");
        if (!tsf.is_open()) {
			printf("ERROR: couldn't open tileset file: %s\n", szFile);
			return false;
		}

		animatedtiletypes = new TileType[256];

        for (short i = 0; i < 256; i++) {
			animatedtiletypes[i] = (TileType)tsf.read_i32();
		}

    } else {
		animatedtiletypes = new TileType[256];

        for (short i = 0; i < 256; i++) {
			animatedtiletypes[i] = tile_nonsolid;
		}
	}

	return true;
}

bool WriteAnimatedTileTypeFile(const char * szFile)
{
	BinaryFile tsf(szFile, "wb");
    if (!tsf.is_open()) {
		printf("ERROR: couldn't open tileset file to save tile types: %s\n", szFile);
		return false;
	}

    for (short i = 0; i < 256; i++) {
		tsf.write_i32(animatedtiletypes[i]);
	}

	return true;
}
