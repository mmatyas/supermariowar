/*----------------------------------------------------------+
| Super Mario War World Editor								|
|															|
|															|
| this sourcecode is released under the GPLv2.				|
|															|
| start:		1.1.2008									|
| last changes:	1.10.2009									|
|															|
|  (C) 2003-2009 Florian Hufsky <florian.hufsky@gmail.com>	|
+----------------------------------------------------------*/

//TODO:
//1) Continue testing by creating a fully functional new world
//3) When saving new world, it didn't save the name correct (or at all)

//Checkin

#define SMW_EDITOR

#include "FileIO.h"
#include "FileList.h"
#include "GameMode.h"
#include "GameValues.h"
#include "map.h"
#include "MapList.h"
#include "menu/ModeOptionsMenu.h"
#include "objectgame.h"
#include "ResourceManager.h"
#include "sfx.h"
#include "TilesetManager.h"
#include "ui/MI_MapField.h"
#include "ui/MI_ImageSelectField.h"
#include "world.h"

#ifdef PNG_SAVE_FORMAT
	// this function was added to SDL2
	#ifndef USE_SDL2
	    #include "savepng.h"
	#endif
#endif

#include "SDL_image.h"

#include <cstdlib>
#include <string.h>
#include <ctype.h>

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

// TODO: Fix JS related problems.
#ifdef __EMSCRIPTEN__
#define SDL_Delay(n) ;
#endif

#define MAPTITLESTRING "SMW 2.0 World Editor"

enum {EDITOR_EDIT, EDITOR_WATER, EDITOR_BACKGROUND, EDITOR_STAGEFOREGROUND, EDITOR_STRUCTUREFOREGROUND, EDITOR_BRIDGES, EDITOR_PATHSPRITE, EDITOR_VEHICLES, EDITOR_QUIT, SAVE_AS, FIND, CLEAR_WORLD, NEW_WORLD, RESIZE_WORLD, SAVE, EDITOR_WARP, DISPLAY_HELP, EDITOR_PATH, EDITOR_TYPE, EDITOR_BOUNDARY, EDITOR_START_ITEMS, EDITOR_STAGE};

const char * szEditModes[10] = {"Background Mode", "Foreground Mode", "Path Sprite Mode", "Stage Mode", "Path Mode", "Vehicle Mode", "Warp Mode", "Start/Door Mode", "Boundary Mode", "Stage Mode"};


SDL_Surface		*screen;
SDL_Surface		*blitdest;
SDL_Surface		*sMapSurface;

SDL_Rect		rectSrcSurface = {0, 0, 768, 608};
SDL_Rect		rectDstSurface = {0, 0, 640, 480};
bool			fNeedBlackBackground = false;
short			iWorldWidth, iWorldHeight;

SDL_Event		event;

gfxFont			menu_font_small;
gfxFont			menu_font_large;

gfxSprite		spr_tilesetworld;
gfxSprite		spr_selectedtile;
gfxSprite		spr_dialog;
gfxSprite		menu_shade;
gfxSprite		spr_largedialog;

gfxSprite		spr_warps[3];
gfxSprite		spr_path;

gfxSprite		spr_worldbackground[3];
gfxSprite		spr_worldforeground[3];
gfxSprite		spr_worldforegroundspecial[3];
gfxSprite		spr_worldpaths[3];
gfxSprite		spr_worldvehicle[3];

gfxSprite		spr_storedpowerupsmall;
gfxSprite		spr_worlditems;
gfxSprite		spr_worldpopup;

gfxSprite		spr_worlditemssmall;
gfxSprite		spr_worlditemsplace;

//Mode Options Menu Gfx
gfxSprite		menu_egg;
gfxSprite		menu_plain_field;
gfxSprite		menu_slider_bar;
gfxSprite		menu_stomp;
gfxSprite		menu_survival;
gfxSprite		spr_phanto;
gfxSprite		spr_selectfield;
gfxSprite		menu_verticalarrows;
gfxSprite		spr_storedpoweruplarge;
gfxSprite		menu_mode_small;
gfxSprite		menu_mode_large;

gfxSprite		spr_vehicleicons;

int				set_tile = 0;
bool			fAutoPaint = true;

int				edit_mode = 0;

int				draw_offset_col = 0;  //col and row offset for drawing map to surface
int				draw_offset_row = 0;
int				draw_offset_x = 0;  //x and y offset for drawing maps smaller than screensize
int				draw_offset_y = 0;

int				state;
bool			selectedtiles[MAPWIDTH][MAPHEIGHT];
bool			moveselectedtiles[MAPWIDTH][MAPHEIGHT];
WorldMapTile	copiedtiles[MAPWIDTH][MAPHEIGHT];

//Vehicle structure that holds the current vehicle "stamp"
WorldVehicle g_wvVehicleStamp;

gfxSprite		spr_platformstarttile;
gfxSprite		spr_platformendtile;
gfxSprite		spr_platformpath;

gfxSprite		spr_thumbnail_warps[2];
gfxSprite		spr_thumbnail_mapitems[2];
gfxSprite		spr_tileanimation[3];
gfxSprite		spr_blocks[3];
gfxSprite		spr_unknowntile[3];

//// Global stuff that the map editor doesn't need, but has references to
gfxSprite		spr_warplock;
short			x_shake = 0;
short			y_shake = 0;
extern CGameValues game_values;

void CPlayer::flipsidesifneeded() {}
bool CPlayer::isInvincible() const { return false; }
bool CPlayer::isShielded() const { return false; }
void CPlayer::AddKillerAward(CPlayer*, killstyle) {}
short CPlayer::KillPlayerMapHazard(bool fForce, killstyle style, bool fKillCarriedItem, short iPlayerId)
{
    return 0;
}
bool PlayerCollisions::checktop(CPlayer&)
{
    return false;
}
bool CPlayer::bouncejump()
{
    return false;
}
bool PlayerKuriboShoe::is_on() const { return false; }
bool PlayerTanookiSuit::isStatue() const { return false; }

void B_WeaponBreakableBlock::triggerBehavior(short iPlayerID, short iTeamID) {}

//void IO_MovingObject::flipsidesifneeded() {}
//void IO_MovingObject::KillObjectMapHazard(short playerID) {}

void CO_Egg::placeEgg() {}
void CO_Flag::placeFlag() {}
void CO_PhantoKey::placeKey() {}
void CO_Star::placeStar() {}
void MO_Coin::placeCoin() {}
void OMO_Area::placeArea() {}
void OMO_Area::reset() {}
void OMO_Area::setOwner(CPlayer * player) {}

bool CO_ThrowBox::HasKillVelocity()
{
    return false;
}

short PlayerKilledPlayer(short id, CPlayer * killed, short deathstyle, killstyle style, bool fForce, bool fKillCarriedItem)
{
    return 0;
}
void AddAwardKill(CPlayer * killer, CPlayer * killed, killstyle style) {}

CPlayer * GetPlayerFromGlobalID(short id)
{
    return NULL;
}

/*float CapFallingVelocity(float f)
{
    return 0.0f;
}*/
void removeifprojectile(IO_MovingObject * object, bool playsound, bool forcedead) {}
bool LoadMenuSkin(short playerID, short skinID, short colorID, bool fLoadBothDirections)
{
    return false;
}

gfxSprite		spr_awardsouls, spr_fireballexplosion;
gfxSprite		spr_backmap[2];
gfxSprite		spr_background;
gfxSprite		spr_explosion;
gfxSprite		spr_hazard_bulletbilldead;

sfxSound		sfx_boomerang;
sfxSound		sfx_bobombsound;
sfxSound		sfx_bulletbillsound;
sfxSound		sfx_flamecannon;
sfxSound		sfx_hit;
sfxSound		sfx_kicksound;
sfxSound		sfx_mip;
sfxSound		sfx_transform;

gfxSprite		**spr_player[4];
CGameMode		*gamemodes[GAMEMODE_LAST];

extern FiltersList *filterslist;
extern MapList* maplist;
extern GraphicsList *menugraphicspacklist;
extern GraphicsList *gamegraphicspacklist;

extern CMap* g_map;
extern CTilesetManager* g_tilesetmanager;

extern CResourceManager* rm;
extern CGame* smw;
extern std::string RootDataDirectory;

gfxSprite		spr_hazard_fireball[3];
gfxSprite		spr_hazard_rotodisc[3];
gfxSprite		spr_hazard_bulletbill[3];
gfxSprite		spr_hazard_flame[3];
gfxSprite		spr_hazard_pirhanaplant[3];
gfxSprite		spr_overlay, spr_overlayhole;

void DECLSPEC soundfinished(int channel){}
void DECLSPEC musicfinished(){}

CEyecandyContainer eyecandy[3];
gfxSprite		spr_frontmap[2];

CPlayer			*list_players[4];
short			list_players_cnt = 0;


short LookupTeamID(short id)
{
    return 0;
}
gfxSprite		spr_scoretext;
gfxSprite		spr_poof;

IO_MovingObject * createpowerup(short iType, short ix, short iy, bool side, bool spawn)
{
    return NULL;
}
void CScore::AdjustScore(short iValue) {}
///////

CObjectContainer noncolcontainer;
CObjectContainer objectcontainer[3];

int save_as();
int find();
int clear_world();
bool dialog(const char * title, const char * instructions, char * input, int inputsize);
int display_help();

void drawmap(bool fScreenshot, short iBlockSize);

bool UpdateForeground(short iCol, short iRow);
void UpdatePath(short iCol, short iRow);
void AutoSetPath(short iCol, short iRow);
bool UpdateCoastline(short iCol, short iRow);
bool AutoSetTile(short iCol, short iRow);
void UpdatePathSprite(short iCol, short iRow);
void AutoSetPathSprite(short iCol, short iRow);
short AdjustForeground(short iSprite, short iCol, short iRow);

void GetForegroundTileValues(short iCol, short iRow, short iOldTiles[9]);
bool ForegroundTileValuesChanged(short iCol, short iRow, short iOldTiles[9]);

void ReadVehiclesIntoEditor();
void WriteVehiclesIntoWorld();
void AddVehicleToTile(short iCol, short iRow, short iType);
void RemoveVehicleFromTile(short iCol, short iRow);

void ReadWarpsIntoEditor();
void WriteWarpsIntoWorld();
void AddWarpToTile(short iCol, short iRow, short iType);
void RemoveWarpFromTile(short iCol, short iRow);

extern WorldMap g_worldmap;
extern WorldList *worldlist;
void loadcurrentworld();
int savecurrentworld();
int findcurrentstring();
int new_world();
int resize_world();

int editor_edit();
int editor_warp();
int editor_boundary();
int editor_water();
int	editor_background();
int editor_stageforeground();
int editor_structureforeground();
int editor_bridges();
int editor_vehicles();
int	editor_path();
int editor_pathsprite();
int editor_type();
int editor_stage();
int editor_start_items();

void DisplayStageDetails(bool fForce, short iStageId, short iMouseX, short iMouseY);

void updateworldsurface();
void takescreenshot();

bool ignoreclick = false;

char findstring[FILEBUFSIZE] = "";

extern const char * g_szWorldMusicCategoryNames[MAXWORLDMUSICCATEGORY];
short g_musiccategorydisplaytimer = 0;

short g_messagedisplaytimer = 0;
std::string g_szMessageTitle = "";
std::string g_szMessageLine[3];
void SetDisplayMessage(short iTime,
                       const char *szTitle,
                       const char *szLine1,
                       const char *szLine2,
                       const char *szLine3);
void DrawMessage();

//Menu keys to use for menus
extern SDL_KEYTYPE controlkeys[2][2][4][NUM_KEYS];

//Vehicle stuff
std::vector<WorldVehicle*> vehiclelist;

//Warp stuff
std::vector<WorldWarp*> warplist;

bool g_fFullScreen = false;
bool g_fShowStagePreviews = true;

//Stage Mode Menu
UI_Menu * mCurrentMenu;
UI_Menu mStageSettingsMenu;
UI_Menu mBonusItemPicker;

MI_ImageSelectField * miModeField;
MI_SelectField * miGoalField[GAMEMODE_LAST];
MI_Button * miModeSettingsButton;
MI_Button * miBonusItemsButton;
MI_SelectField * miSpecialGoalField[3];

MI_SelectField * miFinalStageField;
MI_SelectField * miPointsField;

MI_TextField * miNameField;
MI_SelectField * miBonusType;
MI_TextField * miBonusTextField[5];

MI_MapField * miMapField;

MI_Button * miDeleteStageButton;

MI_Image * miDeleteStageDialogImage;
MI_Text * miDeleteStageDialogAreYouText;
MI_Text * miDeleteStageDialogSureText;
MI_Button * miDeleteStageDialogYesButton;
MI_Button * miDeleteStageDialogNoButton;

//Vehicle Creation Menu
UI_Menu mVehicleMenu;
MI_ImageSelectField * miVehicleSpriteField;
MI_ImageSelectField * miVehicleStageField;
MI_SelectField * miVehicleMinMovesField;
MI_SelectField * miVehicleMaxMovesField;
MI_SelectField * miVehiclePacesField;
MI_SelectField * miVehicleDirectionField;
MI_SelectField * miVehicleBoundaryField;
MI_Button * miVehicleCreateButton;
MI_Text * miTitleText;

UI_ModeOptionsMenu* mModeOptionsMenu;

gfxSprite menu_dialog;

SDL_Surface * sMapThumbnail = NULL;
short iOldStageId = -1;

//Sets up default mode options
extern void LoadMapHazards(bool fPreview);

struct StageModeOption {
	char szName[64];
	short iValue;
};

struct StageMode {
	char szName[128];
	char szGoal[64];

	short iDefaultGoal;

	StageModeOption options[GAMEMODE_NUM_OPTIONS - 1];
};

StageMode stagemodes[GAMEMODE_LAST];

void SetStageMode(short iIndex, const char * szModeName, const char * szGoalName, short iIncrement, short iDefault)
{
	if (iIndex < 0 || iIndex >= GAMEMODE_LAST)
		return;

	StageMode * sm = &stagemodes[iIndex];

	strncpy(sm->szName, szModeName, 128);
	sm->szName[127] = 0;

	strncpy(sm->szGoal, szGoalName, 64);
	sm->szGoal[63] = 0;

	sm->iDefaultGoal = iDefault;

    for (short iModeOption = 0; iModeOption < GAMEMODE_NUM_OPTIONS - 1; iModeOption++) {
		sm->options[iModeOption].iValue = (iModeOption + 1) * iIncrement;
		sprintf(sm->options[iModeOption].szName, "%d", sm->options[iModeOption].iValue);
	}
}

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
    menugraphicspacklist = new GraphicsList;
    gamegraphicspacklist = new GraphicsList;
    worldlist = new WorldList;

	game_values.sound = false;
	game_values.music = false;

    /* This must occur before any data files are loaded */
    Initialize_Paths();

	bool done;

	printf("-------------------------------------------------------------------------------\n");
	printf(" %s\n", MAPTITLESTRING);
	printf("-------------------------------------------------------------------------------\n");
	printf("\n---------------- startup ----------------\n");

    int saved_col = 0, saved_row = 0;
    {
        BinaryFile editor_settings("worldeditor.bin", "rb");
        if (editor_settings.is_open()) {
            saved_col = editor_settings.read_i32();
            saved_row = editor_settings.read_i32();
            g_fFullScreen = editor_settings.read_bool();
            editor_settings.read_string_long(findstring, FILEBUFSIZE);
        }
    }

	gfx_init(640,480, g_fFullScreen);
	blitdest = screen;
	g_tilesetmanager->Init(convertPath("gfx/Classic/tilesets").c_str());

	SDL_WM_SetCaption(MAPTITLESTRING, "worldeditor.ico");

	game_values.toplayer = true;

	printf("\n---------------- loading graphics ----------------\n");

	spr_warps[0].init(convertPath("gfx/leveleditor/leveleditor_warp.png"), 255, 0, 255);
	spr_warps[1].init(convertPath("gfx/leveleditor/leveleditor_warp_preview.png"), 255, 0, 255);
	spr_warps[2].init(convertPath("gfx/leveleditor/leveleditor_warp_thumbnail.png"), 255, 0, 255);

	spr_path.init(convertPath("gfx/leveleditor/leveleditor_world_path.png"), 255, 0, 255);

	spr_selectedtile.init(convertPath("gfx/leveleditor/leveleditor_selectedtile.png"), 0, 0, 0, 128, true);

	spr_dialog.init(convertPath("gfx/leveleditor/leveleditor_dialog.png"), 255, 0, 255, 255, true);
	menu_shade.init(convertPath("gfx/leveleditor/leveleditor_shade.png"), 255, 0, 255, 128, true);
	spr_largedialog.init(convertPath("gfx/leveleditor/leveleditor_platform.png"), 255, 0, 255, 255, true);

	menu_font_small.init(convertPath("gfx/packs/Classic/fonts/font_small.png"));
	menu_font_large.init(convertPath("gfx/packs/Classic/fonts/font_large.png"));
    rm->menu_font_small.init(convertPath("gfx/packs/Classic/fonts/font_small.png"));
    rm->menu_font_large.init(convertPath("gfx/packs/Classic/fonts/font_large.png"));

	spr_platformpath.init(convertPath("gfx/leveleditor/leveleditor_platform_path.png"), 255, 0, 255, 128, true);

	spr_platformstarttile.init(convertPath("gfx/leveleditor/leveleditor_platformstarttile.png"), 0, 0, 0, 64, true);
	spr_platformendtile.init(convertPath("gfx/leveleditor/leveleditor_selectedtile.png"), 0, 0, 0, 64, true);
	spr_platformstarttile.SetWrap(true);
	spr_platformendtile.SetWrap(true);


	printf("\n---------------- load world ----------------\n");

	spr_worldbackground[0].init(convertPath("gfx/packs/Classic/world/world_background.png"), 255, 0, 255, false);
	spr_worldbackground[1].init(convertPath("gfx/packs/Classic/world/preview/world_background.png"), 255, 0, 255);
	spr_worldbackground[2].init(convertPath("gfx/packs/Classic/world/thumbnail/world_background.png"), 255, 0, 255);

	spr_worldforeground[0].init(convertPath("gfx/packs/Classic/world/world_foreground.png"), 255, 0, 255, false);
	spr_worldforeground[1].init(convertPath("gfx/packs/Classic/world/preview/world_foreground.png"), 255, 0, 255);
	spr_worldforeground[2].init(convertPath("gfx/packs/Classic/world/thumbnail/world_foreground.png"), 255, 0, 255);

	spr_worldforegroundspecial[0].init(convertPath("gfx/packs/Classic/world/world_foreground_special.png"), 255, 0, 255, false);
	spr_worldforegroundspecial[1].init(convertPath("gfx/packs/Classic/world/preview/world_foreground_special.png"), 255, 0, 255);
	spr_worldforegroundspecial[2].init(convertPath("gfx/packs/Classic/world/thumbnail/world_foreground_special.png"), 255, 0, 255);

	spr_worldpaths[0].init(convertPath("gfx/packs/Classic/world/world_paths.png"), 255, 0, 255, false);
	spr_worldpaths[1].init(convertPath("gfx/packs/Classic/world/preview/world_paths.png"), 255, 0, 255);
	spr_worldpaths[2].init(convertPath("gfx/packs/Classic/world/thumbnail/world_paths.png"), 255, 0, 255);

	spr_worldvehicle[0].init(convertPath("gfx/packs/Classic/world/world_vehicles.png"), 255, 0, 255);
	spr_worldvehicle[1].init(convertPath("gfx/packs/Classic/world/preview/world_vehicles.png"), 255, 0, 255);
	spr_worldvehicle[2].init(convertPath("gfx/packs/Classic/world/thumbnail/world_vehicles.png"), 255, 0, 255);

	spr_worlditems.init(convertPath("gfx/packs/Classic/world/world_powerups.png"), 255, 0, 255);
	spr_worldpopup.init(convertPath("gfx/packs/Classic/world/world_item_popup.png"), 255, 0, 255);

	spr_storedpowerupsmall.init(convertPath("gfx/packs/Classic/powerups/small.png"), 255, 0, 255);
	spr_worlditemssmall.init(convertPath("gfx/packs/Classic/world/world_powerupssmall.png"), 255, 0, 255);
	spr_worlditemsplace.init(convertPath("gfx/packs/Classic/world/world_bonusplace.png"), 255, 0, 255);

	menu_dialog.init(convertPath("gfx/packs/Classic/menu/menu_dialog.png"), 255, 0, 255);

	//Mode Options Menu Gfx
	menu_egg.init(convertPath("gfx/packs/Classic/modeobjects/menu_egg.png"), 255, 0, 255);
	menu_stomp.init(convertPath("gfx/packs/Classic/modeobjects/menu_stomp.png"), 255, 0, 255);
	menu_survival.init(convertPath("gfx/packs/Classic/modeobjects/menu_survival.png"), 255, 0, 255);
	spr_phanto.init(convertPath("gfx/packs/Classic/modeobjects/phanto.png"), 255, 0, 255);
	menu_plain_field.init(convertPath("gfx/leveleditor/menu_plain_field.png"), 255, 0, 255);
	menu_slider_bar.init(convertPath("gfx/packs/Classic/menu/menu_slider_bar.png"), 255, 0, 255);
	spr_selectfield.init(convertPath("gfx/leveleditor/menu_selectfield.png"), 255, 0, 255);
	menu_verticalarrows.init(convertPath("gfx/packs/Classic/menu/menu_vertical_arrows.png"), 255, 0, 255);
	spr_storedpoweruplarge.init(convertPath("gfx/packs/Classic/powerups/large.png"), 255, 0, 255);

	menu_mode_small.init(convertPath("gfx/packs/Classic/menu/menu_mode_small.png"), 255, 0, 255);
	menu_mode_large.init(convertPath("gfx/packs/Classic/menu/menu_mode_large.png"), 255, 0, 255);

	spr_vehicleicons.init(convertPath("gfx/leveleditor/vehicle_icons.png"), 255, 0, 255);

	spr_thumbnail_warps[0].init(convertPath("gfx/packs/Classic/menu/menu_warp_preview.png"), 255, 0, 255);
	spr_thumbnail_warps[1].init(convertPath("gfx/packs/Classic/menu/menu_warp_thumbnail.png"), 255, 0, 255);

	spr_thumbnail_mapitems[0].init(convertPath("gfx/packs/Classic/menu/menu_mapitems_preview.png"), 255, 0, 255);
	spr_thumbnail_mapitems[1].init(convertPath("gfx/packs/Classic/menu/menu_mapitems_thumbnail.png"), 255, 0, 255);

	spr_tileanimation[1].init(convertPath("gfx/packs/Classic/tilesets/tile_animation_preview.png"), 255, 0, 255);
	spr_tileanimation[2].init(convertPath("gfx/packs/Classic/tilesets/tile_animation_thumbnail.png"), 255, 0, 255);

	spr_blocks[1].init(convertPath("gfx/packs/Classic/tilesets/blocks_preview.png"), 255, 0, 255);
	spr_blocks[2].init(convertPath("gfx/packs/Classic/tilesets/blocks_thumbnail.png"), 255, 0, 255);

	spr_unknowntile[1].init(convertPath("gfx/packs/Classic/tilesets/unknown_tile_preview.png"), 255, 0, 255);
	spr_unknowntile[2].init(convertPath("gfx/packs/Classic/tilesets/unknown_tile_thumbnail.png"), 255, 0, 255);

	spr_hazard_fireball[1].init(convertPath("gfx/packs/Classic/hazards/fireball_preview.png"), 255, 0, 255);
	spr_hazard_fireball[2].init(convertPath("gfx/packs/Classic/hazards/fireball_thumbnail.png"), 255, 0, 255);

	spr_hazard_rotodisc[1].init(convertPath("gfx/packs/Classic/hazards/rotodisc_preview.png"), 255, 0, 255);
	spr_hazard_rotodisc[2].init(convertPath("gfx/packs/Classic/hazards/rotodisc_thumbnail.png"), 255, 0, 255);

	spr_hazard_bulletbill[1].init(convertPath("gfx/packs/Classic/hazards/bulletbill_preview.png"), 255, 0, 255);
	spr_hazard_bulletbill[2].init(convertPath("gfx/packs/Classic/hazards/bulletbill_thumbnail.png"), 255, 0, 255);

	spr_hazard_flame[1].init(convertPath("gfx/packs/Classic/hazards/flame_preview.png"), 255, 0, 255);
	spr_hazard_flame[2].init(convertPath("gfx/packs/Classic/hazards/flame_thumbnail.png"), 255, 0, 255);

	spr_hazard_pirhanaplant[1].init(convertPath("gfx/packs/Classic/hazards/pirhanaplant_preview.png"), 255, 0, 255);
	spr_hazard_pirhanaplant[2].init(convertPath("gfx/packs/Classic/hazards/pirhanaplant_thumbnail.png"), 255, 0, 255);


	sMapSurface = SDL_CreateRGBSurface(screen->flags, 768, 608, screen->format->BitsPerPixel, 0, 0, 0, 0);

    worldlist->find(findstring);
    game_values.worldindex = worldlist->GetCurrentIndex();
	loadcurrentworld();

    if (saved_row >= 0 && saved_row <= iWorldHeight - 15 && saved_col >= 0 && saved_col <= iWorldWidth - 20) {
		draw_offset_row = saved_row;
		draw_offset_col = saved_col;
		updateworldsurface();
	}

	//Setup input for menus
	game_values.inputConfiguration[0][0].iDevice = DEVICE_KEYBOARD;
    for (short iInputState = 0; iInputState < 2; iInputState++) { //for game/menu
        for (short iKey = 0; iKey < NUM_KEYS; iKey++) {
			game_values.inputConfiguration[0][0].inputGameControls[iInputState].keys[iKey] = controlkeys[0][iInputState][0][iKey];
		}
	}

	game_values.playerInput.inputControls[0] = &game_values.inputConfiguration[0][0];

	SetStageMode(0, "Classic", "Lives", 5, 10);
	SetStageMode(1, "Frag", "Kills", 5, 20);
	SetStageMode(2, "Time Limit", "Time", 30, 60);
	SetStageMode(3, "Jail", "Kills", 5, 20);
	SetStageMode(4, "Coin Collection", "Coins", 5, 20);
	SetStageMode(5, "Stomp", "Kills", 10, 10);
	SetStageMode(6, "Yoshi's Eggs", "Eggs", 5, 20);
	SetStageMode(7, "Capture The Flag", "Points", 5, 20);
	SetStageMode(8, "Chicken", "Points", 50, 200);
	SetStageMode(9, "Tag", "Points", 50, 200);
	SetStageMode(10, "Star", "Lives", 1, 5);
	SetStageMode(11, "Domination", "Points", 50, 200);
	SetStageMode(12, "King of the Hill", "Points", 50, 200);
	SetStageMode(13, "Race", "Laps", 2, 10);
	SetStageMode(14, "Owned", "Points", 50, 200);
	SetStageMode(15, "Frenzy", "Kills", 5, 20);
	SetStageMode(16, "Survival", "Lives", 5, 10);
	SetStageMode(17, "Greed", "Coins", 10, 40);
	SetStageMode(18, "Health", "Lives", 1, 5);
	SetStageMode(19, "Card Collection", "Points", 10, 30);
	SetStageMode(20, "Phanto Chase", "Points", 50, 200);
	SetStageMode(21, "Shy Guy Tag", "Points", 50, 200);

	//Setup The Mode Menu
	mCurrentMenu = &mStageSettingsMenu;

	//Name
	miNameField = new MI_TextField(&spr_selectfield, 70, 20, "Name", 500, 120);
	miNameField->SetDisallowedChars(",");

	miModeField = new MI_ImageSelectField(&spr_selectfield, &menu_mode_small, 70, 60, "Mode", 500, 120, 16, 16);
	//miModeField->SetData(game_values.tourstops[0]->iMode, NULL, NULL);
	//miModeField->SetKey(0);
	miModeField->SetItemChangedCode(MENU_CODE_MODE_CHANGED);

    for (short iGameMode = 0; iGameMode < GAMEMODE_LAST; iGameMode++) {
		miModeField->Add(stagemodes[iGameMode].szName, iGameMode, "", false, false);

		miGoalField[iGameMode] = new MI_SelectField(&spr_selectfield, 70, 100, stagemodes[iGameMode].szGoal, 352, 120);
		miGoalField[iGameMode]->Show(iGameMode == 0);

        for (short iGameModeOption = 0; iGameModeOption < GAMEMODE_NUM_OPTIONS - 1; iGameModeOption++) {
			StageModeOption * option = &stagemodes[iGameMode].options[iGameModeOption];
			miGoalField[iGameMode]->Add(option->szName, option->iValue, "", false, false);
		}

		//miGoalField[iGameMode]->SetData(&gamemodes[iGameMode]->goal, NULL, NULL);
		//miGoalField[iGameMode]->SetKey(gamemodes[iGameMode]->goal);
	}

	miModeField->Add("Bonus House", 24, "", false, false);
	miModeField->Add("Pipe Minigame", 25, "", false, false);
	miModeField->Add("Boss Minigame", 26, "", false, false);
	miModeField->Add("Boxes Minigame", 27, "", false, false);

	//Create goal field for pipe game
	miSpecialGoalField[0] = new MI_SelectField(&spr_selectfield, 70, 100, "Points", 352, 120);
	miSpecialGoalField[0]->Show(false);

    for (short iGameModeOption = 0; iGameModeOption < GAMEMODE_NUM_OPTIONS - 1; iGameModeOption++) {
		short iValue = 10 + iGameModeOption * 10;
		char szName[16];
		sprintf(szName, "%d", iValue);
		miSpecialGoalField[0]->Add(szName, iValue, "", false, false);
	}

	//Create goal field for boss game
	miSpecialGoalField[1] = new MI_SelectField(&spr_selectfield, 70, 100, "Lives", 352, 120);
	miSpecialGoalField[1]->Show(false);

    for (short iGameLives = 1; iGameLives <= 30; iGameLives++) {
		char szName[16];
		sprintf(szName, "%d", iGameLives);
		miSpecialGoalField[1]->Add(szName, iGameLives, "", false, false);
	}

	//Create goal field for boxes game
	miSpecialGoalField[2] = new MI_SelectField(&spr_selectfield, 70, 100, "Lives", 352, 120);
	miSpecialGoalField[2]->Show(false);

    for (short iGameLives = 1; iGameLives <= 30; iGameLives++) {
		char szName[16];
		sprintf(szName, "%d", iGameLives);
		miSpecialGoalField[2]->Add(szName, iGameLives, "", false, false);
	}

	//Mode Settings Button
	miModeSettingsButton = new MI_Button(&spr_selectfield, 430, 100, "Settings", 140, 0);
	miModeSettingsButton->SetCode(MENU_CODE_TO_MODE_SETTINGS_MENU);

	//Points Field
	miPointsField = new MI_SelectField(&spr_selectfield, 70, 140, "Points", 245, 120);
    for (short iPoints = 0; iPoints <= 20; iPoints++) {
		char szPoints[8];
		sprintf(szPoints, "%d", iPoints);
		miPointsField->Add(szPoints, iPoints, "", false, false);
	}

	//Final Stage Field
	miFinalStageField = new MI_SelectField(&spr_selectfield, 325, 140, "End Stage", 245, 120);
	miFinalStageField->Add("No", 0, "", false, false);
	miFinalStageField->Add("Yes", 1, "", true, false);
	miFinalStageField->SetAutoAdvance(true);

	//Map Select Field
	miMapField = new MI_MapField(&spr_selectfield, 70, 180, "Map", 500, 120, true);

	//Bonus Item Picker Menu Button
	miBonusItemsButton= new MI_Button(&spr_selectfield, 430, 220, "Bonuses", 140, 0);
	miBonusItemsButton->SetCode(MENU_CODE_TO_BONUS_PICKER_MENU);

	//Bonus Type
	miBonusType = new MI_SelectField(&spr_selectfield, 70, 100, "Type", 500, 120);
	miBonusType->Add("Fixed", 0, "", false, false);
	miBonusType->Add("Random", 1, "", true, false);
	miBonusType->SetAutoAdvance(true);

	//Bonus House Text * 5
	miBonusTextField[0] = new MI_TextField(&spr_selectfield, 70, 140, "Text", 500, 120);
	miBonusTextField[1] = new MI_TextField(&spr_selectfield, 70, 180, "Text", 500, 120);
	miBonusTextField[2] = new MI_TextField(&spr_selectfield, 70, 220, "Text", 500, 120);
	miBonusTextField[3] = new MI_TextField(&spr_selectfield, 70, 260, "Text", 500, 120);
	miBonusTextField[4] = new MI_TextField(&spr_selectfield, 70, 300, "Text", 500, 120);

	miBonusTextField[0]->SetDisallowedChars(",|");
	miBonusTextField[1]->SetDisallowedChars(",|");
	miBonusTextField[2]->SetDisallowedChars(",|");
	miBonusTextField[3]->SetDisallowedChars(",|");
	miBonusTextField[4]->SetDisallowedChars(",|");

	//Delete Stage Button
	miDeleteStageButton = new MI_Button(&spr_selectfield, 430, 440, "Delete", 140, 0);
	miDeleteStageButton->SetCode(MENU_CODE_DELETE_STAGE_BUTTON);

	//Are You Sure Dialog for Delete Stage
	miDeleteStageDialogImage = new MI_Image(&spr_dialog, 224, 176, 0, 0, 192, 128, 1, 1, 0);
	miDeleteStageDialogAreYouText = new MI_Text("Are You", 320, 195, 0, 2, 1);
	miDeleteStageDialogSureText = new MI_Text("Sure?", 320, 220, 0, 2, 1);
	miDeleteStageDialogYesButton = new MI_Button(&spr_selectfield, 235, 250, "Yes", 80, 1);
	miDeleteStageDialogNoButton = new MI_Button(&spr_selectfield, 325, 250, "No", 80, 1);

	miDeleteStageDialogYesButton->SetCode(MENU_CODE_DELETE_STAGE_YES);
	miDeleteStageDialogNoButton->SetCode(MENU_CODE_DELETE_STAGE_NO);

	miDeleteStageDialogImage->Show(false);
	miDeleteStageDialogAreYouText->Show(false);
	miDeleteStageDialogSureText->Show(false);
	miDeleteStageDialogYesButton->Show(false);
	miDeleteStageDialogNoButton->Show(false);

	//Add Name Field
	mStageSettingsMenu.AddControl(miNameField, miDeleteStageButton, miModeField, NULL, NULL);

	//Add Mode Field
	mStageSettingsMenu.AddControl(miModeField, miNameField, miGoalField[0], NULL, NULL);

	//Add Mode Goal Fields
	mStageSettingsMenu.AddControl(miGoalField[0], miModeField, miGoalField[1], NULL, miModeSettingsButton);

	for (short iGoalField = 1; iGoalField < GAMEMODE_LAST - 1; iGoalField++)
		mStageSettingsMenu.AddControl(miGoalField[iGoalField], miGoalField[iGoalField - 1], miGoalField[iGoalField + 1], miGoalField[iGoalField - 1], miModeSettingsButton);

	mStageSettingsMenu.AddControl(miGoalField[GAMEMODE_LAST - 1], miGoalField[GAMEMODE_LAST - 2], miSpecialGoalField[0], miGoalField[GAMEMODE_LAST - 2], miModeSettingsButton);

	mStageSettingsMenu.AddControl(miSpecialGoalField[0], miGoalField[GAMEMODE_LAST - 1], miSpecialGoalField[1], miGoalField[GAMEMODE_LAST - 1], miSpecialGoalField[1]);
	mStageSettingsMenu.AddControl(miSpecialGoalField[1], miSpecialGoalField[0], miSpecialGoalField[2], miSpecialGoalField[0], miSpecialGoalField[2]);
	mStageSettingsMenu.AddControl(miSpecialGoalField[2], miSpecialGoalField[1], miPointsField, miSpecialGoalField[1], miModeSettingsButton);

	//Add Mode Settings Button
	mStageSettingsMenu.AddControl(miModeSettingsButton, miModeField, miFinalStageField, miSpecialGoalField[2], NULL);

	//Add Points Field
	mStageSettingsMenu.AddControl(miPointsField, miSpecialGoalField[2], miMapField, NULL, miFinalStageField);

	//Add Final Stage Field
	mStageSettingsMenu.AddControl(miFinalStageField, miSpecialGoalField[2], miMapField, miPointsField, NULL);

	//Add Map Field
	mStageSettingsMenu.AddControl(miMapField, miFinalStageField, miBonusType, NULL, miBonusItemsButton);

	//Add Bonus House Fields
	mStageSettingsMenu.AddControl(miBonusType, miMapField, miBonusTextField[0], NULL, NULL);
	mStageSettingsMenu.AddControl(miBonusTextField[0], miBonusType, miBonusTextField[1], NULL, NULL);
	mStageSettingsMenu.AddControl(miBonusTextField[1], miBonusTextField[0], miBonusTextField[2], NULL, NULL);
	mStageSettingsMenu.AddControl(miBonusTextField[2], miBonusTextField[1], miBonusTextField[3], NULL, NULL);
	mStageSettingsMenu.AddControl(miBonusTextField[3], miBonusTextField[2], miBonusTextField[4], NULL, NULL);
	mStageSettingsMenu.AddControl(miBonusTextField[4], miBonusTextField[3], miBonusItemsButton, NULL, NULL);

	//Add Bonus Button
	mStageSettingsMenu.AddControl(miBonusItemsButton, miBonusTextField[4], miDeleteStageButton, miMapField, NULL);

	//Add Delete Stage Button
	mStageSettingsMenu.AddControl(miDeleteStageButton, miBonusItemsButton, miNameField, miMapField, NULL);

	//Add Are You Sure Dialog
	mStageSettingsMenu.AddNonControl(miDeleteStageDialogImage);
	mStageSettingsMenu.AddNonControl(miDeleteStageDialogAreYouText);
	mStageSettingsMenu.AddNonControl(miDeleteStageDialogSureText);

	mStageSettingsMenu.AddControl(miDeleteStageDialogYesButton, NULL, NULL, NULL, miDeleteStageDialogNoButton);
	mStageSettingsMenu.AddControl(miDeleteStageDialogNoButton, NULL, NULL, miDeleteStageDialogYesButton, NULL);

	mStageSettingsMenu.SetHeadControl(miNameField);
	mStageSettingsMenu.SetCancelCode(MENU_CODE_EXIT_APPLICATION);

	mBonusItemPicker.SetCancelCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);

	mModeOptionsMenu = new UI_ModeOptionsMenu();

	g_wvVehicleStamp.iDrawSprite = 0;
	g_wvVehicleStamp.iActionId = 0;
	g_wvVehicleStamp.iCurrentTileX = 0;
	g_wvVehicleStamp.iCurrentTileY = 0;
	g_wvVehicleStamp.iMinMoves = 5;
	g_wvVehicleStamp.iMaxMoves = 8;
	g_wvVehicleStamp.fSpritePaces = true;
	g_wvVehicleStamp.iDrawDirection = 0;
	g_wvVehicleStamp.iBoundary = 0;

	//Create Vehicle Menu
	miVehicleSpriteField = new MI_ImageSelectField(&spr_selectfield, &spr_vehicleicons, 70, 80, "Sprite", 500, 150, 16, 16);
	miVehicleSpriteField->Add("Hammer Brother", 0, "", false, false);
	miVehicleSpriteField->Add("Boomerang Brother", 1, "", false, false);
	miVehicleSpriteField->Add("Fire Brother", 2, "", false, false);
	miVehicleSpriteField->Add("Tank 1", 3, "", false, false);
	miVehicleSpriteField->Add("Boat 1", 4, "", false, false);
	miVehicleSpriteField->Add("Boat 2", 5, "", false, false);
	miVehicleSpriteField->Add("Airship 1", 6, "", false, false);
	miVehicleSpriteField->Add("Airship 2", 7, "", false, false);
	miVehicleSpriteField->Add("Tank 2", 8, "", false, false);
	miVehicleSpriteField->SetData(&g_wvVehicleStamp.iDrawSprite, NULL, NULL);
	miVehicleSpriteField->SetKey(g_wvVehicleStamp.iDrawSprite);

	miVehicleStageField = new MI_ImageSelectField(&spr_selectfield, &menu_mode_small, 70, 120, "Stage", 500, 150, 16, 16);
	miVehicleStageField->SetData(&g_wvVehicleStamp.iActionId, NULL, NULL);

	miVehicleMinMovesField = new MI_SelectField(&spr_selectfield, 70, 160, "Min Moves", 500, 150);

    for (short iMinMoves = 0; iMinMoves <= 100; iMinMoves++) {
		char szMinMoves[8];
		sprintf(szMinMoves, "%d", iMinMoves);
		miVehicleMinMovesField->Add(szMinMoves, iMinMoves, "", false, false);
	}

	miVehicleMinMovesField->SetData(&g_wvVehicleStamp.iMinMoves, NULL, NULL);
	miVehicleMinMovesField->SetKey(g_wvVehicleStamp.iMinMoves);
	miVehicleMinMovesField->SetItemChangedCode(MENU_CODE_VEHICLE_MIN_MOVES_CHANGED);
	miVehicleMinMovesField->SetNoWrap(true);
	miVehicleMinMovesField->AllowFastScroll(true);

	miVehicleMaxMovesField = new MI_SelectField(&spr_selectfield, 70, 200, "Max Moves", 500, 150);

    for (short iMaxMoves = 0; iMaxMoves <= 100; iMaxMoves++) {
		char szMaxMoves[8];
		sprintf(szMaxMoves, "%d", iMaxMoves);
		miVehicleMaxMovesField->Add(szMaxMoves, iMaxMoves, "", false, false);
	}

	miVehicleMaxMovesField->SetData(&g_wvVehicleStamp.iMaxMoves, NULL, NULL);
	miVehicleMaxMovesField->SetKey(g_wvVehicleStamp.iMaxMoves);
	miVehicleMaxMovesField->SetItemChangedCode(MENU_CODE_VEHICLE_MAX_MOVES_CHANGED);
	miVehicleMaxMovesField->SetNoWrap(true);
	miVehicleMaxMovesField->AllowFastScroll(true);

	miVehiclePacesField = new MI_SelectField(&spr_selectfield, 70, 240, "Paces", 500, 150);
	miVehiclePacesField->Add("No", 0, "", false, false);
	miVehiclePacesField->Add("Yes", 1, "", true, false);
	miVehiclePacesField->SetData(NULL, NULL, &g_wvVehicleStamp.fSpritePaces);
	miVehiclePacesField->SetKey(g_wvVehicleStamp.fSpritePaces ? 1 : 0);
	miVehiclePacesField->SetAutoAdvance(true);

	miVehicleDirectionField = new MI_SelectField(&spr_selectfield, 70, 280, "Direction", 500, 150);
	miVehicleDirectionField->Add("Left", 0, "", false, false);
	miVehicleDirectionField->Add("Right", 1, "", true, false);
	miVehiclePacesField->SetData(&g_wvVehicleStamp.iDrawDirection, NULL, NULL);
	miVehiclePacesField->SetKey(g_wvVehicleStamp.iDrawDirection);
	miVehicleDirectionField->SetAutoAdvance(true);

	miVehicleBoundaryField = new MI_SelectField(&spr_selectfield, 70, 320, "Boundary", 500, 150);
	miVehicleBoundaryField->Add("No Boundary", 0, "", false, false);

    for (short iBoundary = 1; iBoundary <= 100; iBoundary++) {
		char szBoundary[8];
		sprintf(szBoundary, "%d", iBoundary);
		miVehicleBoundaryField->Add(szBoundary, iBoundary, "", false, false);
	}

	miVehicleBoundaryField->SetData(&g_wvVehicleStamp.iBoundary, NULL, NULL);
	miVehicleBoundaryField->SetKey(g_wvVehicleStamp.iBoundary);
	miVehicleBoundaryField->AllowFastScroll(true);

	miVehicleCreateButton = new MI_Button(&spr_selectfield, 430, 360, "OK", 140, 1);
	miVehicleCreateButton->SetCode(MENU_CODE_CREATE_VEHICLE);

	miTitleText = new MI_Text("Clicking on the map will add the vehicle configured below", 320, 50, 640, 2, 1);

	mVehicleMenu.AddNonControl(miTitleText);

	mVehicleMenu.AddControl(miVehicleSpriteField, miVehicleCreateButton, miVehicleStageField, NULL, NULL);
	mVehicleMenu.AddControl(miVehicleStageField, miVehicleSpriteField, miVehicleMinMovesField, NULL, NULL);
	mVehicleMenu.AddControl(miVehicleMinMovesField, miVehicleStageField, miVehicleMaxMovesField, NULL, NULL);
	mVehicleMenu.AddControl(miVehicleMaxMovesField, miVehicleMinMovesField, miVehiclePacesField, NULL, NULL);
	mVehicleMenu.AddControl(miVehiclePacesField, miVehicleMaxMovesField, miVehicleDirectionField, NULL, NULL);
	mVehicleMenu.AddControl(miVehicleDirectionField, miVehiclePacesField, miVehicleBoundaryField, NULL, NULL);
	mVehicleMenu.AddControl(miVehicleBoundaryField, miVehicleDirectionField, miVehicleCreateButton, NULL, NULL);
	mVehicleMenu.AddControl(miVehicleCreateButton, miVehicleBoundaryField, miVehicleSpriteField, NULL, NULL);

	mVehicleMenu.SetHeadControl(miVehicleSpriteField);
	mVehicleMenu.SetCancelCode(MENU_CODE_EXIT_APPLICATION);


	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

	printf("\n---------------- ready, steady, go! ----------------\n");

	printf("entering world editor loop...\n");
	done = false;
    while (!done) {
        switch (state) {
			case EDITOR_EDIT:
				state = editor_edit();
			break;

			case EDITOR_WATER:
				state = editor_water();
			break;

			case EDITOR_BACKGROUND:
				state = editor_background();
			break;

			case EDITOR_STAGEFOREGROUND:
				state = editor_stageforeground();
			break;

			case EDITOR_STRUCTUREFOREGROUND:
				state = editor_structureforeground();
			break;

			case EDITOR_BRIDGES:
				state = editor_bridges();
			break;

			case EDITOR_VEHICLES:
				state = editor_vehicles();
			break;

			case EDITOR_PATH:
				state = editor_path();
			break;

			case EDITOR_PATHSPRITE:
				state = editor_pathsprite();
			break;

			case EDITOR_WARP:
				state = editor_warp();
			break;

			case EDITOR_START_ITEMS:
				state = editor_start_items();
			break;

			case EDITOR_BOUNDARY:
				state = editor_boundary();
			break;

			case EDITOR_TYPE:
				state = editor_type();
			break;

			case EDITOR_STAGE:
				state = editor_stage();
			break;

			case EDITOR_QUIT:
				done = true;
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

			case CLEAR_WORLD:
				state = clear_world();
			break;

			case NEW_WORLD:
				state = new_world();
			break;

			case RESIZE_WORLD:
				state = resize_world();
			break;

			case SAVE:
				state = savecurrentworld();
			break;

			default:
				printf(" PANIC: WEIRD GAMESTATE: %d\n", state);
			break;
		}
	}

	SDL_FreeSurface(sMapSurface);

	printf("\n---------------- save world ----------------\n");

	WriteVehiclesIntoWorld();
	WriteWarpsIntoWorld();
	g_worldmap.Save(convertPath("worlds/ZZworldeditor.txt").c_str());

    {
        BinaryFile editor_settings("worldeditor.bin", "wb");
        if (editor_settings.is_open()) {
            editor_settings.write_i32(draw_offset_col);
            editor_settings.write_i32(draw_offset_row);
            editor_settings.write_bool(g_fFullScreen);
            editor_settings.write_string_long(worldlist->current_name());
        }
    }

	printf("\n---------------- shutdown ----------------\n");
	return 0;
}


int editor_edit()
{
	bool done = false;
	g_musiccategorydisplaytimer = 0;

	short view_repeat_direction = -1;
	short view_repeat_timer = 0;

	bool fExiting = false;
	bool fSelectedYes = false;

	short iStageDisplay = -1;

    while (!done) {
		int framestart = SDL_GetTicks();

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
                case SDL_QUIT: {
						done = true;
						break;
					}

                case SDL_KEYDOWN: {
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
							if (g_musiccategorydisplaytimer > 0)
								g_musiccategorydisplaytimer = 0;
                        else if (edit_mode != 0) {
								edit_mode = 0;
								set_tile = 0;
                        } else {
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

						if (event.key.keysym.sym == SDLK_INSERT)
							takescreenshot();

						if (event.key.keysym.sym == SDLK_1)
							return EDITOR_WATER;

						if (event.key.keysym.sym == SDLK_2)
							return EDITOR_BACKGROUND;

						if (event.key.keysym.sym == SDLK_3)
							return EDITOR_STAGEFOREGROUND;

						if (event.key.keysym.sym == SDLK_4)
							return EDITOR_PATHSPRITE;

						if (event.key.keysym.sym == SDLK_5)
							return EDITOR_STRUCTUREFOREGROUND;

						if (event.key.keysym.sym == SDLK_6)
							return EDITOR_BRIDGES;

						if (event.key.keysym.sym == SDLK_t)
							return EDITOR_TYPE;

						if (event.key.keysym.sym == SDLK_e)
							return EDITOR_STAGE;

						if (event.key.keysym.sym == SDLK_p)
							return EDITOR_PATH;

						if (event.key.keysym.sym == SDLK_v)
							return EDITOR_VEHICLES;

                    if (event.key.keysym.sym == SDLK_SPACE) {
							g_fShowStagePreviews = !g_fShowStagePreviews;
							if (g_fShowStagePreviews)
								SetDisplayMessage(60, "Stage Previews", "Preview popups", "have been", "enabled.");
							else
								SetDisplayMessage(60, "Stage Previews", "Preview popups", "have been", "disabled.");
						}

                    if (edit_mode == 5 && event.key.keysym.sym == SDLK_c) {
							int iMouseX, iMouseY;
							SDL_GetMouseState(&iMouseX, &iMouseY);

							short iButtonX = iMouseX - draw_offset_x;
							short iButtonY = iMouseY - draw_offset_y;
							short iCol = iButtonX / TILESIZE + draw_offset_col;
							short iRow = iButtonY / TILESIZE + draw_offset_row;

							std::vector<WorldVehicle*>::iterator itr = vehiclelist.begin(), lim = vehiclelist.end();
                        while (itr != lim) {
								WorldVehicle * vehicle = *itr;
                            if (vehicle->iCurrentTileX == iCol && vehicle->iCurrentTileY == iRow) {
									g_wvVehicleStamp.iDrawSprite = vehicle->iDrawSprite;
									g_wvVehicleStamp.iActionId = vehicle->iActionId;
									g_wvVehicleStamp.iMinMoves = vehicle->iMinMoves;
									g_wvVehicleStamp.iMaxMoves = vehicle->iMaxMoves;
									g_wvVehicleStamp.fSpritePaces = vehicle->fSpritePaces;
									g_wvVehicleStamp.iDrawDirection = vehicle->iDrawDirection;
									g_wvVehicleStamp.iBoundary = vehicle->iBoundary;

									miVehicleSpriteField->SetKey(g_wvVehicleStamp.iDrawSprite);
									miVehicleStageField->SetKey(g_wvVehicleStamp.iActionId);
									miVehicleMinMovesField->SetKey(g_wvVehicleStamp.iMinMoves);
									miVehicleMaxMovesField->SetKey(g_wvVehicleStamp.iMaxMoves);
									miVehiclePacesField->SetKey(g_wvVehicleStamp.fSpritePaces ? 1 : 0);
									miVehicleDirectionField->SetKey(g_wvVehicleStamp.iDrawDirection);
									miVehicleBoundaryField->SetKey(g_wvVehicleStamp.iBoundary);

									return EDITOR_VEHICLES;
								}

								itr++;
							}
						}

						if (event.key.keysym.sym == SDLK_w)
							return EDITOR_WARP;

						if (event.key.keysym.sym == SDLK_i)
							return EDITOR_START_ITEMS;

						if (event.key.keysym.sym == SDLK_b)
							return EDITOR_BOUNDARY;

						if (event.key.keysym.sym == SDLK_a)
							fAutoPaint = !fAutoPaint;

                    if (event.key.keysym.sym == SDLK_r) {
							if (g_musiccategorydisplaytimer > 0 && ++g_worldmap.iMusicCategory >= MAXWORLDMUSICCATEGORY)
								g_worldmap.iMusicCategory = 0;

							g_musiccategorydisplaytimer = 90;
						}

                    if (event.key.keysym.sym == SDLK_s) {
							if (keystate[SDLK_LSHIFT] || keystate[SDLK_RSHIFT])
								return SAVE_AS;

							return SAVE;
						}

                    if (event.key.keysym.sym == SDLK_f) {
							if (keystate[SDLK_LSHIFT] || keystate[SDLK_RSHIFT] || findstring[0] == '\0')
								return FIND;

							findcurrentstring();
						}

                    if (event.key.keysym.sym == SDLK_DELETE && (keystate[SDLK_LCTRL] || keystate[SDLK_RCTRL])) {
							return CLEAR_WORLD;
						}

						if (event.key.keysym.sym == SDLK_n)
							return NEW_WORLD;

						if (event.key.keysym.sym == SDLK_k)
							return RESIZE_WORLD;

						if (event.key.keysym.sym == SDLK_h || event.key.keysym.sym == SDLK_F1)
							return DISPLAY_HELP;

                    if (event.key.keysym.sym == SDLK_UP) {
                        if (draw_offset_row > 0) {
								draw_offset_row--;
								updateworldsurface();

								view_repeat_direction = 0;
								view_repeat_timer = 30;

								iStageDisplay = -1;
							}
                    } else if (event.key.keysym.sym == SDLK_DOWN) {
                        if (draw_offset_row < iWorldHeight - 15) {
								draw_offset_row++;
								updateworldsurface();

								view_repeat_direction = 1;
								view_repeat_timer = 30;

								iStageDisplay = -1;
							}
                    } else if (event.key.keysym.sym == SDLK_LEFT) {
                        if (draw_offset_col > 0) {
								draw_offset_col--;
								updateworldsurface();

								view_repeat_direction = 2;
								view_repeat_timer = 30;

								iStageDisplay = -1;
							}
                    } else if (event.key.keysym.sym == SDLK_RIGHT) {
                        if (draw_offset_col < iWorldWidth - 20) {
								draw_offset_col++;
								updateworldsurface();

								view_repeat_direction = 3;
								view_repeat_timer = 30;

								iStageDisplay = -1;
							}
						}

                    if (event.key.keysym.sym == SDLK_PAGEUP) {
							if (--game_values.worldindex < 0)
                            game_values.worldindex = worldlist->GetCount() - 1;

                        worldlist->prev();

							loadcurrentworld();

							iOldStageId = -1;
						}

                    if (event.key.keysym.sym == SDLK_PAGEDOWN) {
                        if (++game_values.worldindex >= worldlist->GetCount())
								game_values.worldindex = 0;

                        worldlist->next();

							loadcurrentworld();

							iOldStageId = -1;
						}

						break;
					}

                case SDL_KEYUP: {
						if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_DOWN ||
                            event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_RIGHT) {
							view_repeat_direction = -1;
							view_repeat_timer = 0;
						}

						break;
					}

                case SDL_MOUSEBUTTONDOWN: {
						short iButtonX = event.button.x - draw_offset_x;
						short iButtonY = event.button.y - draw_offset_y;
						short iCol = iButtonX / TILESIZE + draw_offset_col;
						short iRow = iButtonY / TILESIZE + draw_offset_row;

                    if (iButtonX >= 0 && iButtonY >= 0 && iButtonX < iWorldWidth * TILESIZE && iButtonY < iWorldHeight * TILESIZE) {
                        if (event.button.button == SDL_BUTTON_LEFT && !ignoreclick) {
                            if (edit_mode == 0) { //selected background
                                if (g_worldmap.tiles[iCol][iRow].iBackgroundSprite != set_tile || fAutoPaint) {
										bool fNeedUpdate = false;
                                    if (g_worldmap.tiles[iCol][iRow].iBackgroundSprite != set_tile) {
											g_worldmap.tiles[iCol][iRow].iBackgroundSprite = set_tile;
											fNeedUpdate = true;
										}

										fNeedUpdate |= UpdateForeground(iCol, iRow);

										if ((set_tile % WORLD_BACKGROUND_SPRITE_SET_SIZE) < 2 && fAutoPaint)
											fNeedUpdate |= UpdateCoastline(iCol, iRow);

										if (fNeedUpdate)
											updateworldsurface();
									}
                            } else if (edit_mode == 1) { //selected foreground
                                if (g_worldmap.tiles[iCol][iRow].iForegroundSprite != set_tile) {
										g_worldmap.tiles[iCol][iRow].iForegroundSprite = set_tile;
										updateworldsurface();

										if (set_tile >= WORLD_BRIDGE_SPRITE_OFFSET && set_tile <= WORLD_BRIDGE_SPRITE_OFFSET + 3)
											g_worldmap.tiles[iCol][iRow].iConnectionType = set_tile - WORLD_BRIDGE_SPRITE_OFFSET + 12;
									}
                            } else if (edit_mode == 2) { //selected connection
									g_worldmap.tiles[iCol][iRow].iConnectionType = set_tile;

									if (fAutoPaint)
										UpdatePath(iCol, iRow);
                            } else if (edit_mode == 3) { //selected type
									//start tiles
                                if (set_tile <= 1) {
                                    if (g_worldmap.tiles[iCol][iRow].iForegroundSprite != set_tile + WORLD_START_SPRITE_OFFSET) {
											g_worldmap.tiles[iCol][iRow].iType = 1;
											g_worldmap.tiles[iCol][iRow].iForegroundSprite = set_tile + WORLD_START_SPRITE_OFFSET;
											updateworldsurface();
										}
                                } else if (set_tile <= 5) { //doors
                                    if (g_worldmap.tiles[iCol][iRow].iType != set_tile) {
											//if the door was placed on a start tile
											if (g_worldmap.tiles[iCol][iRow].iType == 1)
												g_worldmap.tiles[iCol][iRow].iForegroundSprite = 0;

											g_worldmap.tiles[iCol][iRow].iType = set_tile;
											updateworldsurface();
										}
									}
                            } else if (edit_mode == 4) { //selected path sprite
									short iAdjustedTile = AdjustForeground(set_tile, iCol, iRow);
									bool fNeedUpdate = false;

                                if (!fAutoPaint && g_worldmap.tiles[iCol][iRow].iForegroundSprite != iAdjustedTile) {
										g_worldmap.tiles[iCol][iRow].iForegroundSprite = iAdjustedTile;
										fNeedUpdate = true;
									}

									//Detect if there was a change so we can repaint the screen
                                if (fAutoPaint) {
										short iOldTiles[9];
										GetForegroundTileValues(iCol, iRow, iOldTiles);
										g_worldmap.tiles[iCol][iRow].iForegroundSprite = iAdjustedTile;
										UpdatePathSprite(iCol, iRow);

										if (ForegroundTileValuesChanged(iCol, iRow, iOldTiles))
											fNeedUpdate = true;
									}

									if (fNeedUpdate)
										updateworldsurface();
                            } else if (edit_mode == 5) { //selected vehicle
									AddVehicleToTile(iCol, iRow, set_tile);
                            } else if (edit_mode == 6) { //selected warp
									AddWarpToTile(iCol, iRow, set_tile);
                            } else if (edit_mode == 7) { //water
                                if (g_worldmap.tiles[iCol][iRow].iBackgroundWater != set_tile) {
										g_worldmap.tiles[iCol][iRow].iBackgroundWater = set_tile;
										updateworldsurface();
									}
                            } else if (edit_mode == 8) { //boundary
									g_worldmap.tiles[iCol][iRow].iVehicleBoundary = set_tile;
                            } else if (edit_mode == 9) {
									//if the stage was placed on a start tile
                                if (g_worldmap.tiles[iCol][iRow].iType == 1) {
										g_worldmap.tiles[iCol][iRow].iForegroundSprite = 0;
										updateworldsurface();
									}

									g_worldmap.tiles[iCol][iRow].iType = set_tile;
								}
                        } else if (event.button.button == SDL_BUTTON_RIGHT) {
                            if (edit_mode == 0) {
                                if (g_worldmap.tiles[iCol][iRow].iBackgroundSprite != 0 || fAutoPaint) {
										bool fNeedUpdate = false;

                                    if (g_worldmap.tiles[iCol][iRow].iBackgroundSprite != 0) {
											g_worldmap.tiles[iCol][iRow].iBackgroundSprite = 0;
											fNeedUpdate = true;
										}

										fNeedUpdate |= UpdateForeground(iCol, iRow);

										if (fAutoPaint)
											fNeedUpdate |= UpdateCoastline(iCol, iRow);

										if (fNeedUpdate)
											updateworldsurface();
									}
                            } else if (edit_mode == 1) {
                                if (g_worldmap.tiles[iCol][iRow].iForegroundSprite != 0) {
										g_worldmap.tiles[iCol][iRow].iForegroundSprite = 0;
										updateworldsurface();
									}
                            } else if (edit_mode == 2) {
									g_worldmap.tiles[iCol][iRow].iConnectionType = 0;

									if (fAutoPaint)
										UpdatePath(iCol, iRow);
                            } else if (edit_mode == 3) { //selected start/door
                                if (g_worldmap.tiles[iCol][iRow].iType == 1) {
										g_worldmap.tiles[iCol][iRow].iForegroundSprite = 0;
										updateworldsurface();
                                } else if (g_worldmap.tiles[iCol][iRow].iType <= 5) {
										g_worldmap.tiles[iCol][iRow].iType = 0;
										updateworldsurface();
									}

									g_worldmap.tiles[iCol][iRow].iType = 0;
                            } else if (edit_mode == 4) {
									bool fNeedUpdate = false;

                                if (!fAutoPaint && g_worldmap.tiles[iCol][iRow].iForegroundSprite != 0) {
										g_worldmap.tiles[iCol][iRow].iForegroundSprite = 0;
										fNeedUpdate = true;
									}

									//Detect if there was a change so we can repaint the screen
                                if (fAutoPaint) {
										short iOldTiles[9];
										GetForegroundTileValues(iCol, iRow, iOldTiles);
										g_worldmap.tiles[iCol][iRow].iForegroundSprite = 0;
										UpdatePathSprite(iCol, iRow);

										if (ForegroundTileValuesChanged(iCol, iRow, iOldTiles))
											fNeedUpdate = true;
									}

									if (fNeedUpdate)
										updateworldsurface();
                            } else if (edit_mode == 5) {
									RemoveVehicleFromTile(iCol, iRow);
									iStageDisplay = -1;
                            } else if (edit_mode == 6) {
									RemoveWarpFromTile(iCol, iRow);
                            } else if (edit_mode == 7) { //water
                                if (g_worldmap.tiles[iCol][iRow].iBackgroundWater != 0) {
										g_worldmap.tiles[iCol][iRow].iBackgroundWater = 0;
										updateworldsurface();
									}
                            } else if (edit_mode == 8) { //boundary
									g_worldmap.tiles[iCol][iRow].iVehicleBoundary = 0;
                            } else if (edit_mode == 9) { //stage
									g_worldmap.tiles[iCol][iRow].iType = 0;
									iStageDisplay = -1;
								}
							}
						}

						break;
					}
					//Painting tiles with mouse movement
                case SDL_MOUSEMOTION: {
						short iButtonX = event.button.x - draw_offset_x;
						short iButtonY = event.button.y - draw_offset_y;
						short iCol = (iButtonX >> 5) + draw_offset_col;
						short iRow = (iButtonY >> 5) + draw_offset_row;

                    if (iButtonX >= 0 && iButtonY >= 0 && iButtonX < iWorldWidth * TILESIZE && iButtonY < iWorldHeight * TILESIZE) {
                        if (event.motion.state == SDL_BUTTON(SDL_BUTTON_LEFT) && !ignoreclick) {
                            if (edit_mode == 0) { //selected background
                                if (g_worldmap.tiles[iCol][iRow].iBackgroundSprite != set_tile || fAutoPaint) {
										bool fNeedUpdate = false;
                                    if (g_worldmap.tiles[iCol][iRow].iBackgroundSprite != set_tile) {
											g_worldmap.tiles[iCol][iRow].iBackgroundSprite = set_tile;
											fNeedUpdate = true;
										}

										fNeedUpdate |= UpdateForeground(iCol, iRow);

										if ((set_tile % WORLD_BACKGROUND_SPRITE_SET_SIZE) < 2 && fAutoPaint)
											fNeedUpdate |= UpdateCoastline(iCol, iRow);

										if (fNeedUpdate)
											updateworldsurface();
									}
                            } else if (edit_mode == 1) {
                                if (g_worldmap.tiles[iCol][iRow].iForegroundSprite != set_tile) {
										g_worldmap.tiles[iCol][iRow].iForegroundSprite = set_tile;
										updateworldsurface();
									}
                            } else if (edit_mode == 2) {
									g_worldmap.tiles[iCol][iRow].iConnectionType = set_tile;

									if (fAutoPaint)
										UpdatePath(iCol, iRow);
                            } else if (edit_mode == 3) { //selected stage/door
                                if (set_tile <= 1) {
                                    if (g_worldmap.tiles[iCol][iRow].iForegroundSprite != set_tile + WORLD_START_SPRITE_OFFSET) {
											g_worldmap.tiles[iCol][iRow].iType = 1;
											g_worldmap.tiles[iCol][iRow].iForegroundSprite = set_tile + WORLD_START_SPRITE_OFFSET;
											updateworldsurface();
										}
                                } else if (set_tile <= 5) {
                                    if (g_worldmap.tiles[iCol][iRow].iType != set_tile) {
											//if the door was placed on a start tile
											if (g_worldmap.tiles[iCol][iRow].iType == 1)
												g_worldmap.tiles[iCol][iRow].iForegroundSprite = 0;

											g_worldmap.tiles[iCol][iRow].iType = set_tile;
											updateworldsurface();
										}
									}
                            } else if (edit_mode == 4) {
									short iAdjustedTile = AdjustForeground(set_tile, iCol, iRow);
									bool fNeedUpdate = false;

                                if (!fAutoPaint && g_worldmap.tiles[iCol][iRow].iForegroundSprite != iAdjustedTile) {
										g_worldmap.tiles[iCol][iRow].iForegroundSprite = iAdjustedTile;
										fNeedUpdate = true;
									}

									//Detect if there was a change so we can repaint the screen
                                if (fAutoPaint) {
										short iOldTiles[9];
										GetForegroundTileValues(iCol, iRow, iOldTiles);
										g_worldmap.tiles[iCol][iRow].iForegroundSprite = iAdjustedTile;
										UpdatePathSprite(iCol, iRow);

										if (ForegroundTileValuesChanged(iCol, iRow, iOldTiles))
											fNeedUpdate = true;
									}

									if (fNeedUpdate)
										updateworldsurface();
                            } else if (edit_mode == 5) {
									AddVehicleToTile(iCol, iRow, set_tile);
                            } else if (edit_mode == 6) {
									AddWarpToTile(iCol, iRow, set_tile);
                            } else if (edit_mode == 7) { //water
                                if (g_worldmap.tiles[iCol][iRow].iBackgroundWater != set_tile) {
										g_worldmap.tiles[iCol][iRow].iBackgroundWater = set_tile;
										updateworldsurface();
									}
                            } else if (edit_mode == 9) { //stage
									//if the stage was placed on a start tile
                                if (g_worldmap.tiles[iCol][iRow].iType == 1) {
										g_worldmap.tiles[iCol][iRow].iForegroundSprite = 0;
										updateworldsurface();
									}

									g_worldmap.tiles[iCol][iRow].iType = set_tile;
								}
                        } else if (event.motion.state == SDL_BUTTON(SDL_BUTTON_RIGHT)) {
                            if (edit_mode == 0) { //selected background
                                if (g_worldmap.tiles[iCol][iRow].iBackgroundSprite != 0 || fAutoPaint) {
										bool fNeedUpdate = false;
                                    if (g_worldmap.tiles[iCol][iRow].iBackgroundSprite != 0) {
											g_worldmap.tiles[iCol][iRow].iBackgroundSprite = 0;
											fNeedUpdate = true;
										}

										fNeedUpdate |= UpdateForeground(iCol, iRow);

										if (fAutoPaint)
											fNeedUpdate |= UpdateCoastline(iCol, iRow);

										if (fNeedUpdate)
											updateworldsurface();
									}
                            } else if (edit_mode == 1) {
                                if (g_worldmap.tiles[iCol][iRow].iForegroundSprite != 0) {
										g_worldmap.tiles[iCol][iRow].iForegroundSprite = 0;
										updateworldsurface();
									}
                            } else if (edit_mode == 2) {
									g_worldmap.tiles[iCol][iRow].iConnectionType = 0;

									if (fAutoPaint)
										UpdatePath(iCol, iRow);
                            } else if (edit_mode == 3) {
                                if (g_worldmap.tiles[iCol][iRow].iType == 1) {
										g_worldmap.tiles[iCol][iRow].iForegroundSprite = 0;
										updateworldsurface();
                                } else if (g_worldmap.tiles[iCol][iRow].iType <= 5) {
										g_worldmap.tiles[iCol][iRow].iType = 0;
										updateworldsurface();
									}

									g_worldmap.tiles[iCol][iRow].iType = 0;
                            } else if (edit_mode == 4) {
									bool fNeedUpdate = false;

                                if (!fAutoPaint && g_worldmap.tiles[iCol][iRow].iForegroundSprite != 0) {
										g_worldmap.tiles[iCol][iRow].iForegroundSprite = 0;
										fNeedUpdate = true;
									}

									//Detect if there was a change so we can repaint the screen
                                if (fAutoPaint) {
										short iOldTiles[9];
										GetForegroundTileValues(iCol, iRow, iOldTiles);
										g_worldmap.tiles[iCol][iRow].iForegroundSprite = 0;
										UpdatePathSprite(iCol, iRow);

										if (ForegroundTileValuesChanged(iCol, iRow, iOldTiles))
											fNeedUpdate = true;
									}

									if (fNeedUpdate)
										updateworldsurface();
                            } else if (edit_mode == 5) { //vehicles
									RemoveVehicleFromTile(iCol, iRow);
									iStageDisplay = -1;
                            } else if (edit_mode == 6) { //Warps
									RemoveWarpFromTile(iCol, iRow);
                            } else if (edit_mode == 7) { //water
                                if (g_worldmap.tiles[iCol][iRow].iBackgroundWater != 0) {
										g_worldmap.tiles[iCol][iRow].iBackgroundWater = 0;
										updateworldsurface();
									}
                            } else if (edit_mode == 9) { //stage
									g_worldmap.tiles[iCol][iRow].iType = 0;
									iStageDisplay = -1;
								}
							}
						}

						//Scan to see if we are mousing over a stage
                    if (edit_mode == 5) {
							iStageDisplay = -1;

							std::vector<WorldVehicle*>::iterator itr = vehiclelist.begin(), lim = vehiclelist.end();
                        while (itr != lim) {
								WorldVehicle * vehicle = *itr;
                            if (vehicle->iCurrentTileX == iCol && vehicle->iCurrentTileY == iRow) {
									iStageDisplay = vehicle->iActionId;
									break;
								}

								itr++;
							}
                    } else if (edit_mode == 9) {
							iStageDisplay = -1;
                        if (iCol >= 0 && iRow >= 0 && iCol < iWorldWidth && iRow < iWorldHeight) {
								short iType = g_worldmap.tiles[iCol][iRow].iType - 6;
                            if (iType >= 0) {
									iStageDisplay = iType;
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

			//Allow auto-scrolling of world when the arrow keys are held down
            if (view_repeat_direction >= 0 && view_repeat_timer > 0) {
                if (--view_repeat_timer <= 0) {
					view_repeat_timer = 5;

                    if (view_repeat_direction == 0 && draw_offset_row > 0) {
						draw_offset_row--;
						updateworldsurface();
                    } else if (view_repeat_direction == 1 && draw_offset_row < iWorldHeight - 15) {
						draw_offset_row++;
						updateworldsurface();
                    } else if (view_repeat_direction == 2 && draw_offset_col > 0) {
						draw_offset_col--;
						updateworldsurface();
                    } else if (view_repeat_direction == 3 && draw_offset_col < iWorldWidth - 20) {
						draw_offset_col++;
						updateworldsurface();
					}
				}
			}
		}

		drawmap(false, TILESIZE);

		//Ask if you are sure you want to exit
        if (fExiting) {
			spr_dialog.draw(224, 176, 0, 0, 192, 128);
			menu_font_large.drawCentered(320, 195, "Exit");
			menu_font_large.drawCentered(320, 220, "Are You Sure?");
			menu_font_large.drawCentered(282, 254, "Yes");
			menu_font_large.drawCentered(356, 254, "No");

			spr_dialog.draw(fSelectedYes ? 250 : 326, 250, 192, 0, 64, 32);
        } else {
			//Draw Paths
            if (edit_mode == 2) {
                for (short iRow = draw_offset_row; iRow < draw_offset_row + 15 && iRow < iWorldHeight; iRow++) {
                    for (short iCol = draw_offset_col; iCol <= draw_offset_col + 20 && iCol < iWorldWidth; iCol++) {
						short iConnection = g_worldmap.tiles[iCol][iRow].iConnectionType;

						if (iConnection > 0)
							spr_path.draw((iCol - draw_offset_col) * TILESIZE + draw_offset_x, (iRow - draw_offset_row) * TILESIZE + draw_offset_y, (iConnection - 1) << 5, 0, TILESIZE, TILESIZE);
					}
				}
            } else if (edit_mode == 6) { //draw warps
				std::vector<WorldWarp*>::iterator itr = warplist.begin(), lim = warplist.end();
                while (itr != lim) {
					WorldWarp * warp = *itr;

					short ix, iy;

                    if (warp->iCol1 >= 0) {
						ix = (warp->iCol1 - draw_offset_col) * TILESIZE + draw_offset_x;
						iy = (warp->iRow1 - draw_offset_row) * TILESIZE + draw_offset_y;

						spr_warps[0].draw(ix, iy, warp->iID << 5, 0, 32, 32);
					}

                    if (warp->iCol2 >= 0) {
						ix = (warp->iCol2 - draw_offset_col) * TILESIZE + draw_offset_x;
						iy = (warp->iRow2 - draw_offset_row) * TILESIZE + draw_offset_y;

						spr_warps[0].draw(ix, iy, warp->iID << 5, 0, 32, 32);
					}

					itr++;
				}
            } else if (edit_mode == 8) { //draw boundaries
				int color = SDL_MapRGB(blitdest->format, 255, 0, 255);
                for (short iRow = draw_offset_row; iRow < draw_offset_row + 15 && iRow < iWorldHeight; iRow++) {
                    for (short iCol = draw_offset_col; iCol <= draw_offset_col + 20 && iCol < iWorldWidth; iCol++) {
						short iBoundary = g_worldmap.tiles[iCol][iRow].iVehicleBoundary - 1;

                        if (iBoundary >= 0) {
							short ix = (iCol - draw_offset_col) * TILESIZE + draw_offset_x;
							short iy = (iRow - draw_offset_row) * TILESIZE + draw_offset_y;
							SDL_Rect r = {ix, iy, 32, 32};
							SDL_FillRect(blitdest, &r, color);

							spr_worldforegroundspecial[0].draw(ix, iy, (iBoundary % 10) << 5, (iBoundary / 10) << 5, 32, 32);
						}
					}
				}
            } else if (edit_mode == 9) { //draw stages
				int color = SDL_MapRGB(blitdest->format, 0, 0, 255);
                for (short iRow = draw_offset_row; iRow < draw_offset_row + 15 && iRow < iWorldHeight; iRow++) {
                    for (short iCol = draw_offset_col; iCol <= draw_offset_col + 20 && iCol < iWorldWidth; iCol++) {
						short iType = g_worldmap.tiles[iCol][iRow].iType - 6;

                        if (iType >= 0) {
							short ix = (iCol - draw_offset_col) * TILESIZE + draw_offset_x;
							short iy = (iRow - draw_offset_row) * TILESIZE + draw_offset_y;
							SDL_Rect r = {ix, iy, 32, 32};
							SDL_FillRect(blitdest, &r, color);

							spr_worldforegroundspecial[0].draw(ix, iy, (iType % 10) << 5, (iType / 10) << 5, 32, 32);
						}
					}
				}

                if (iStageDisplay >= 0 && g_fShowStagePreviews) {
					int iMouseX, iMouseY;
					SDL_GetMouseState(&iMouseX, &iMouseY);

					DisplayStageDetails(false, iStageDisplay, iMouseX, iMouseY);
				}
			}

            if (edit_mode == 5 || edit_mode == 8) { //draw vehicles
				std::vector<WorldVehicle*>::iterator itr = vehiclelist.begin(), lim = vehiclelist.end();
				int color = SDL_MapRGB(blitdest->format, 0, 0, 128);
                while (itr != lim) {
					WorldVehicle * vehicle = *itr;

					short ix = (vehicle->iCurrentTileX  - draw_offset_col) * TILESIZE + draw_offset_x;
					short iy = (vehicle->iCurrentTileY - draw_offset_row) * TILESIZE + draw_offset_y;

					SDL_Rect r = {ix, iy, 32, 32};
					SDL_FillRect(blitdest, &r, color);

					spr_worldvehicle[0].draw(ix, iy, vehicle->iDrawDirection << 5, vehicle->iDrawSprite << 5, 32, 32);

                    if (edit_mode == 5) {
						spr_worldforegroundspecial[0].draw(ix, iy, (vehicle->iActionId % 10) << 5, (vehicle->iActionId / 10) << 5, 32, 32);
					}

                    if (edit_mode == 8) {
						short iBoundary = vehicle->iBoundary - 1;
						if (iBoundary == -1)
							spr_worldforegroundspecial[0].draw(ix, iy, 288, 288, 32, 32);
						else
							spr_worldforegroundspecial[0].draw(ix, iy, (iBoundary % 10) << 5, (iBoundary / 10) << 5, 32, 32);
					}

					itr++;
				}

                if (edit_mode == 5) {
                    if (iStageDisplay >= 0) {
						int iMouseX, iMouseY;
						SDL_GetMouseState(&iMouseX, &iMouseY);

						DisplayStageDetails(false, iStageDisplay, iMouseX, iMouseY);
					}
				}
			}

			menu_font_small.draw(0, 0, szEditModes[edit_mode]);

			if (fAutoPaint)
				menu_font_small.draw(0, 16, "Auto Paint");

            menu_font_small.drawRightJustified(640, 0, worldlist->current_name());


            if (--g_musiccategorydisplaytimer > 0) {
				spr_dialog.draw(224, 176, 0, 0, 192, 128);
				menu_font_small.drawCentered(320, 195, "Music Category");
				menu_font_large.drawCentered(320, 220, g_szWorldMusicCategoryNames[g_worldmap.iMusicCategory]);

				menu_font_small.drawCentered(320, 255, "Press 'R' Again");
				menu_font_small.drawCentered(320, 270, "To Change");
			}

			DrawMessage();
		}

		SDL_Flip(screen);

		int delay = WAITTIME - (SDL_GetTicks() - framestart);
		if (delay < 0)
			delay = 0;
		else if (delay > WAITTIME)
			delay = WAITTIME;

		SDL_Delay(delay);
	}

	return EDITOR_QUIT;
}

void DrawMessage()
{
    if (g_messagedisplaytimer > 0) {
		--g_messagedisplaytimer;

		spr_dialog.draw(224, 176, 0, 0, 192, 128);
		menu_font_large.drawCentered(320, 195, g_szMessageTitle.c_str());
		menu_font_large.drawCentered(320, 220, g_szMessageLine[0].c_str());
		menu_font_large.drawCentered(320, 240, g_szMessageLine[1].c_str());
		menu_font_large.drawCentered(320, 260, g_szMessageLine[2].c_str());
	}
}

void GetForegroundTileValues(short iCol, short iRow, short iOldTiles[9])
{
	short iIndex = 0;
    for (short iAutoRow = iRow - 1; iAutoRow <= iRow + 1; iAutoRow++) {
        for (short iAutoCol = iCol - 1; iAutoCol <= iCol + 1; iAutoCol++) {
            if (iAutoRow >= 0 && iAutoRow < iWorldHeight && iAutoCol >= 0 && iAutoCol < iWorldWidth) {
				iOldTiles[iIndex++] = g_worldmap.tiles[iAutoCol][iAutoRow].iForegroundSprite;
			}
		}
	}

	for (short i = iIndex; i < 9; i++)
		iOldTiles[i] = 0;
}

bool ForegroundTileValuesChanged(short iCol, short iRow, short iOldTiles[9])
{
	short iIndex = 0;
    for (short iAutoRow = iRow - 1; iAutoRow <= iRow + 1; iAutoRow++) {
        for (short iAutoCol = iCol - 1; iAutoCol <= iCol + 1; iAutoCol++) {
            if (iAutoRow >= 0 && iAutoRow < iWorldHeight && iAutoCol >= 0 && iAutoCol < iWorldWidth) {
				if (g_worldmap.tiles[iAutoCol][iAutoRow].iForegroundSprite != iOldTiles[iIndex++])
					return true;
			}
		}
	}

	return false;
}

void ReadVehiclesIntoEditor()
{
	std::vector<WorldVehicle*>::iterator itr = vehiclelist.begin(), lim = vehiclelist.end();
    while (itr != lim) {
		delete (*itr);
		itr++;
	}

	vehiclelist.clear();

    for (short iVehicle = 0; iVehicle < g_worldmap.iNumVehicles; iVehicle++) {
		WorldVehicle * vehicle = &g_worldmap.vehicles[iVehicle];
		WorldVehicle * vehiclecopy = new WorldVehicle();

		vehiclecopy->iDrawSprite = vehicle->iDrawSprite;
		vehiclecopy->iActionId = vehicle->iActionId;
		vehiclecopy->iCurrentTileX = vehicle->iCurrentTileX;
		vehiclecopy->iCurrentTileY = vehicle->iCurrentTileY;
		vehiclecopy->iMinMoves = vehicle->iMinMoves;
		vehiclecopy->iMaxMoves = vehicle->iMaxMoves;
		vehiclecopy->fSpritePaces = vehicle->fSpritePaces;
		vehiclecopy->iDrawDirection = vehicle->iDrawDirection;
		vehiclecopy->iBoundary = vehicle->iBoundary;

		vehiclelist.push_back(vehiclecopy);
	}
}

void WriteVehiclesIntoWorld()
{
	//Cleanup old vehicles
	delete [] g_worldmap.vehicles;

	//Insert new vehicles
	g_worldmap.iNumVehicles = vehiclelist.size();
	g_worldmap.vehicles = new WorldVehicle[g_worldmap.iNumVehicles];

    for (short iVehicle = 0; iVehicle < g_worldmap.iNumVehicles; iVehicle++) {
		WorldVehicle * vehicle = vehiclelist[iVehicle];
		WorldVehicle * vehiclecopy = &g_worldmap.vehicles[iVehicle];

		vehiclecopy->iDrawSprite = vehicle->iDrawSprite;
		vehiclecopy->iActionId = vehicle->iActionId;
		vehiclecopy->iCurrentTileX = vehicle->iCurrentTileX;
		vehiclecopy->iCurrentTileY = vehicle->iCurrentTileY;
		vehiclecopy->iMinMoves = vehicle->iMinMoves;
		vehiclecopy->iMaxMoves = vehicle->iMaxMoves;
		vehiclecopy->fSpritePaces = vehicle->fSpritePaces;
		vehiclecopy->iDrawDirection = vehicle->iDrawDirection;
		vehiclecopy->iBoundary = vehicle->iBoundary;
	}
}

void AddVehicleToTile(short iCol, short iRow, short iType)
{
	std::vector<WorldVehicle*>::iterator itr = vehiclelist.begin(), lim = vehiclelist.end();
	WorldVehicle * newvehicle = NULL;
    while (itr != lim) {
		WorldVehicle * vehicle = *itr;
        if (vehicle->iCurrentTileX == iCol && vehicle->iCurrentTileY == iRow) {
			newvehicle = vehicle;
			break;
		}

		itr++;
	}

    if (!newvehicle) {
		newvehicle = new WorldVehicle();
		newvehicle->iCurrentTileX = iCol;
		newvehicle->iCurrentTileY = iRow;
		vehiclelist.push_back(newvehicle);
	}

	newvehicle->iDrawSprite = g_wvVehicleStamp.iDrawSprite;
	newvehicle->iActionId = g_wvVehicleStamp.iActionId;
	newvehicle->iMinMoves = g_wvVehicleStamp.iMinMoves;
	newvehicle->iMaxMoves = g_wvVehicleStamp.iMaxMoves;
	newvehicle->fSpritePaces = g_wvVehicleStamp.fSpritePaces;
	newvehicle->iDrawDirection = g_wvVehicleStamp.iDrawDirection;
	newvehicle->iBoundary = g_wvVehicleStamp.iBoundary;
}

void RemoveVehicleFromTile(short iCol, short iRow)
{
	std::vector<WorldVehicle*>::iterator itr = vehiclelist.begin(), lim = vehiclelist.end();
    while (itr != lim) {
		WorldVehicle * vehicle = *itr;
        if (vehicle->iCurrentTileX == iCol && vehicle->iCurrentTileY == iRow) {
			delete (*itr);

			itr = vehiclelist.erase(itr);
			lim = vehiclelist.end();

			return;
		}

		itr++;
	}
}

void ReadWarpsIntoEditor()
{
	std::vector<WorldWarp*>::iterator itr = warplist.begin(), lim = warplist.end();
    while (itr != lim) {
		delete (*itr);
		itr++;
	}

	warplist.clear();

    for (short iWarp = 0; iWarp < g_worldmap.iNumWarps; iWarp++) {
		WorldWarp * warp = &g_worldmap.warps[iWarp];
		WorldWarp * warpcopy = new WorldWarp();

		warpcopy->iID = warp->iID;
		warpcopy->iCol1 = warp->iCol1;
		warpcopy->iRow1 = warp->iRow1;
		warpcopy->iCol2 = warp->iCol2;
		warpcopy->iRow2 = warp->iRow2;

		warplist.push_back(warpcopy);
	}
}

void WriteWarpsIntoWorld()
{
	//Cleanup old vehicles
	delete [] g_worldmap.warps;

	//Insert new vehicles
	g_worldmap.iNumWarps = warplist.size();
	g_worldmap.warps = new WorldWarp[g_worldmap.iNumWarps];

    for (short iWarp = 0; iWarp < g_worldmap.iNumWarps; iWarp++) {
		WorldWarp * warp = warplist[iWarp];
		WorldWarp * warpcopy = &g_worldmap.warps[iWarp];

		warpcopy->iID = warp->iID;
		warpcopy->iCol1 = warp->iCol1;
		warpcopy->iRow1 = warp->iRow1;
		warpcopy->iCol2 = warp->iCol2;
		warpcopy->iRow2 = warp->iRow2;
	}
}

void AddWarpToTile(short iCol, short iRow, short iType)
{
	std::vector<WorldWarp*>::iterator itr = warplist.begin(), lim = warplist.end();
	WorldWarp * newwarp = NULL;
    while (itr != lim) {
		WorldWarp * warp = *itr;
        if (warp->iID == iType) {
			newwarp = warp;
			break;
		}

		itr++;
	}

    if (!newwarp) {
		newwarp = new WorldWarp();

		newwarp->iID = iType;
		newwarp->iCol1 = iCol;
		newwarp->iRow1 = iRow;
		newwarp->iCol2 = -1;
		newwarp->iRow2 = -1;

		warplist.push_back(newwarp);
    } else {
        if (newwarp->iCol1 == -1) {
			newwarp->iCol1 = iCol;
			newwarp->iRow1 = iRow;
        } else if (newwarp->iCol1 != iCol || newwarp->iRow1 != iRow) {
			newwarp->iCol2 = iCol;
			newwarp->iRow2 = iRow;
		}
	}
}

void RemoveWarpFromTile(short iCol, short iRow)
{
	std::vector<WorldWarp*>::iterator itr = warplist.begin(), lim = warplist.end();
    while (itr != lim) {
		WorldWarp * warp = *itr;
        if (warp->iCol1 == iCol && warp->iRow1 == iRow) {
            if (warp->iCol2 == -1 && warp->iRow2 == -1) {
				delete (*itr);

				itr = warplist.erase(itr);
				lim = warplist.end();

				return;
            } else {
				warp->iCol1 = -1;
				warp->iRow1 = -1;
			}
        } else if (warp->iCol2 == iCol && warp->iRow2 == iRow) {
            if (warp->iCol1 == -1 && warp->iRow1 == -1) {
				delete (*itr);

				itr = warplist.erase(itr);
				lim = warplist.end();

				return;
            } else {
				warp->iCol2 = -1;
				warp->iRow2 = -1;
			}
		}

		itr++;
	}
}

void UpdatePathSprite(short iCol, short iRow)
{
    for (short iAutoRow = iRow - 1; iAutoRow <= iRow + 1; iAutoRow++) {
        for (short iAutoCol = iCol - 1; iAutoCol <= iCol + 1; iAutoCol++) {
            if (iAutoRow >= 0 && iAutoRow < iWorldHeight && iAutoCol >= 0 && iAutoCol < iWorldWidth) {
				AutoSetPathSprite(iAutoCol, iAutoRow);
			}
		}
	}
}

void AutoSetPathSprite(short iCol, short iRow)
{
	short iPathTypes[16] = {6, 4, 3, 5, 6, 4, 1, 5, 6, 2, 3, 5, 6, 4, 3, 5};

	short iPath = 0;
	short iNeighborIndex = 0;

	short iForegroundSprite = g_worldmap.tiles[iCol][iRow].iForegroundSprite;
	short iForegroundStyle = iForegroundSprite / WORLD_PATH_SPRITE_SET_SIZE;

	if (iForegroundSprite == 0 || iForegroundSprite >= WORLD_FOREGROUND_STAGE_OFFSET)
		return;

    for (short iAutoRow = iRow - 1; iAutoRow <= iRow + 1; iAutoRow++) {
        for (short iAutoCol = iCol - 1; iAutoCol <= iCol + 1; iAutoCol++) {
			if (iAutoCol == iCol && iAutoRow == iRow)
				continue;

            if ((iAutoCol == iCol && iAutoRow != iRow) || (iAutoCol != iCol && iAutoRow == iRow)) {
                if (iAutoRow >= 0 && iAutoRow < iWorldHeight && iAutoCol >= 0 && iAutoCol < iWorldWidth) {
					iForegroundSprite = g_worldmap.tiles[iAutoCol][iAutoRow].iForegroundSprite;

					if ((iForegroundSprite >= WORLD_BRIDGE_SPRITE_OFFSET && iForegroundSprite <= WORLD_BRIDGE_SPRITE_OFFSET + 3) ||
						(iForegroundSprite >= WORLD_FOREGROUND_STAGE_OFFSET && iForegroundSprite <= WORLD_FOREGROUND_STAGE_OFFSET + 399) ||
						/*(iForegroundSprite >= WORLD_FOREGROUND_SPRITE_OFFSET && iForegroundSprite <= WORLD_FOREGROUND_SPRITE_OFFSET + 179) ||*/
						/*(iForegroundSprite >= WORLD_FOREGROUND_SPRITE_ANIMATED_OFFSET && iForegroundSprite <= WORLD_FOREGROUND_SPRITE_ANIMATED_OFFSET + 29) ||*/
                            (iForegroundSprite >= WORLD_START_SPRITE_OFFSET && iForegroundSprite <= WORLD_START_SPRITE_OFFSET + 1)) {
						iPath += 1 << iNeighborIndex;
                    } else if (iForegroundSprite >= 0 && iForegroundSprite < WORLD_FOREGROUND_STAGE_OFFSET) {
						short iPathSprite = iForegroundSprite % WORLD_PATH_SPRITE_SET_SIZE;

						if (iPathSprite >= 1 && iPathSprite <= 18)
							iPath += 1 << iNeighborIndex;
					}
				}

				iNeighborIndex++;
			}
		}
	}

	//#1 == -  2 == |  3 == -o  4 == !  5 == -`  6 == o
	g_worldmap.tiles[iCol][iRow].iForegroundSprite = AdjustForeground(iPathTypes[iPath] + iForegroundStyle * WORLD_PATH_SPRITE_SET_SIZE, iCol, iRow);
}

//Convert foreground sprite to match the background sprite
short AdjustForeground(short iSprite, short iCol, short iRow)
{
	if (iSprite >= WORLD_FOREGROUND_STAGE_OFFSET)
		return iSprite;

	short iBackgroundSprite = g_worldmap.tiles[iCol][iRow].iBackgroundSprite % WORLD_BACKGROUND_SPRITE_SET_SIZE;

	short iPathStyle = iSprite / WORLD_PATH_SPRITE_SET_SIZE;
	iSprite %= WORLD_PATH_SPRITE_SET_SIZE;

	//Convert already adjusted sprites back to their "base" sprite
	if (iSprite >= 11 && iSprite <= 18)
		iSprite = 2 - (iSprite % 2);
	else if (iSprite >= 7 && iSprite <= 10)
		iSprite -= 4;

	if (iBackgroundSprite == 1)
		return iSprite + iPathStyle * WORLD_PATH_SPRITE_SET_SIZE;

	if (iSprite == 2 && (iBackgroundSprite == 12 || iBackgroundSprite == 20 || iBackgroundSprite == 23 ||
		iBackgroundSprite == 24 || iBackgroundSprite == 27 || iBackgroundSprite == 32 || iBackgroundSprite == 36 ||
                        iBackgroundSprite == 37 || iBackgroundSprite == 40)) {
		iSprite = 14;
    } else if (iSprite == 2 && (iBackgroundSprite == 13 || iBackgroundSprite == 21 || iBackgroundSprite == 22 ||
		iBackgroundSprite == 25 || iBackgroundSprite == 26 || iBackgroundSprite == 33 || iBackgroundSprite == 34 ||
                               iBackgroundSprite == 35 || iBackgroundSprite == 41)) {
		iSprite = 16;
    } else if (iSprite == 1 && (iBackgroundSprite == 14 || iBackgroundSprite == 18 || iBackgroundSprite == 19 ||
		iBackgroundSprite == 26 || iBackgroundSprite == 27 || iBackgroundSprite == 31 || iBackgroundSprite == 34 ||
                               iBackgroundSprite == 37 || iBackgroundSprite == 39)) {
		iSprite = 13;
    } else if (iSprite == 1 && (iBackgroundSprite == 15 || iBackgroundSprite == 16 || iBackgroundSprite == 17 ||
		iBackgroundSprite == 24 || iBackgroundSprite == 25 || iBackgroundSprite == 30 || iBackgroundSprite == 35 ||
                               iBackgroundSprite == 36 || iBackgroundSprite == 38)) {
		iSprite = 15;
    } else if (iSprite == 2 && (iBackgroundSprite == 28 || iBackgroundSprite == 38 || iBackgroundSprite == 39 ||
                               iBackgroundSprite == 43)) {
		iSprite = 12;
    } else if (iSprite == 1 && (iBackgroundSprite == 28 || iBackgroundSprite == 40 || iBackgroundSprite == 41 ||
                               iBackgroundSprite == 42)) {
		iSprite = 11;
    } else {
		if (iSprite == 1 || iSprite == 2)
			iSprite += 16;
		else if (iSprite >= 3 && iSprite <= 6)
			iSprite += 4;
	}

	return iSprite + iPathStyle * WORLD_PATH_SPRITE_SET_SIZE;
}


void UpdatePath(short iCol, short iRow)
{
    for (short iAutoRow = iRow - 1; iAutoRow <= iRow + 1; iAutoRow++) {
        for (short iAutoCol = iCol - 1; iAutoCol <= iCol + 1; iAutoCol++) {
            if (iAutoRow >= 0 && iAutoRow < iWorldHeight && iAutoCol >= 0 && iAutoCol < iWorldWidth) {
				AutoSetPath(iAutoCol, iAutoRow);
			}
		}
	}
}

void AutoSetPath(short iCol, short iRow)
{
	short iPathTypes[16] = {11, 1, 2, 3, 2, 4, 2, 8, 1, 1, 6, 7, 5, 9, 10, 11};

	short iPath = 0;
	short iNeighborIndex = 0;

	if (g_worldmap.tiles[iCol][iRow].iConnectionType == 0)
		return;

    for (short iAutoRow = iRow - 1; iAutoRow <= iRow + 1; iAutoRow++) {
        for (short iAutoCol = iCol - 1; iAutoCol <= iCol + 1; iAutoCol++) {
			if (iAutoCol == iCol && iAutoRow == iRow)
				continue;

            if ((iAutoCol == iCol && iAutoRow != iRow) || (iAutoCol != iCol && iAutoRow == iRow)) {
                if (iAutoRow >= 0 && iAutoRow < iWorldHeight && iAutoCol >= 0 && iAutoCol < iWorldWidth) {
					if (g_worldmap.tiles[iAutoCol][iAutoRow].iConnectionType > 0)
						iPath += 1 << iNeighborIndex;
				}

				iNeighborIndex++;
			}
		}
	}

	//#1 == |  2 == -  3 == -!  4 == L  5 == ,-  6 == -,
	//#7 == -|  8 == -`-  9 == |-  10 == -,-  11 == +

	short iPathType = iPathTypes[iPath];
	short iForegroundSprite = g_worldmap.tiles[iCol][iRow].iForegroundSprite;

    if (iPathType == 2 && iForegroundSprite >= WORLD_BRIDGE_SPRITE_OFFSET && iForegroundSprite <= WORLD_BRIDGE_SPRITE_OFFSET + 1) {
		iPathType = iForegroundSprite - WORLD_BRIDGE_SPRITE_OFFSET + 12;
    } else if (iPathType == 1 && iForegroundSprite >= WORLD_BRIDGE_SPRITE_OFFSET + 2 && iForegroundSprite <= WORLD_BRIDGE_SPRITE_OFFSET + 3) {
		iPathType = iForegroundSprite - WORLD_BRIDGE_SPRITE_OFFSET + 12;
	}

	g_worldmap.tiles[iCol][iRow].iConnectionType = iPathType;
}

bool UpdateForeground(short iCol, short iRow)
{
	short iNewForeground = AdjustForeground(g_worldmap.tiles[iCol][iRow].iForegroundSprite, iCol, iRow);

    if (g_worldmap.tiles[iCol][iRow].iForegroundSprite != iNewForeground) {
		g_worldmap.tiles[iCol][iRow].iForegroundSprite = iNewForeground;
		return true;
	}

	return false;
}

bool UpdateCoastline(short iCol, short iRow)
{
	short iStartCol = iCol == 0 ? 0 : iCol - 1;
	short iEndCol = iCol == iWorldWidth - 1 ? iWorldWidth - 1 : iCol + 1;

	short iStartRow = iRow == 0 ? 0 : iRow - 1;
	short iEndRow = iRow == iWorldHeight - 1 ? iWorldHeight - 1 : iRow + 1;

	bool fRet = false;
    for (short iAutoRow = iStartRow; iAutoRow <= iEndRow; iAutoRow++) {
        for (short iAutoCol = iStartCol; iAutoCol <= iEndCol; iAutoCol++) {
			fRet |= AutoSetTile(iAutoCol, iAutoRow);
			fRet |= UpdateForeground(iAutoCol, iAutoRow);
		}
	}

	return fRet;
}

bool AutoSetTile(short iCol, short iRow)
{
	//Don't need to do anything if this tile is solid
	if (g_worldmap.tiles[iCol][iRow].iBackgroundSprite % WORLD_BACKGROUND_SPRITE_SET_SIZE == 1)
		return false;

	bool iTile[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	short iNeighborIndex = 0;

	short iNeighborStyle[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    for (short iAutoRow = iRow - 1; iAutoRow <= iRow + 1; iAutoRow++) {
        for (short iAutoCol = iCol - 1; iAutoCol <= iCol + 1; iAutoCol++) {
			if (iAutoCol == iCol && iAutoRow == iRow)
				continue;

            if (iAutoRow >= 0 && iAutoRow < iWorldHeight && iAutoCol >= 0 && iAutoCol < iWorldWidth) {
				short iBackgroundSprite = g_worldmap.tiles[iAutoCol][iAutoRow].iBackgroundSprite;

                if (iBackgroundSprite % WORLD_BACKGROUND_SPRITE_SET_SIZE == 1) {
					iTile[iNeighborIndex] = true;
					iNeighborStyle[iBackgroundSprite / WORLD_BACKGROUND_SPRITE_SET_SIZE]++;
				}
			}

			iNeighborIndex++;
		}
	}

	short iMaxStyle = 0;
	short iTileStyleOffset = 0;
    for (short iStyle = 0; iStyle < 10; iStyle++) {
        if (iNeighborStyle[iStyle] > iMaxStyle) {
			iMaxStyle = iNeighborStyle[iStyle];
			iTileStyleOffset = iStyle * WORLD_BACKGROUND_SPRITE_SET_SIZE;
		}
	}

	short iNewTile = 0;
    if (iTile[0] && !iTile[1] && iTile[2] && !iTile[3] && !iTile[4] && iTile[5] && !iTile[6] && iTile[7]) {
		iNewTile = iTileStyleOffset + 44;
    } else if (iTile[0] && !iTile[1] && !iTile[3] && iTile[4] && iTile[5] && !iTile[6]) {
		iNewTile = iTileStyleOffset + 30;
    } else if (!iTile[1] && iTile[2] && iTile[3] && !iTile[4] && !iTile[6] && iTile[7]) {
		iNewTile = iTileStyleOffset + 31;
    } else if (iTile[1] && !iTile[3] && !iTile[4] && iTile[5] && !iTile[6] && iTile[7]) {
		iNewTile = iTileStyleOffset + 32;
    } else if (iTile[0] && !iTile[1] && iTile[2] && !iTile[3] && !iTile[4] && iTile[6]) {
		iNewTile = iTileStyleOffset + 33;
    } else if (!iTile[1] && iTile[2] && iTile[3] && !iTile[4] && iTile[6]) {
		iNewTile = iTileStyleOffset + 34;
    } else if (iTile[0] && !iTile[1] && !iTile[3] && iTile[4] && iTile[6]) {
		iNewTile = iTileStyleOffset + 35;
    } else if (iTile[1] && !iTile[3] && iTile[4] && iTile[5] && !iTile[6]) {
		iNewTile = iTileStyleOffset + 36;
    } else if (iTile[1] && iTile[3] && !iTile[4] && !iTile[6] && iTile[7]) {
		iNewTile = iTileStyleOffset + 37;
    } else if (iTile[0] && !iTile[1] && iTile[2] && !iTile[4] && iTile[7]) {
		iNewTile = iTileStyleOffset + 45;
    } else if (iTile[2] && !iTile[4] && iTile[5] && !iTile[6] && iTile[7]) {
		iNewTile = iTileStyleOffset + 46;
    } else if (iTile[0] && !iTile[3] && iTile[5] && !iTile[6] && iTile[7]) {
		iNewTile = iTileStyleOffset + 47;
    } else if (iTile[0] && !iTile[1] && iTile[2] && !iTile[3] && iTile[5]) {
		iNewTile = iTileStyleOffset + 48;
    } else if (iTile[1] && iTile[3] && iTile[4] && iTile[6]) {
		iNewTile = iTileStyleOffset + 28;
    } else if (iTile[1] && !iTile[3] && iTile[4] && iTile[6]) {
		iNewTile = iTileStyleOffset + 38;
    } else if (iTile[1] && iTile[3] && !iTile[4] && iTile[6]) {
		iNewTile = iTileStyleOffset + 39;
    } else if (iTile[1] && iTile[3] && iTile[4] && !iTile[6]) {
		iNewTile = iTileStyleOffset + 40;
    } else if (!iTile[1] && iTile[3] && iTile[4] && iTile[6]) {
		iNewTile = iTileStyleOffset + 41;
    } else if (!iTile[1] && iTile[3] && iTile[4] && !iTile[6]) {
		iNewTile = iTileStyleOffset + 42;
    } else if (iTile[1] && !iTile[3] && !iTile[4] && iTile[6]) {
		iNewTile = iTileStyleOffset + 43;
    } else if (iTile[0] && !iTile[1] && iTile[4]) {
		iNewTile = iTileStyleOffset + 16;
    } else if (iTile[4] && iTile[5] && !iTile[6]) {
		iNewTile = iTileStyleOffset + 17;
    } else if (iTile[3] && !iTile[6] && iTile[7]) {
		iNewTile = iTileStyleOffset + 18;
    } else if (!iTile[1] && iTile[2] && iTile[3]) {
		iNewTile = iTileStyleOffset + 19;
    } else if (iTile[1] && !iTile[4] && iTile[7]) {
		iNewTile = iTileStyleOffset + 20;
    } else if (iTile[2] && !iTile[4] && iTile[6]) {
		iNewTile = iTileStyleOffset + 21;
    } else if (iTile[0] && !iTile[3] && iTile[6]) {
		iNewTile = iTileStyleOffset + 22;
    } else if (iTile[1] && !iTile[3] && iTile[5]) {
		iNewTile = iTileStyleOffset + 23;
    } else if (iTile[0] && !iTile[1] && iTile[2]) {
		iNewTile = iTileStyleOffset + 6;
    } else if (iTile[5] && !iTile[6] && iTile[7]) {
		iNewTile = iTileStyleOffset + 7;
    } else if (iTile[0] && !iTile[3] && iTile[5]) {
		iNewTile = iTileStyleOffset + 8;
    } else if (iTile[2] && !iTile[4] && iTile[7]) {
		iNewTile = iTileStyleOffset + 9;
    } else if (iTile[1] && iTile[4]) {
		iNewTile = iTileStyleOffset + 24;
    } else if (iTile[4] && iTile[6]) {
		iNewTile = iTileStyleOffset + 25;
    } else if (iTile[3] && iTile[6]) {
		iNewTile = iTileStyleOffset + 26;
    } else if (iTile[1] && iTile[3]) {
		iNewTile = iTileStyleOffset + 27;
    } else if (iTile[2] && iTile[5]) {
		iNewTile = iTileStyleOffset + 10;
    } else if (iTile[0] && iTile[7]) {
		iNewTile = iTileStyleOffset + 11;
    } else if (iTile[1]) {
		iNewTile = iTileStyleOffset + 12;
    } else if (iTile[6]) {
		iNewTile = iTileStyleOffset + 13;
    } else if (iTile[3]) {
		iNewTile = iTileStyleOffset + 14;
    } else if (iTile[4]) {
		iNewTile = iTileStyleOffset + 15;
    } else if (iTile[2]) {
		iNewTile = iTileStyleOffset + 2;
    } else if (iTile[0]) {
		iNewTile = iTileStyleOffset + 3;
    } else if (iTile[5]) {
		iNewTile = iTileStyleOffset + 4;
    } else if (iTile[7]) {
		iNewTile = iTileStyleOffset + 5;
    } else {
		iNewTile = iTileStyleOffset + 0;
	}

    if (g_worldmap.tiles[iCol][iRow].iBackgroundSprite != iNewTile) {
		g_worldmap.tiles[iCol][iRow].iBackgroundSprite = iNewTile;
		return true;
	}

	return false;
}

void updateworldsurface()
{
	g_worldmap.DrawMapToSurface(-1, true, sMapSurface, draw_offset_col, draw_offset_row, 0);
}

void drawmap(bool fScreenshot, short iBlockSize)
{
	if (fNeedBlackBackground)
		SDL_FillRect(screen, NULL, 0x0);

	SDL_BlitSurface(sMapSurface, &rectSrcSurface, blitdest, &rectDstSurface);
}

int editor_warp()
{
	bool done = false;

    while (!done) {
		int framestart = SDL_GetTicks();

		//handle messages
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT: {
					done = true;
					break;
				}

            case SDL_KEYDOWN: {
					edit_mode = 6;  //change to edit mode using warps
					return EDITOR_EDIT;
				}

            case SDL_MOUSEBUTTONDOWN: {
					short iButtonX = event.button.x / TILESIZE;
					short iButtonY = event.button.y / TILESIZE;

                if (event.button.button == SDL_BUTTON_LEFT) {
                    if (iButtonX >= 0 && iButtonX <= 9 && iButtonY == 0) {
							set_tile = iButtonX;

							edit_mode = 6;  //change to edit mode using warps

							//The user must release the mouse button before trying to add a tile
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
		menu_shade.draw(0, 0);

		spr_warps[0].draw(0, 0, 0, 0, 320, 32);

        menu_font_small.drawRightJustified(640, 0, worldlist->current_name());

		DrawMessage();
		SDL_Flip(screen);

		int delay = WAITTIME - (SDL_GetTicks() - framestart);
		if (delay < 0)
			delay = 0;
		else if (delay > WAITTIME)
			delay = WAITTIME;

		SDL_Delay(delay);
	}

	return EDITOR_QUIT;
}

int editor_start_items()
{
	bool done = false;

	SDL_Rect rItemDst[NUM_POWERUPS + NUM_WORLD_POWERUPS];

	SDL_Rect rPickedItemDst[32];

	short iColCount = 0;
	short iRowCount = 0;
    for (short iItem = 0; iItem < NUM_POWERUPS + NUM_WORLD_POWERUPS; iItem++) {
		rItemDst[iItem].x = 16 + iColCount * 48;
		rItemDst[iItem].y = 16 + iRowCount * 48;
		rItemDst[iItem].w = 32;
		rItemDst[iItem].h = 32;

        if (++iColCount > 12) {
			iColCount = 0;
			iRowCount++;
		}
	}

	short iPickedItem = 0;
    for (short iPickedItemY = 0; iPickedItemY < 4; iPickedItemY++) {
        for (short iPickedItemX = 0; iPickedItemX < 8; iPickedItemX++) {
			rPickedItemDst[iPickedItem].x = 122 + iPickedItemX * 52;
			rPickedItemDst[iPickedItem].y = 240 + iPickedItemY * 64;
			rPickedItemDst[iPickedItem].w = 32;
			rPickedItemDst[iPickedItem].h = 32;

			iPickedItem++;
		}
	}

    while (!done) {
		int framestart = SDL_GetTicks();

		//handle messages
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT: {
					done = true;
					break;
				}

            case SDL_KEYDOWN: {
					return EDITOR_EDIT;
				}

            case SDL_MOUSEBUTTONDOWN: {
					short iButtonX = event.button.x;
					short iButtonY = event.button.y;

                if (event.button.button == SDL_BUTTON_LEFT || event.button.button == SDL_BUTTON_RIGHT) {
                    if (g_worldmap.iNumInitialBonuses < 32) {
                        for (short iItem = 0; iItem < NUM_POWERUPS + NUM_WORLD_POWERUPS; iItem++) {
								if (iButtonX >= rItemDst[iItem].x && iButtonX < rItemDst[iItem].w + rItemDst[iItem].x &&
                                    iButtonY >= rItemDst[iItem].y && iButtonY < rItemDst[iItem].h + rItemDst[iItem].y) {
									g_worldmap.iInitialBonuses[g_worldmap.iNumInitialBonuses++] = iItem;
									break;
								}
							}
						}

                    for (short iRemoveItem = 0; iRemoveItem < g_worldmap.iNumInitialBonuses; iRemoveItem++) {
							if (iButtonX >= rPickedItemDst[iRemoveItem].x && iButtonX < rPickedItemDst[iRemoveItem].w + rPickedItemDst[iRemoveItem].x &&
                                iButtonY >= rPickedItemDst[iRemoveItem].y && iButtonY < rPickedItemDst[iRemoveItem].h + rPickedItemDst[iRemoveItem].y) {
                            for (short iAdjust = iRemoveItem; iAdjust < g_worldmap.iNumInitialBonuses - 1; iAdjust++) {
									g_worldmap.iInitialBonuses[iAdjust] = g_worldmap.iInitialBonuses[iAdjust + 1];
								}

								g_worldmap.iNumInitialBonuses--;

								break;
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
		menu_shade.draw(0, 0);

        for (short iItem = 0; iItem < NUM_POWERUPS; iItem++) {
			spr_storedpoweruplarge.draw(rItemDst[iItem].x, rItemDst[iItem].y, iItem << 5, 0, 32, 32);
		}

        for (short iWorldItem = 0; iWorldItem < NUM_WORLD_POWERUPS; iWorldItem++) {
			spr_worlditems.draw(rItemDst[iWorldItem + NUM_POWERUPS].x, rItemDst[iWorldItem + NUM_POWERUPS].y, iWorldItem << 5, 0, 32, 32);
		}

        for (short iPopup = 0; iPopup < 4; iPopup++) {
			spr_worldpopup.draw(0, 416 - (iPopup << 6), 0, 0, 320, 64);
			spr_worldpopup.draw(320, 416 - (iPopup << 6), 192, 0, 320, 64);
		}

        for (short iPickedItem = 0; iPickedItem < g_worldmap.iNumInitialBonuses; iPickedItem++) {
			short iPowerup = g_worldmap.iInitialBonuses[iPickedItem];
			if (iPowerup >= NUM_POWERUPS)
				spr_worlditems.draw(rPickedItemDst[iPickedItem].x, rPickedItemDst[iPickedItem].y, (iPowerup - NUM_POWERUPS) << 5, 0, 32, 32);
			else
				spr_storedpoweruplarge.draw(rPickedItemDst[iPickedItem].x, rPickedItemDst[iPickedItem].y, iPowerup << 5, 0, 32, 32);
		}

        menu_font_small.drawRightJustified(640, 0, worldlist->current_name());

		DrawMessage();
		SDL_Flip(screen);

		int delay = WAITTIME - (SDL_GetTicks() - framestart);
		if (delay < 0)
			delay = 0;
		else if (delay > WAITTIME)
			delay = WAITTIME;

		SDL_Delay(delay);
	}

	return EDITOR_QUIT;
}

int editor_boundary()
{
	bool done = false;

    while (!done) {
		int framestart = SDL_GetTicks();

		//handle messages
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT: {
					done = true;
					break;
				}

            case SDL_KEYDOWN: {
					edit_mode = 8;  //change to edit mode using warps
					return EDITOR_EDIT;
				}

            case SDL_MOUSEBUTTONDOWN: {
					short iButtonX = event.button.x / TILESIZE;
					short iButtonY = event.button.y / TILESIZE;

                if (event.button.button == SDL_BUTTON_LEFT) {
                    if (iButtonX >= 0 && iButtonX <= 9 && iButtonY >= 0 && iButtonY <= 9) {
							set_tile = iButtonX + 10 * iButtonY + 1;

							edit_mode = 8;  //change to edit mode using warps

							//The user must release the mouse button before trying to add a tile
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
		menu_shade.draw(0, 0);

		int color = SDL_MapRGB(blitdest->format, 255, 0, 255);
		SDL_Rect r = {0, 0, 320, 320};
		SDL_FillRect(blitdest, &r, color);

		spr_worldforegroundspecial[0].draw(0, 0, 0, 0, 320, 320);

        menu_font_small.drawRightJustified(640, 0, worldlist->current_name());

		DrawMessage();
		SDL_Flip(screen);

		int delay = WAITTIME - (SDL_GetTicks() - framestart);
		if (delay < 0)
			delay = 0;
		else if (delay > WAITTIME)
			delay = WAITTIME;

		SDL_Delay(delay);
	}

	return EDITOR_QUIT;
}

int editor_type()
{
	bool done = false;

	SDL_Rect r;
	r.x = 0;
	r.y = 0;
	r.w = 640;
	r.h = 480;

    while (!done) {
		int framestart = SDL_GetTicks();

		//handle messages
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT: {
					done = true;
					break;
				}

            case SDL_KEYDOWN: {
					edit_mode = 3;  //change to edit mode using doors/start
					return EDITOR_EDIT;

					break;
				}

            case SDL_MOUSEBUTTONDOWN: {
                if (event.button.button == SDL_BUTTON_LEFT) {
						short iButtonX = event.button.x / TILESIZE;
						short iButtonY = event.button.y / TILESIZE;

						//Start and doors
                    if (iButtonX >= 0 && iButtonX <= 5 && iButtonY == 0) {
							set_tile = iButtonX;
						}

						edit_mode = 3;  //change to edit mode using warps

						//The user must release the mouse button before trying to add a tile
						ignoreclick = true;

						return EDITOR_EDIT;
					}

					break;
				}

				default:
					break;
			}
		}


		drawmap(false, TILESIZE);
		menu_shade.draw(0, 0);

		spr_worldforegroundspecial[0].draw(0, 0, 320, 128, 64, 32);
		spr_worldforegroundspecial[0].draw(64, 0, 320, 192, 128, 32);

		spr_worldforegroundspecial[0].draw(64, 0, 448, 64, 128, 32);

        menu_font_small.drawRightJustified(640, 0, worldlist->current_name());

		DrawMessage();
		SDL_Flip(screen);

		int delay = WAITTIME - (SDL_GetTicks() - framestart);
		if (delay < 0)
			delay = 0;
		else if (delay > WAITTIME)
			delay = WAITTIME;

		SDL_Delay(delay);
	}

	return EDITOR_QUIT;
}

int editor_water()
{
	bool done = false;

    while (!done) {
		int framestart = SDL_GetTicks();

		//handle messages
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT: {
					done = true;
					break;
				}

            case SDL_KEYDOWN: {
					edit_mode = 7;
					return EDITOR_EDIT;
				}

            case SDL_MOUSEBUTTONDOWN: {
                if (event.button.button == SDL_BUTTON_LEFT) {
						short iButtonX = event.button.x / TILESIZE;
						short iButtonY = event.button.y / TILESIZE;

                    if (iButtonY == 0) {
                        if (iButtonX >= 0 && iButtonX <= 2) {
								set_tile = iButtonX + 4;
							}
						}

						ignoreclick = true;
						edit_mode = 7;
						return EDITOR_EDIT;
					}

					break;
				}

				default:
					break;
			}
		}

		SDL_FillRect(screen, NULL, 0x0);

		for (short iWater = 0; iWater < 3; iWater++)
			spr_worldbackground[0].draw(iWater << 5, 0, 512 + (iWater << 7), 0, 32, 32);

		DrawMessage();
		SDL_Flip(screen);

		int delay = WAITTIME - (SDL_GetTicks() - framestart);
		if (delay < 0)
			delay = 0;
		else if (delay > WAITTIME)
			delay = WAITTIME;

		SDL_Delay(delay);
	}

	return EDITOR_QUIT;
}

int editor_background()
{
	bool done = false;
	short iPage = 0;

    while (!done) {
		int framestart = SDL_GetTicks();

		//handle messages
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT: {
					done = true;
					break;
				}

            case SDL_KEYDOWN: {
					SDLKey key = event.key.keysym.sym;
                if (key >= SDLK_1 && key <= SDLK_2) {
						iPage = key - SDLK_1;
                } else {
						edit_mode = 0;
						return EDITOR_EDIT;
					}

					break;
				}

            case SDL_MOUSEBUTTONDOWN: {
                if (event.button.button == SDL_BUTTON_LEFT) {
						short iButtonX = event.button.x / TILESIZE;
						short iButtonY = event.button.y / TILESIZE;

						short iTileStyleOffset = ((iButtonX / 4) + (iPage * 5)) * WORLD_BACKGROUND_SPRITE_SET_SIZE;

						iButtonX %= 4;

                    if (iButtonX == 0) {
							if (iButtonY == 0)
								set_tile = 0;
							else if (iButtonY >= 1 && iButtonY < 15)
								set_tile = iButtonY + 1;
                    } else if (iButtonX == 1) {
							if (iButtonY == 0)
								set_tile = 1;
							else if (iButtonY >= 1 && iButtonY < 15)
								set_tile = iButtonY + 15;
                    } else if (iButtonX == 2) {
							if (iButtonY >= 0 && iButtonY < 15)
								set_tile = iButtonY + 30;
                    } else if (iButtonX == 3) {
							if (iButtonY >= 0 && iButtonY < 15)
								set_tile = iButtonY + 45;
						}

						set_tile += iTileStyleOffset;

						ignoreclick = true;
						edit_mode = 0;
						return EDITOR_EDIT;
					}

					break;
				}

				default:
					break;
			}
		}

		SDL_FillRect(screen, NULL, 0x0);

		spr_worldbackground[0].draw(0, 0, iPage * 640, 32, 640, 480);

		DrawMessage();
		SDL_Flip(screen);

		int delay = WAITTIME - (SDL_GetTicks() - framestart);
		if (delay < 0)
			delay = 0;
		else if (delay > WAITTIME)
			delay = WAITTIME;

		SDL_Delay(delay);
	}

	return EDITOR_QUIT;
}

int editor_stageforeground()
{
	bool done = false;
	short iForegroundScreen = 0;

    while (!done) {
		int framestart = SDL_GetTicks();

		//handle messages
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT: {
					done = true;
					break;
				}

            case SDL_KEYDOWN: {
					SDLKey key = event.key.keysym.sym;

                if (key >= SDLK_1 && key <= SDLK_4) {
						iForegroundScreen = key - SDLK_1;
                } else {
						edit_mode = 1;
						return EDITOR_EDIT;
					}

					break;
				}

            case SDL_MOUSEBUTTONDOWN: {
                if (event.button.button == SDL_BUTTON_LEFT) {
						short iButtonX = event.button.x / TILESIZE;
						short iButtonY = event.button.y / TILESIZE;

                    if (iButtonX >= 0 && iButtonX < 10) {
                        if (iButtonY >= 0 && iButtonY < 10) {
								set_tile = WORLD_FOREGROUND_STAGE_OFFSET + iButtonY * 10 + iButtonX + iForegroundScreen * 100;

								ignoreclick = true;
								edit_mode = 1;
								return EDITOR_EDIT;
							}
						}
					}

					break;
				}

				default:
					break;
			}
		}

		SDL_FillRect(screen, NULL, 0x0);

        for (short iRow = 0; iRow < 10; iRow++) {
            for (short iCol = 0; iCol < 10; iCol++) {
				spr_worldforegroundspecial[0].draw(iCol << 5, iRow << 5, 384, iForegroundScreen << 5, 32, 32);
			}
		}

		spr_worldforegroundspecial[0].draw(0, 0, 0, 0, 320, 320);

		DrawMessage();
		SDL_Flip(screen);

		int delay = WAITTIME - (SDL_GetTicks() - framestart);
		if (delay < 0)
			delay = 0;
		else if (delay > WAITTIME)
			delay = WAITTIME;

		SDL_Delay(delay);
	}

	return EDITOR_QUIT;
}

int editor_bridges()
{
	bool done = false;

    while (!done) {
		int framestart = SDL_GetTicks();

		//handle messages
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT: {
					done = true;
					break;
				}

            case SDL_KEYDOWN: {
					edit_mode = 1;
					return EDITOR_EDIT;

					break;
				}

            case SDL_MOUSEBUTTONDOWN: {
                if (event.button.button == SDL_BUTTON_LEFT) {
						short iButtonX = event.button.x / TILESIZE;
						short iButtonY = event.button.y / TILESIZE;

                    if (iButtonX >= 0 && iButtonX < 4) {
                        if (iButtonY >= 0 && iButtonY < 1) {
								set_tile = WORLD_BRIDGE_SPRITE_OFFSET + iButtonX;

								ignoreclick = true;
								edit_mode = 1;
								return EDITOR_EDIT;
							}
						}
					}

					break;
				}

				default:
					break;
			}
		}

		SDL_FillRect(screen, NULL, 0x0);

		spr_worldforegroundspecial[0].draw(0, 0, 320, 224, 128, 32);

		DrawMessage();
		SDL_Flip(screen);

		int delay = WAITTIME - (SDL_GetTicks() - framestart);
		if (delay < 0)
			delay = 0;
		else if (delay > WAITTIME)
			delay = WAITTIME;

		SDL_Delay(delay);
	}

	return EDITOR_QUIT;
}

int editor_structureforeground()
{
	bool done = false;

    while (!done) {
		int framestart = SDL_GetTicks();

		//handle messages
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT: {
					done = true;
					break;
				}

            case SDL_KEYDOWN: {
					edit_mode = 1;
					return EDITOR_EDIT;

					break;
				}

            case SDL_MOUSEBUTTONDOWN: {
                if (event.button.button == SDL_BUTTON_LEFT) {
						short iButtonX = event.button.x / TILESIZE;
						short iButtonY = event.button.y / TILESIZE;

                    if (iButtonY >= 0 && iButtonY < 15) {
                        if (iButtonX >= 0 && iButtonX < 12) {
								set_tile = WORLD_FOREGROUND_SPRITE_OFFSET + iButtonX + iButtonY * 12;

								ignoreclick = true;
								edit_mode = 1;
								return EDITOR_EDIT;
                        } else if (iButtonX >= 12 && iButtonX < 14) {
								set_tile = WORLD_FOREGROUND_SPRITE_ANIMATED_OFFSET + iButtonY + (iButtonX - 12) * 15;

								ignoreclick = true;
								edit_mode = 1;
								return EDITOR_EDIT;
							}
						}
					}

					break;
				}

				default:
					break;
			}
		}

		SDL_FillRect(screen, NULL, 0x0);

		spr_worldforeground[0].draw(0, 0, 0, 0, 416, 480);
		spr_worldforeground[0].draw(416, 0, 512, 0, 32, 480);

		DrawMessage();
		SDL_Flip(screen);

		int delay = WAITTIME - (SDL_GetTicks() - framestart);
		if (delay < 0)
			delay = 0;
		else if (delay > WAITTIME)
			delay = WAITTIME;

		SDL_Delay(delay);
	}

	return EDITOR_QUIT;
}

int editor_pathsprite()
{
	bool done = false;

    while (!done) {
		int framestart = SDL_GetTicks();

        //handle messages
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT: {
					done = true;
					break;
				}

            case SDL_KEYDOWN: {
					edit_mode = 4;
					return EDITOR_EDIT;
				}

            case SDL_MOUSEBUTTONDOWN: {
                if (event.button.button == SDL_BUTTON_LEFT) {
						short iButtonX = event.button.x / TILESIZE;
						short iButtonY = event.button.y / TILESIZE;

                    if (iButtonX >= 0 && iButtonX < 8) {
							if (iButtonY >= 0 && iButtonY < 6)
								set_tile = iButtonY + 1 + iButtonX * WORLD_PATH_SPRITE_SET_SIZE;
						}

						ignoreclick = true;
						edit_mode = 4;
						return EDITOR_EDIT;
					}

					break;
				}

				default:
					break;
			}
		}

		SDL_FillRect(screen, NULL, 0x0);

        for (short iPath = 0; iPath < 8; iPath++) {
			spr_worldpaths[0].draw(iPath << 5, 0, (iPath % 4) * 160, (iPath / 4) * 320, 32, 192);
		}

		DrawMessage();
		SDL_Flip(screen);

		int delay = WAITTIME - (SDL_GetTicks() - framestart);
		if (delay < 0)
			delay = 0;
		else if (delay > WAITTIME)
			delay = WAITTIME;

		SDL_Delay(delay);
	}

	return EDITOR_QUIT;
}

//Display stages over vehicles
//allow setting of stages on vehicles

int editor_vehicles()
{
    if (g_worldmap.iNumStages <= 0) {
		SetDisplayMessage(120, "No Stages", "You need to create", "stages before you", "can create vehicles");
		return EDITOR_EDIT;
	}

	mCurrentMenu = &mVehicleMenu;
	mCurrentMenu->ResetMenu();

	bool done = false;

	miVehicleStageField->Clear();

    for (short iStage = 0; iStage < g_worldmap.iNumStages; iStage++) {
		TourStop * ts = game_values.tourstops[iStage];
		char szStageName[256];
		sprintf(szStageName, "(%d) %s", iStage + 1, ts->szName);
		miVehicleStageField->Add(szStageName, iStage, "", false, false, true, ts->iStageType == 1 ? 24 : (ts->iMode >= 1000 ? ts->iMode - 975 : ts->iMode));
	}

	miVehicleStageField->SetKey(g_wvVehicleStamp.iActionId);

    while (!done) {
		int framestart = SDL_GetTicks();

		game_values.playerInput.ClearPressedKeys(1);

		MenuCodeEnum code = MENU_CODE_NONE;

		//handle messages
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT: {
					edit_mode = 5;
					game_values.playerInput.ResetKeys();
					return EDITOR_EDIT;
				}

            case SDL_KEYDOWN: {
                if (event.key.keysym.sym == SDLK_v) {
                    if (!mCurrentMenu->IsModifying()) {
							edit_mode = 5;
							game_values.playerInput.ResetKeys();
							return EDITOR_EDIT;
						}
					}

					break;
				}

            case SDL_MOUSEBUTTONDOWN: {
					short iButtonX = event.button.x;
					short iButtonY = event.button.y;

                if (event.button.button == SDL_BUTTON_LEFT) {
						code = mCurrentMenu->MouseClick(iButtonX, iButtonY);
					}

					break;
				}

				default:
					break;
			}

			game_values.playerInput.Update(event, 1);
		}

        if (MENU_CODE_NONE == code) {
			code = mCurrentMenu->SendInput(&game_values.playerInput);
		}

        if (MENU_CODE_EXIT_APPLICATION == code) {
			edit_mode = 5;
			game_values.playerInput.ResetKeys();
			return EDITOR_EDIT;
        } else if (MENU_CODE_VEHICLE_MIN_MOVES_CHANGED == code) {
			short iMaxMoves = miVehicleMaxMovesField->GetShortValue();
            if (miVehicleMinMovesField->GetShortValue() > iMaxMoves) {
				miVehicleMinMovesField->SetKey(iMaxMoves);
			}
        } else if (MENU_CODE_VEHICLE_MAX_MOVES_CHANGED == code) {
			short iMinMoves = miVehicleMinMovesField->GetShortValue();
            if (miVehicleMaxMovesField->GetShortValue() < iMinMoves) {
				miVehicleMaxMovesField->SetKey(iMinMoves);
			}
        } else if (MENU_CODE_CREATE_VEHICLE == code) {
			edit_mode = 5;
			ignoreclick = true;
			game_values.playerInput.ResetKeys();
			return EDITOR_EDIT;
		}

		drawmap(false, TILESIZE);
		menu_shade.draw(0, 0);

		mCurrentMenu->Update();
		mCurrentMenu->Draw();

        menu_font_small.drawRightJustified(640, 0, worldlist->current_name());

		DrawMessage();
		SDL_Flip(screen);

		int delay = WAITTIME - (SDL_GetTicks() - framestart);
		if (delay < 0)
			delay = 0;
		else if (delay > WAITTIME)
			delay = WAITTIME;

		SDL_Delay(delay);
	}

	return EDITOR_QUIT;
}

int editor_path()
{
	bool done = false;

	SDL_Rect r;
	r.x = 0;
	r.y = 0;
	r.w = 640;
	r.h = 480;

    while (!done) {
		int framestart = SDL_GetTicks();

		//handle messages
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT: {
					done = true;
					break;
				}

            case SDL_KEYDOWN: {
					edit_mode = 2;
					return EDITOR_EDIT;
				}

            case SDL_MOUSEBUTTONDOWN: {
                if (event.button.button == SDL_BUTTON_LEFT) {
						short iButtonX = event.button.x / TILESIZE;
						short iButtonY = event.button.y / TILESIZE;

                    if (iButtonX >= 0 && iButtonX <= 15 && iButtonY == 0) {
							set_tile = iButtonX + 1;

							ignoreclick = true;
							edit_mode = 2;
							return EDITOR_EDIT;
						}
					}

					break;
				}

				default:
					break;
			}
		}

		SDL_FillRect(screen, NULL, 0x0);
		spr_path.draw(0, 0, 0, 0, 480, 32);

		DrawMessage();
		SDL_Flip(screen);

		int delay = WAITTIME - (SDL_GetTicks() - framestart);
		if (delay < 0)
			delay = 0;
		else if (delay > WAITTIME)
			delay = WAITTIME;

		SDL_Delay(delay);
	}

	return EDITOR_QUIT;
}

void DisplayStageDetails(bool fForce, short iStageId, short iMouseX, short iMouseY)
{
	TourStop * ts = game_values.tourstops[iStageId];

	//If we're pointing to a new stage or no stage at all
    if (iStageId != iOldStageId || fForce) {
        if (ts->iStageType == 1) {
            if (sMapThumbnail) {
				SDL_FreeSurface(sMapThumbnail);
				sMapThumbnail = NULL;
			}
        } else {
			const char * pszMapName = ts->pszMapFile;

            if (pszMapName) {
                if (sMapThumbnail) {
					SDL_FreeSurface(sMapThumbnail);
					sMapThumbnail = NULL;
				}

                if (maplist->findexact(pszMapName, false)) {
					g_map->loadMap(maplist->currentFilename(), read_type_preview);
					sMapThumbnail = g_map->createThumbnailSurface(true);
                } else { //otherwise show a unknown map icon
					sMapThumbnail = IMG_Load(convertPath("gfx/leveleditor/leveleditor_mapnotfound.png").c_str());
				}
			}
		}
	}

	iOldStageId = iStageId;

	short iMode = ts->iMode;
	if (ts->iStageType == 1)
		iMode = 24;
	else if (ts->iMode >= 1000)
		iMode = ts->iMode - 975;  //Convert mode id from map file to internal id for special stage modes

	//Make sure we're displaying it on the screen
	if (iMouseX > 408)
		iMouseX = 408;

    if (iMode < GAMEMODE_LAST || (iMode >= 25 && iMode <= 27)) {
		if (iMouseY > 248)
			iMouseY = 248;

		spr_largedialog.draw(iMouseX, iMouseY, 0, 0, 116, 116);
		spr_largedialog.draw(iMouseX + 116, iMouseY, 140, 0, 116, 116);
		spr_largedialog.draw(iMouseX, iMouseY + 116, 0, 108, 116, 116);
		spr_largedialog.draw(iMouseX + 116, iMouseY + 116, 140, 108, 116, 116);
    } else if (iMode == 24) {
		//Make sure we're displaying it on the screen
		if (iMouseY > 392)
			iMouseY = 392;

		spr_largedialog.draw(iMouseX, iMouseY, 0, 0, 116, 44);
		spr_largedialog.draw(iMouseX + 116, iMouseY, 140, 0, 116, 44);
		spr_largedialog.draw(iMouseX, iMouseY + 44, 0, 180, 116, 44);
		spr_largedialog.draw(iMouseX + 116, iMouseY + 44, 140, 180, 116, 44);
	}

	menu_mode_large.draw(iMouseX + 16, iMouseY + 16, iMode << 5, 0, 32, 32);

	menu_font_small.drawChopRight(iMouseX + 52, iMouseY + 16, 164, ts->szName);

	char szPrint[128];
    if (iMode != 24) {
		sprintf(szPrint, "Goal: %d", ts->iGoal);
		menu_font_small.drawChopRight(iMouseX + 52, iMouseY + 34, 164, szPrint);

		sprintf(szPrint, "Points: %d", ts->iPoints);
		menu_font_small.drawChopRight(iMouseX + 16, iMouseY + 176, 100, szPrint);

		sprintf(szPrint, "End: %s", ts->fEndStage ? "Yes" : "No");
		menu_font_small.drawChopRight(iMouseX + 126, iMouseY + 176, 80, szPrint);

        for (short iBonus = 0; iBonus < ts->iNumBonuses; iBonus++) {
			WorldStageBonus * wsb = &ts->wsbBonuses[iBonus];
			spr_worlditemsplace.draw(iMouseX + iBonus * 20 + 16, iMouseY + 194, wsb->iWinnerPlace * 20, 0, 20, 20);

			short iBonusIcon = wsb->iBonus;
			gfxSprite * spr_icon = iBonusIcon < NUM_POWERUPS ? &spr_storedpowerupsmall : &spr_worlditemssmall;
			spr_icon->draw(iMouseX + iBonus * 20 + 18, iMouseY + 196, (iBonusIcon < NUM_POWERUPS ? iBonusIcon : iBonusIcon - NUM_POWERUPS) << 4, 0, 16, 16);
		}

        if (sMapThumbnail) {
			SDL_Rect rSrc = {0, 0, 160, 120};
			SDL_Rect rDst = {iMouseX + 16, iMouseY + 52, 160, 120};

			SDL_BlitSurface(sMapThumbnail, &rSrc, blitdest, &rDst);
		}
    } else {
		sprintf(szPrint, "Sort: %s", ts->iBonusType == 0 ? "Fixed" : "Random");
		menu_font_small.drawChopRight(iMouseX + 52, iMouseY + 34, 164, szPrint);

        for (short iBonus = 0; iBonus < ts->iNumBonuses; iBonus++) {
			short iBonusIcon = ts->wsbBonuses[iBonus].iBonus;
			gfxSprite * spr_icon = NULL;
			short iSrcX = 0, iSrcY = 0;

            if (iBonusIcon < NUM_POWERUPS) {
				spr_icon = &spr_storedpowerupsmall;
				iSrcX = iBonusIcon << 4;
				iSrcY = 0;
            } else if (iBonusIcon < NUM_POWERUPS + NUM_WORLD_POWERUPS) {
				spr_icon = &spr_worlditemssmall;
				iSrcX = (iBonusIcon - NUM_POWERUPS) << 4;
				iSrcY = 0;
            } else if (iBonusIcon < NUM_POWERUPS + NUM_WORLD_POWERUPS + 10) {
				spr_icon = &spr_worlditemssmall;
				iSrcX = (iBonusIcon - NUM_POWERUPS - NUM_WORLD_POWERUPS) << 4;
				iSrcY = 16;
            } else {
				spr_icon = &spr_worlditemssmall;
				iSrcX = (iBonusIcon - NUM_POWERUPS - NUM_WORLD_POWERUPS - 10) << 4;
				iSrcY = 32;
			}

			spr_icon->draw(iMouseX + iBonus * 20 + 18, iMouseY + 52, iSrcX, iSrcY, 16, 16);
		}
	}
}

int g_iNumGameModeSettings[GAMEMODE_LAST] = {2,2,3,4,3,10,9,6,2,1,3,5,3,3,0,22,6,4,3,4,4,3};

SDL_Rect rItemDst[NUM_WORLD_ITEMS];

void SetBonusString(char * szString, short iPlace, short iItem, short iStageType)
{
	char cType = 'p';

    if (iItem >= NUM_POWERUPS + NUM_WORLD_POWERUPS) {
		cType = 's';
		iItem -= NUM_POWERUPS + NUM_WORLD_POWERUPS;
    } else if (iItem >= NUM_POWERUPS) {
		cType = 'w';
		iItem -= NUM_POWERUPS;
	}

    if (iStageType == 0) {
		sprintf(szString, "%d%c%d", iPlace, cType, iItem);
    } else {
		sprintf(szString, "%c%d", cType, iItem);
	}
}

void TestAndSetBonusItem(TourStop * ts, short iPlace, short iButtonX, short iButtonY)
{
	short iMaxBonusesAllowed = 10;

	if (ts->iStageType == 1)
		iMaxBonusesAllowed = MAX_BONUS_CHESTS;

    if (ts->iNumBonuses < iMaxBonusesAllowed) {
		short iNumSelectableItems = NUM_WORLD_ITEMS;
		if (ts->iStageType == 0)
			iNumSelectableItems = NUM_POWERUPS + NUM_WORLD_POWERUPS;

        for (short iItem = 0; iItem < iNumSelectableItems; iItem++) {
			if (iButtonX >= rItemDst[iItem].x && iButtonX < rItemDst[iItem].w + rItemDst[iItem].x &&
                    iButtonY >= rItemDst[iItem].y && iButtonY < rItemDst[iItem].h + rItemDst[iItem].y) {
				//If this is a normal stage, then alert the player that they need to select the place for this item
                if (ts->iStageType == 0 && iPlace == 0) {
					SetDisplayMessage(120, "Use Number Keys", "Hover over item", "use keys 1 to 4", "to select bonus");
					return;
				}

				//Set the bonus item for the place selected
				ts->wsbBonuses[ts->iNumBonuses].iBonus = iItem;
				ts->wsbBonuses[ts->iNumBonuses].iWinnerPlace = iPlace - 1;

				SetBonusString(ts->wsbBonuses[ts->iNumBonuses].szBonusString, iPlace, iItem, ts->iStageType);

				ts->iNumBonuses++;

				break;
			}
		}
	}
}

static short iLastStageType = 0;

void AdjustBonuses(TourStop * ts)
{
	//No need to do anything if we were a stage and we're still a stage
	//(or a house and still a house)
	if (ts->iStageType == iLastStageType)
		return;

    if (ts->iStageType == 0) {
		//Remove any score bonuses
        for (short iBonus = 0; iBonus < ts->iNumBonuses; iBonus++) {
            if (ts->wsbBonuses[iBonus].iBonus >= NUM_POWERUPS + NUM_WORLD_POWERUPS) {
				ts->iNumBonuses--;
                for (short iRemoveBonus = iBonus; iRemoveBonus < ts->iNumBonuses; iRemoveBonus++) {
					ts->wsbBonuses[iRemoveBonus].iBonus = ts->wsbBonuses[iRemoveBonus + 1].iBonus;
					ts->wsbBonuses[iRemoveBonus].iWinnerPlace = ts->wsbBonuses[iRemoveBonus + 1].iWinnerPlace;
					strcpy(ts->wsbBonuses[iRemoveBonus].szBonusString, ts->wsbBonuses[iRemoveBonus + 1].szBonusString);
				}
			}
		}

		//Add places to bonuses
        for (short iBonus = 0; iBonus < ts->iNumBonuses; iBonus++) {
			if (ts->wsbBonuses[iBonus].iWinnerPlace < 0 || ts->wsbBonuses[iBonus].iWinnerPlace > 3)
				ts->wsbBonuses[iBonus].iWinnerPlace = 0;

			SetBonusString(ts->wsbBonuses[iBonus].szBonusString, ts->wsbBonuses[iBonus].iWinnerPlace + 1, ts->wsbBonuses[iBonus].iBonus, ts->iStageType);
		}
    } else if (ts->iStageType == 1) {
		//Cap the number of
		if (ts->iNumBonuses > MAX_BONUS_CHESTS)
			ts->iNumBonuses = MAX_BONUS_CHESTS;

        for (short iBonus = 0; iBonus < ts->iNumBonuses; iBonus++) {
			SetBonusString(ts->wsbBonuses[iBonus].szBonusString, 0, ts->wsbBonuses[iBonus].iBonus, ts->iStageType);
		}
	}

	iLastStageType = ts->iStageType;
}

void SaveStage(short iEditStage)
{
	//Set the number of game mode settings to the maximum so we write them all out
	game_values.tourstops[iEditStage]->fUseSettings = true;
	game_values.tourstops[iEditStage]->iNumUsedSettings = g_iNumGameModeSettings[game_values.tourstops[iEditStage]->iMode];

	//Copy the working values back into the structure that will be saved
	memcpy(&game_values.tourstops[iEditStage]->gmsSettings, &game_values.gamemodemenusettings, sizeof(GameModeSettings));

	if (game_values.tourstops[iEditStage]->iMode >= 25 && game_values.tourstops[iEditStage]->iMode <= 27)
		game_values.tourstops[iEditStage]->iMode += 975;
}

void EditStage(short iEditStage)
{
	memcpy(&game_values.gamemodemenusettings, &game_values.tourstops[iEditStage]->gmsSettings, sizeof(GameModeSettings));

	//Set fields to write data to the selected stage
	TourStop * ts = game_values.tourstops[iEditStage];

	miModeField->SetData(&ts->iMode, NULL, NULL);
	miNameField->SetData(ts->szName, 128);

	miPointsField->SetData(&ts->iPoints, NULL, NULL);
	miFinalStageField->SetData(NULL, NULL, &ts->fEndStage);

	if (!ts->pszMapFile)
		ts->pszMapFile = maplist->currentShortmapname();

	miMapField->SetMap(ts->pszMapFile, false);

	miBonusType->SetData(&ts->iBonusType, NULL, NULL);
	miBonusTextField[0]->SetData(ts->szBonusText[0], 128);
	miBonusTextField[1]->SetData(ts->szBonusText[1], 128);
	miBonusTextField[2]->SetData(ts->szBonusText[2], 128);
	miBonusTextField[3]->SetData(ts->szBonusText[3], 128);
	miBonusTextField[4]->SetData(ts->szBonusText[4], 128);

	ts->iBonusTextLines = 5;

	short iMode = game_values.tourstops[iEditStage]->iMode;
	short iStageType = game_values.tourstops[iEditStage]->iStageType;

	if (iMode >= 25 && iMode <= 27)
		iMode += 975;

	//Show fields applicable for this mode
	miPointsField->Show(iStageType == 0);
	miFinalStageField->Show(iStageType == 0);
	miMapField->Show(iStageType == 0);

	miBonusType->Show(iStageType == 1);
	miBonusTextField[0]->Show(iStageType == 1);
	miBonusTextField[1]->Show(iStageType == 1);
	miBonusTextField[2]->Show(iStageType == 1);
	miBonusTextField[3]->Show(iStageType == 1);
	miBonusTextField[4]->Show(iStageType == 1);

	miSpecialGoalField[0]->Show(iMode == 1000);
	miSpecialGoalField[1]->Show(iMode == 1001);
	miSpecialGoalField[2]->Show(iMode == 1002);

	miBonusItemsButton->SetPosition(430, iStageType == 0 ? 220 : 340);

    if (iStageType == 0 && iMode >= 0 && iMode < GAMEMODE_LAST) {
		miModeField->SetKey(iMode);

		miModeSettingsButton->Show(iMode != game_mode_owned);

        for (short iGameMode = 0; iGameMode < GAMEMODE_LAST; iGameMode++) {
			miGoalField[iGameMode]->Show(iMode == iGameMode);
		}

		miGoalField[iMode]->SetData(&game_values.tourstops[iEditStage]->iGoal, NULL, NULL);
		miGoalField[iMode]->SetKey(game_values.tourstops[iEditStage]->iGoal);
		miPointsField->SetKey(game_values.tourstops[iEditStage]->iPoints);
		miFinalStageField->SetKey(game_values.tourstops[iEditStage]->fEndStage ? 1 : 0);

		game_values.tourstops[iEditStage]->fUseSettings = true;
		game_values.tourstops[iEditStage]->iNumUsedSettings = g_iNumGameModeSettings[iMode];
    } else {
		//Show the settings button for boss mode
		miModeSettingsButton->Show(iMode == 1001);

        for (short iGameMode = 0; iGameMode < GAMEMODE_LAST; iGameMode++) {
			miGoalField[iGameMode]->Show(false);
		}

        if (iStageType == 1) { //Bonus House
			miModeField->SetKey(24);
        } else if (iMode >= 1000 && iMode <= 1002) { //Pipe, Boss and Boxes Game
			miModeField->SetKey(iMode - 975);
			miPointsField->SetKey(game_values.tourstops[iEditStage]->iPoints);
			miFinalStageField->SetKey(game_values.tourstops[iEditStage]->fEndStage ? 1 : 0);

			//Have to set this back again since the SetKey() above will set it to 25
			game_values.tourstops[iEditStage]->iMode = iMode;
		}
	}
}

void EnableStageMenu(bool fEnable)
{
	miNameField->Disable(!fEnable);
	miModeField->Disable(!fEnable);

	for (short iGameMode = 0; iGameMode < GAMEMODE_LAST; iGameMode++)
		miGoalField[iGameMode]->Disable(!fEnable);

	miSpecialGoalField[0]->Disable(!fEnable);
	miSpecialGoalField[1]->Disable(!fEnable);
	miSpecialGoalField[2]->Disable(!fEnable);

	miModeSettingsButton->Disable(!fEnable);

	miPointsField->Disable(!fEnable);
	miFinalStageField->Disable(!fEnable);
	miMapField->Disable(!fEnable);
	miBonusItemsButton->Disable(!fEnable);
	miBonusType->Disable(!fEnable);
	miBonusTextField[0]->Disable(!fEnable);
	miBonusTextField[1]->Disable(!fEnable);
	miBonusTextField[2]->Disable(!fEnable);
	miBonusTextField[3]->Disable(!fEnable);
	miBonusTextField[4]->Disable(!fEnable);
	miDeleteStageButton->Disable(!fEnable);
}

void NewStage(short * iEditStage)
{
	TourStop * ts = new TourStop();

	ts->iStageType = 0;

	ts->szBonusText[0][0] = 0;
	ts->szBonusText[1][0] = 0;
	ts->szBonusText[2][0] = 0;
	ts->szBonusText[3][0] = 0;
	ts->szBonusText[4][0] = 0;

	ts->pszMapFile = maplist->currentShortmapname();
	ts->iMode = 0;

	ts->fUseSettings = true;
	ts->iNumUsedSettings = g_iNumGameModeSettings[0];

	ts->iGoal = 10;
	ts->iPoints = 1;

	ts->iBonusType = 0;
	ts->iNumBonuses = 0;

	sprintf(ts->szName, "Tour Stop %d", game_values.tourstoptotal + 1);

	ts->fEndStage = false;

	//Copy in default values first
	memcpy(&ts->gmsSettings, &game_values.gamemodemenusettings, sizeof(GameModeSettings));

	game_values.tourstops.push_back(ts);
	game_values.tourstoptotal++;
	g_worldmap.iNumStages++;

	*iEditStage = game_values.tourstoptotal - 1;
	EditStage(*iEditStage);
}

int editor_stage()
{
	mCurrentMenu = &mStageSettingsMenu;
	mCurrentMenu->ResetMenu();

	bool done = false;
	short iStageDisplay = -1;
	bool fForceStageDisplay = false;
	short iEditStage = -1;

	SDL_Rect rStageBonusDst[10];
	SDL_Rect rHouseBonusDst[MAX_BONUS_CHESTS];

	short iColCount = 0;
	short iRowCount = 0;
    for (short iItem = 0; iItem < NUM_WORLD_ITEMS; iItem++) {
		rItemDst[iItem].x = 16 + iColCount * 48;
		rItemDst[iItem].y = 16 + iRowCount * 48;
		rItemDst[iItem].w = 32;
		rItemDst[iItem].h = 32;

        if (++iColCount > 12) {
			iColCount = 0;
			iRowCount++;
		}
	}

    for (short iStageBonus = 0; iStageBonus < 10; iStageBonus++) {
		rStageBonusDst[iStageBonus].x = 35 + iStageBonus * 58;
		rStageBonusDst[iStageBonus].y = 360;
		rStageBonusDst[iStageBonus].w = 32;
		rStageBonusDst[iStageBonus].h = 32;
	}

	short iStartItemX = (640 - (MAX_BONUS_CHESTS * 58 - 10)) >> 1;

    for (short iHouseBonus = 0; iHouseBonus < MAX_BONUS_CHESTS; iHouseBonus++) {
		rHouseBonusDst[iHouseBonus].x = iStartItemX + iHouseBonus * 58;
		rHouseBonusDst[iHouseBonus].y = 360;
		rHouseBonusDst[iHouseBonus].w = 32;
		rHouseBonusDst[iHouseBonus].h = 32;
	}

    while (!done) {
		int framestart = SDL_GetTicks();

		//Reset the keys that were down the last frame
		game_values.playerInput.ClearPressedKeys(1);

		MenuCodeEnum code = MENU_CODE_NONE;

		//handle messages
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT: {
					done = true;
					break;
				}

            case SDL_KEYDOWN: {
					//Do not allow saving world by pressing 's' key
					//World data structures are not in the correct state to be saved
					//until exiting the stage editor menu in (MENU_CODE_EXIT_APPLICATION == code) below

                if (iEditStage == -1 && event.key.keysym.sym == SDLK_n) {
						NewStage(&iEditStage);
                } else if ((event.key.keysym.sym == SDLK_ESCAPE || event.key.keysym.sym == SDLK_e) && iEditStage == -1) {
                    if (g_worldmap.iNumStages == 0) {
							edit_mode = 1;
                    } else if (set_tile < 6 || set_tile >= g_worldmap.iNumStages + 6) {
							set_tile = 6;
							edit_mode = 9;
                    } else {
							edit_mode = 9;  //change to edit mode using stages
						}

						return EDITOR_EDIT;
                } else if (mCurrentMenu == &mBonusItemPicker && event.key.keysym.sym >= SDLK_1 && event.key.keysym.sym <= SDLK_4) {
						TourStop * ts = game_values.tourstops[iEditStage];
                    if (ts->iStageType == 0) {
							short iPlace = event.key.keysym.sym - SDLK_1 + 1;

							int iMouseX, iMouseY;
							SDL_GetMouseState(&iMouseX, &iMouseY);

							TestAndSetBonusItem(ts, iPlace, iMouseX, iMouseY);
						}
                } else if ((event.key.keysym.sym == SDLK_PAGEUP && iEditStage > 0) ||
                          (event.key.keysym.sym == SDLK_PAGEDOWN && iEditStage < g_worldmap.iNumStages - 1)) {
                    if (iEditStage != -1 && mCurrentMenu == &mStageSettingsMenu) {
							SaveStage(iEditStage);

							if (event.key.keysym.sym == SDLK_PAGEUP)
								iEditStage--;
							else if (event.key.keysym.sym == SDLK_PAGEDOWN)
								iEditStage++;

							EditStage(iEditStage);
							mCurrentMenu->ResetMenu();

							code = MENU_CODE_MODE_CHANGED;

							mModeOptionsMenu->Refresh();
						}
					}

					break;
				}

            case SDL_MOUSEBUTTONDOWN: {
					short iTileX = event.button.x / TILESIZE;
					short iTileY = event.button.y / TILESIZE;
					short iButtonX = event.button.x;
					short iButtonY = event.button.y;

                if (event.button.button == SDL_BUTTON_LEFT) {
						//Stages
                    if (iEditStage == -1) {
                        if (iTileX >= 0 && iTileX < g_worldmap.iNumStages - (iTileY * 20) && iTileY >= 0 && iTileY <= (g_worldmap.iNumStages - 1) / 20) {
								set_tile = iTileX + (iTileY * 20) + 6;

								edit_mode = 9;  //change to edit mode using warps

								//The user must release the mouse button before trying to add a tile
								ignoreclick = true;

								return EDITOR_EDIT;
							}
							//New stage button
                        else if (iButtonX >= 256 && iButtonX < 384 && iButtonY >= 420 && iButtonY < 452) {
								NewStage(&iEditStage);
							}
                    } else if (mCurrentMenu == &mBonusItemPicker) {
							TourStop * ts = game_values.tourstops[iEditStage];

							//See if we clicked an item and add it if we did
							TestAndSetBonusItem(ts, 0, iButtonX, iButtonY);

							//See if we clicked an already added item and remove it
                        for (short iRemoveItem = 0; iRemoveItem < ts->iNumBonuses; iRemoveItem++) {
								SDL_Rect * rects = rStageBonusDst;

								if (ts->iStageType == 1)
									rects = rHouseBonusDst;

								if (iButtonX >= rects[iRemoveItem].x && iButtonX < rects[iRemoveItem].w + rects[iRemoveItem].x &&
                                    iButtonY >= rects[iRemoveItem].y && iButtonY < rects[iRemoveItem].h + rects[iRemoveItem].y) {
                                for (short iAdjust = iRemoveItem; iAdjust < ts->iNumBonuses - 1; iAdjust++) {
										ts->wsbBonuses[iAdjust].iBonus = ts->wsbBonuses[iAdjust + 1].iBonus;
										ts->wsbBonuses[iAdjust].iWinnerPlace = ts->wsbBonuses[iAdjust + 1].iWinnerPlace;
										strcpy(ts->wsbBonuses[iAdjust].szBonusString, ts->wsbBonuses[iAdjust + 1].szBonusString);
									}

									ts->iNumBonuses--;

									break;
								}
							}
                    } else {
							code = mCurrentMenu->MouseClick(iButtonX, iButtonY);
						}
                } else if (event.button.button == SDL_BUTTON_RIGHT) {
                    if (iEditStage == -1) {
                        if (iTileX >= 0 && iTileX < g_worldmap.iNumStages - (iTileY * 20) && iTileY >= 0 && iTileY <= (g_worldmap.iNumStages - 1) / 20) {
								iEditStage = iTileX + (iTileY * 20);
								EditStage(iEditStage);

								code = MENU_CODE_MODE_CHANGED;

								mModeOptionsMenu->Refresh();
							}
						}
					}

					break;
				}

            case SDL_MOUSEMOTION: {
					iStageDisplay = -1;

                if (iEditStage == -1) {
//						if (event.button.x >= 0 && event.button.y >= 0)
						{
							short iMouseX = event.button.x / TILESIZE;
							short iMouseY = event.button.y / TILESIZE;

                        if (iMouseX >= 0 && iMouseX < g_worldmap.iNumStages - (iMouseY * 20) && iMouseY >= 0 && iMouseY <= (g_worldmap.iNumStages - 1) / 20) {
								iStageDisplay = iMouseX + (iMouseY * 20);
							}
						}
					}
				}

				default:
					break;
			}

			game_values.playerInput.Update(event, 1);
		}

        if (iEditStage >= 0) {
            if (MENU_CODE_NONE == code) {
				code = mCurrentMenu->SendInput(&game_values.playerInput);
			}

            if (MENU_CODE_EXIT_APPLICATION == code) {
				//Save the current stage
				SaveStage(iEditStage);

				//We are no longer working on a specific stage
				iEditStage = -1;
            } else if (MENU_CODE_MODE_CHANGED == code) {
				short iMode = miModeField->GetShortValue();

				miPointsField->Show(iMode != 24);
				miFinalStageField->Show(iMode != 24);

				miMapField->Show(iMode != 24);

				miBonusType->Show(iMode == 24);
				miBonusTextField[0]->Show(iMode == 24);
				miBonusTextField[1]->Show(iMode == 24);
				miBonusTextField[2]->Show(iMode == 24);
				miBonusTextField[3]->Show(iMode == 24);
				miBonusTextField[4]->Show(iMode == 24);

				miSpecialGoalField[0]->Show(iMode == 25);
				miSpecialGoalField[1]->Show(iMode == 26);
				miSpecialGoalField[2]->Show(iMode == 27);

				miBonusItemsButton->SetPosition(430, iMode != 24 ? 220 : 340);

                if (iMode >= 0 && iMode < GAMEMODE_LAST) {
					miModeSettingsButton->Show(iMode != game_mode_owned);

                    for (short iGameMode = 0; iGameMode < GAMEMODE_LAST; iGameMode++) {
						miGoalField[iGameMode]->Show(iMode == iGameMode);
					}

					miGoalField[iMode]->SetData(&game_values.tourstops[iEditStage]->iGoal, NULL, NULL);
					miGoalField[iMode]->SetValues();

					game_values.tourstops[iEditStage]->iStageType = 0;

					game_values.tourstops[iEditStage]->fUseSettings = true;
					game_values.tourstops[iEditStage]->iNumUsedSettings = g_iNumGameModeSettings[iMode];
                } else {
					//Show the settings button for boss mode
					miModeSettingsButton->Show(iMode == 26);

					for (short iGameMode = 0; iGameMode < GAMEMODE_LAST; iGameMode++)
						miGoalField[iGameMode]->Show(false);

                    if (iMode == 24) {
						game_values.tourstops[iEditStage]->iStageType = 1;
						game_values.tourstops[iEditStage]->iBonusTextLines = 5;
                    } else if (iMode >= 25 && iMode <= 27) {
						game_values.tourstops[iEditStage]->iStageType = 0;
					}
				}

				//Removes bonuses if we went from a stage to a bonus house
				//(and there were more than the max bonuses for a house)
				AdjustBonuses(game_values.tourstops[iEditStage]);

            } else if (MENU_CODE_TO_MODE_SETTINGS_MENU == code) {
				bool fModeFound = false;
                for (short iGameMode = 0; iGameMode < GAMEMODE_LAST; iGameMode++) {
                    if (miGoalField[iGameMode]->IsVisible()) {
						mCurrentMenu = mModeOptionsMenu->GetOptionsMenu(iGameMode);
						mCurrentMenu->ResetMenu();
						fModeFound = true;
						break;
					}
				}

				//Look to see if this is the boss mode and go to boss settings
                if (!fModeFound) {
                    if (miSpecialGoalField[1]->IsVisible()) {
						mCurrentMenu = mModeOptionsMenu->GetBossOptionsMenu();
						mCurrentMenu->ResetMenu();
					}
				}
            } else if (MENU_CODE_HEALTH_MODE_START_LIFE_CHANGED == code) {
				mModeOptionsMenu->HealthModeStartLifeChanged();
            } else if (MENU_CODE_HEALTH_MODE_MAX_LIFE_CHANGED == code) {
				mModeOptionsMenu->HealthModeMaxLifeChanged();
            } else if (MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS == code) {
				mCurrentMenu = &mStageSettingsMenu;
				mCurrentMenu->ResetMenu();
            } else if (MENU_CODE_MAP_CHANGED == code) {
				game_values.tourstops[iEditStage]->pszMapFile = maplist->currentShortmapname();
				fForceStageDisplay = true;
            } else if (MENU_CODE_TO_BONUS_PICKER_MENU == code) {
				mCurrentMenu = &mBonusItemPicker;
				mCurrentMenu->ResetMenu();
            } else if (MENU_CODE_DELETE_STAGE_BUTTON == code) {
				miDeleteStageDialogImage->Show(true);
				miDeleteStageDialogAreYouText->Show(true);
				miDeleteStageDialogSureText->Show(true);
				miDeleteStageDialogYesButton->Show(true);
				miDeleteStageDialogNoButton->Show(true);

				EnableStageMenu(false);

				mStageSettingsMenu.RememberCurrent();

				mStageSettingsMenu.SetHeadControl(miDeleteStageDialogNoButton);
				mStageSettingsMenu.SetCancelCode(MENU_CODE_DELETE_STAGE_NO);
				mStageSettingsMenu.ResetMenu();
            } else if (MENU_CODE_DELETE_STAGE_YES == code || MENU_CODE_DELETE_STAGE_NO == code) {
				miDeleteStageDialogImage->Show(false);
				miDeleteStageDialogAreYouText->Show(false);
				miDeleteStageDialogSureText->Show(false);
				miDeleteStageDialogYesButton->Show(false);
				miDeleteStageDialogNoButton->Show(false);

				mStageSettingsMenu.SetHeadControl(miNameField);
				mStageSettingsMenu.SetCancelCode(MENU_CODE_EXIT_APPLICATION);
				mStageSettingsMenu.RestoreCurrent();

				//Yes was selected to delete this stage
                if (MENU_CODE_DELETE_STAGE_YES == code) {
					//Scan the grid of stages and remove any references to this stage
					//and decrement stage numbers greater than this stage
                    for (short iRow = 0; iRow < iWorldHeight; iRow++) {
                        for (short iCol = 0; iCol < iWorldWidth; iCol++) {
                            if (g_worldmap.tiles[iCol][iRow].iType == iEditStage + 6) {
								g_worldmap.tiles[iCol][iRow].iType = 0;
                            } else if (g_worldmap.tiles[iCol][iRow].iType > iEditStage + 6) {
								g_worldmap.tiles[iCol][iRow].iType--;
							}
						}
					}

					//Scan vehicles and remove references to deleted stage
					std::vector<WorldVehicle*>::iterator itrVehicle = vehiclelist.begin(), limVehicle = vehiclelist.end();
                    while (itrVehicle != limVehicle) {
						WorldVehicle * vehicle = *itrVehicle;
                        if (vehicle->iActionId == iEditStage) {
							RemoveVehicleFromTile(vehicle->iCurrentTileX, vehicle->iCurrentTileY);
                        } else if (vehicle->iActionId > iEditStage) {
							vehicle->iActionId--;
						}

						itrVehicle++;
					}

					//Remove stage from tourstops vector
					std::vector<TourStop*>::iterator itr = game_values.tourstops.begin(), lim = game_values.tourstops.end();

					short iIndex = 0;
                    while (itr != lim) {
                        if (iIndex == iEditStage) {
							delete (*itr);

							game_values.tourstops.erase(itr);
							game_values.tourstoptotal--;
							g_worldmap.iNumStages--;

							break;
						}

						++itr;
						++iIndex;
					}

					iEditStage = -1;
					mCurrentMenu = &mStageSettingsMenu;
					mCurrentMenu->ResetMenu();
				}

				EnableStageMenu(true);
			}
		}

		drawmap(false, TILESIZE);
		menu_shade.draw(0, 0);

		int color = SDL_MapRGB(blitdest->format, 0, 0, 255);

        if (iEditStage == -1) {
            for (short iStage = 0; iStage < g_worldmap.iNumStages; iStage++) {
				short ix = (iStage % 20) << 5;
				short iy = ((iStage / 20) << 5);

				SDL_Rect r = {ix, iy, 32, 32};
				SDL_FillRect(blitdest, &r, color);

				spr_worldforegroundspecial[0].draw(ix, iy, (iStage % 10) << 5, (iStage / 10) << 5, 32, 32);
			}

            if (iStageDisplay >= 0) {
				int iMouseX, iMouseY;
				SDL_GetMouseState(&iMouseX, &iMouseY);

				DisplayStageDetails(fForceStageDisplay, iStageDisplay, iMouseX, iMouseY);
				fForceStageDisplay = false;
			}

			//Display New button
			spr_selectfield.draw(256, 420, 0, 0, 64, 32);
			spr_selectfield.draw(320, 420, 448, 0, 64, 32);

			menu_font_large.drawCentered(320, 425, "New Stage");

			menu_font_small.draw(0, 480 - menu_font_small.getHeight(), "[LMB] Select Stage, [RMB] Edit Stage, [n] New Stage");
        } else {
			mCurrentMenu->Update();
			mCurrentMenu->Draw();

            if (mCurrentMenu != &mBonusItemPicker) {
				short ix = 20;
				short iy = 20;

				SDL_Rect r = {ix, iy, 32, 32};
				SDL_FillRect(blitdest, &r, color);

				spr_worldforegroundspecial[0].draw(ix, iy, (iEditStage % 10) << 5, (iEditStage / 10) << 5, 32, 32);
			}
		}

        if (mCurrentMenu == &mBonusItemPicker) {
			TourStop * ts = game_values.tourstops[iEditStage];

			//Game powerups
            for (short iItem = 0; iItem < NUM_POWERUPS; iItem++) {
				spr_storedpoweruplarge.draw(rItemDst[iItem].x, rItemDst[iItem].y, iItem << 5, 0, 32, 32);
			}

			//World Powerups
            for (short iWorldItem = 0; iWorldItem < NUM_WORLD_POWERUPS; iWorldItem++) {
				spr_worlditems.draw(rItemDst[iWorldItem + NUM_POWERUPS].x, rItemDst[iWorldItem + NUM_POWERUPS].y, iWorldItem << 5, 0, 32, 32);
			}

			//Score Bonuses
            if (ts->iStageType == 1) {
                for (short iScoreBonus = 0; iScoreBonus < NUM_WORLD_SCORE_BONUSES; iScoreBonus++) {
					spr_worlditems.draw(rItemDst[iScoreBonus + NUM_POWERUPS + NUM_WORLD_POWERUPS].x, rItemDst[iScoreBonus + NUM_POWERUPS + NUM_WORLD_POWERUPS].y, iScoreBonus < 10 ? iScoreBonus << 5 : (iScoreBonus - 10) << 5, iScoreBonus < 10 ? 32 : 64, 32, 32);
				}
			}

			//Draw background container
			spr_worldpopup.draw(0, 344, 0, 0, 320, 64);
			spr_worldpopup.draw(320, 344, 192, 0, 320, 64);

			SDL_Rect * rects = rStageBonusDst;

			if (ts->iStageType == 1)
				rects = rHouseBonusDst;

            for (short iPickedItem = 0; iPickedItem < ts->iNumBonuses; iPickedItem++) {
				short iBonus = ts->wsbBonuses[iPickedItem].iBonus;
				short iPlace = ts->wsbBonuses[iPickedItem].iWinnerPlace;

				//Draw place behind bonus
				if (ts->iStageType == 0)
					spr_worldpopup.draw(rects[iPickedItem].x - 8, rects[iPickedItem].y - 8, iPlace * 48, 256, 48, 48);

                if (iBonus >= NUM_POWERUPS + NUM_WORLD_POWERUPS) {
					short iBonusIndex = iBonus - NUM_POWERUPS - NUM_WORLD_POWERUPS;
					spr_worlditems.draw(rects[iPickedItem].x, rects[iPickedItem].y, iBonusIndex < 10 ? iBonusIndex << 5 : (iBonusIndex - 10) << 5, iBonusIndex < 10 ? 32 : 64, 32, 32);
                } else if (iBonus >= NUM_POWERUPS) {
					spr_worlditems.draw(rects[iPickedItem].x, rects[iPickedItem].y, (iBonus - NUM_POWERUPS) << 5, 0, 32, 32);
                } else {
					spr_storedpoweruplarge.draw(rects[iPickedItem].x, rects[iPickedItem].y, iBonus << 5, 0, 32, 32);
				}
			}

			if (ts->iStageType == 0)
				menu_font_small.draw(0, 480 - menu_font_small.getHeight(), "[1-4] Select Items, [LMB] Remove Items");
			else
				menu_font_small.draw(0, 480 - menu_font_small.getHeight(), "[LMB] Select Items, [LMB] Remove Items");
		}

        menu_font_small.drawRightJustified(640, 0, worldlist->current_name());

		DrawMessage();
		SDL_Flip(screen);

		int delay = WAITTIME - (SDL_GetTicks() - framestart);
		if (delay < 0)
			delay = 0;
		else if (delay > WAITTIME)
			delay = WAITTIME;

		SDL_Delay(delay);
	}

	return EDITOR_QUIT;
}

int display_help()
{
	//unsigned int currentChar = 0;

	drawmap(false, TILESIZE);
	menu_shade.draw(0, 0);
	menu_font_large.drawCentered(320, 15, "Help");

	int offsety = 55;
	int offsetx = 30;
	menu_font_small.draw(offsetx, offsety, "Modes:");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "[1] - Water Mode");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "[2] - Land Mode");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "[3] - Stage Objects Mode");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "[4] - Path Mode");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "[5] - Objects Mode");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "[6] - Bridges Mode");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "[p] - Connection Mode");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "[w] - Warp Mode");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "[v] - Vehicle");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "      [c] - Copy Vehicle");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "[t] - Start and Doors");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "[b] - Vehicle Boundaries");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "[i] - Initial Powerups");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "[e] - Edit Stages");
	offsety += menu_font_small.getHeight() + 20;

	menu_font_small.draw(offsetx, offsety, "File:");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "[n] - New World");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "[s] - Save World");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "[shift] + [s] - Save As");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "[f] - Find World");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "[shift] + [f] - New Search");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "[pageup] - Go To Previous World");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "[pagedown] - Go To Next World");

	offsetx = 300;
	offsety = 55;

	menu_font_small.draw(offsetx, offsety, "Place Tiles:");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "[Left Mouse Button] - Place Item");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "[Right Mouse Button] - Remove Item");
	offsety += menu_font_small.getHeight() + 20;

	/*
	menu_font_small.draw(offsetx, offsety, "Move Mode:");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "[Right Mouse Button] - Select Area");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "[Left Mouse Button] - Unselect Area");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "Select And Drag - Move Selections");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "Hold [shift] - Multiple Selections");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "Hold [ctrl] - Freehand Selections");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "[delete] - Delete Selection");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "[c] - Copy Selection");
	offsety += menu_font_small.getHeight() + 20;
	*/

	menu_font_small.draw(offsetx, offsety, "Miscellaneous:");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "[r] - Change Music Category");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "[Arrow Keys] - Navigate World");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "[a] - Automatic Path/Land");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "[k] - Resize World");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "[ctrl] + [delete] - Clear All");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "[insert] - Screenshot");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "[alt] + [enter] - Full Screen/Window");
	offsety += menu_font_small.getHeight() + 2;
	menu_font_small.draw(offsetx, offsety, "[space] - Toggle Stage Previews");

	SDL_Flip(screen);

    while (true) {
		int framestart = SDL_GetTicks();

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

		int delay = WAITTIME - (SDL_GetTicks() - framestart);
		if (delay < 0)
			delay = 0;
		else if (delay > WAITTIME)
			delay = WAITTIME;

		SDL_Delay(delay);
	}

	return 0;
}

int save_as()
{
	char fileName[FILEBUFSIZE] = "";
	char mapLocation[FILEBUFSIZE] = "worlds/";

    if (dialog("Save As", "Enter name:", fileName, 64)) {
        worldlist->add(strcat(mapLocation, strcat(fileName, ".txt")));
        worldlist->find(fileName);
        game_values.worldindex = worldlist->GetCurrentIndex();
		savecurrentworld();
		loadcurrentworld();
	}

	return 0;
}

bool dialog(const char * title, const char * instructions, char * input, int inputsize)
{
	unsigned int currentChar = 0;

	drawmap(false, TILESIZE);
	menu_shade.draw(0, 0);
	spr_dialog.draw(224, 176, 0, 0, 192, 128);
	menu_font_large.drawCentered(320, 200, title);
	menu_font_small.draw(240, 235, instructions);
    menu_font_small.drawRightJustified(640, 0, worldlist->current_name());
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
							menu_shade.draw(0, 0);
							spr_dialog.draw(224, 176, 0, 0, 192, 128);
							menu_font_large.drawCentered(320, 200, title);
							menu_font_small.draw(240, 235, instructions);
							menu_font_small.draw(240, 255, input);
                        menu_font_small.drawRightJustified(640, 0, worldlist->current_name());
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
							menu_shade.draw(0, 0);
							spr_dialog.draw(224, 176, 0, 0, 192, 128);
							menu_font_large.drawCentered(320, 200, title);
							menu_font_small.draw(240, 235, instructions);
							menu_font_small.draw(240, 255, input);
                        menu_font_small.drawRightJustified(640, 0, worldlist->current_name());
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

        if (worldlist->find(findstring)) {
            game_values.worldindex = worldlist->GetCurrentIndex();
			loadcurrentworld();
		}
	}

	return 0;
}

int clear_world()
{
	g_worldmap.Clear();
	updateworldsurface();

	printf("World Cleared\n");
	return 0;
}

void loadcurrentworld()
{
	g_worldmap.Load(TILESIZE);
	ReadVehiclesIntoEditor();
	ReadWarpsIntoEditor();

	draw_offset_col = 0;
	draw_offset_row = 0;

	rectSrcSurface.x = 0;
	rectSrcSurface.y = 0;
	fNeedBlackBackground = false;

	g_worldmap.GetWorldSize(&iWorldWidth, &iWorldHeight);

    if (iWorldWidth >= 20) {
		rectSrcSurface.w = 640;
		draw_offset_x = 0;
    } else {
		rectSrcSurface.w = iWorldWidth * TILESIZE;
		draw_offset_x = (640 - iWorldWidth * TILESIZE) >> 1;
		fNeedBlackBackground = true;
	}

    if (iWorldHeight >= 15) {
		rectSrcSurface.h = 480;
		draw_offset_y = 0;
    } else {
		rectSrcSurface.h = iWorldHeight * TILESIZE;
		draw_offset_y = (480 - iWorldHeight * TILESIZE) >> 1;
		fNeedBlackBackground = true;
	}

	rectDstSurface.x = draw_offset_x;
	rectDstSurface.y = draw_offset_y;

	updateworldsurface();
}

int savecurrentworld()
{
	SetDisplayMessage(60, "Saved", "Your world has", "been saved.", "");

	WriteVehiclesIntoWorld();
	WriteWarpsIntoWorld();
	g_worldmap.Save();
	return 0;
}

void SetDisplayMessage(short iTime,
                       const char * szTitle,
                       const char * szLine1,
                       const char * szLine2,
                       const char * szLine3)
{
	g_messagedisplaytimer = iTime;
	g_szMessageTitle = szTitle;
	g_szMessageLine[0] = szLine1;
	g_szMessageLine[1] = szLine2;
	g_szMessageLine[2] = szLine3;
}

int findcurrentstring()
{
    if (findstring[0] != '\0') {
        if (worldlist->find(findstring)) {
            game_values.worldindex = worldlist->GetCurrentIndex();
			loadcurrentworld();
		}
	}

	return 0;
}

int new_world()
{
	char fileName[FILEBUFSIZE] = "";
	char worldLocation[FILEBUFSIZE] = "worlds/";
	char szWidth[5], szHeight[5];

	if (dialog("New World", "Enter name:", fileName, 64) &&
		dialog("New World", "Width:", szWidth, 4) &&
            dialog("New World", "Height:", szHeight, 4)) {
		short iWidth = atoi(szWidth);
		short iHeight = atoi(szHeight);

		if (iWidth < 1)
			iWidth = 1;

		if (iHeight < 1)
			iHeight = 1;

		std::vector<WorldVehicle*>::iterator itrVehicle = vehiclelist.begin(), limVehicle = vehiclelist.end();
        while (itrVehicle != limVehicle) {
			delete (*itrVehicle);
			itrVehicle++;
		}

		vehiclelist.clear();

		std::vector<WorldWarp*>::iterator itrWarp = warplist.begin(), limWarp = warplist.end();
        while (itrWarp != limWarp) {
			delete (*itrWarp);
			itrWarp++;
		}

		warplist.clear();

		g_worldmap.New(iWidth, iHeight);
        worldlist->add(strcat(worldLocation, strcat(fileName, ".txt")));
        worldlist->find(fileName);
        game_values.worldindex = worldlist->GetCurrentIndex();
		savecurrentworld();
		loadcurrentworld();
	}

	return 0;
}


int resize_world()
{
	char szWidth[5], szHeight[5];

	if (dialog("Resize World", "Width:", szWidth, 4) &&
            dialog("Resize World", "Height:", szHeight, 4)) {
		short iWidth = atoi(szWidth);
		short iHeight = atoi(szHeight);

		if (iWidth < 1)
			iWidth = 1;

		if (iHeight < 1)
			iHeight = 1;

		std::vector<WorldVehicle*>::iterator itrVehicle = vehiclelist.begin(), limVehicle = vehiclelist.end();
        while (itrVehicle != limVehicle) {
			if ((*itrVehicle)->iCurrentTileX >= iWidth || (*itrVehicle)->iCurrentTileY >= iHeight)
				RemoveVehicleFromTile((*itrVehicle)->iCurrentTileX, (*itrVehicle)->iCurrentTileY);

			itrVehicle++;
		}

		std::vector<WorldWarp*>::iterator itrWarp = warplist.begin(), limWarp = warplist.end();
        while (itrWarp != limWarp) {
			delete (*itrWarp);

			itrWarp = warplist.erase(itrWarp);
			limWarp = warplist.end();

			itrWarp++;
		}

		g_worldmap.Resize(iWidth, iHeight);

		savecurrentworld();
		loadcurrentworld();
	}

	return 0;
}

//take screenshots in full and thumbnail sizes
void takescreenshot()
{
	short iTileSizes[3] = {32, 16, 8};

    for (short iScreenshotSize = 0; iScreenshotSize < 3; iScreenshotSize++) {
		short iTileSize = iTileSizes[iScreenshotSize];
		g_worldmap.Load(iTileSize);

		short w, h;
		g_worldmap.GetWorldSize(&w, &h);

		//Draw most of the world to screenshot
		SDL_Surface * sScreenshot = SDL_CreateRGBSurface(screen->flags, iTileSize * w, iTileSize * h, screen->format->BitsPerPixel, 0, 0, 0, 0);
		blitdest = sScreenshot;

		g_worldmap.DrawMapToSurface(sScreenshot);

		//Draw vehicles to screenshot
		std::vector<WorldVehicle*>::iterator itr = vehiclelist.begin(), lim = vehiclelist.end();
        while (itr != lim) {
			WorldVehicle * vehicle = *itr;

			short ix = vehicle->iCurrentTileX * iTileSize;
			short iy = vehicle->iCurrentTileY * iTileSize;

			spr_worldvehicle[iScreenshotSize].draw(ix, iy, vehicle->iDrawDirection * iTileSize, vehicle->iDrawSprite * iTileSize, iTileSize, iTileSize);

			itr++;
		}

		//Draw warps to screenshot
		std::vector<WorldWarp*>::iterator itrWarp = warplist.begin(), limWarp = warplist.end();
        while (itrWarp != limWarp) {
			WorldWarp * warp = *itrWarp;

			if (warp->iCol1 >= 0)
				spr_warps[iScreenshotSize].draw(warp->iCol1 * iTileSize, warp->iRow1 * iTileSize, warp->iID * iTileSize, 0, iTileSize, iTileSize);

			if (warp->iCol2 >= 0)
				spr_warps[iScreenshotSize].draw(warp->iCol2 * iTileSize, warp->iRow2 * iTileSize, warp->iID * iTileSize, 0, iTileSize, iTileSize);

			itrWarp++;
		}

		//Save the screenshot with the same name as the map file
		char szSaveFile[256];
		strcpy(szSaveFile, "worlds/screenshots/");
		char * pszSaveFile = szSaveFile + strlen(szSaveFile);
        GetNameFromFileName(pszSaveFile, worldlist->current_name());

		if (iTileSize == PREVIEWTILESIZE)
			strcat(szSaveFile, "_preview");
		else if (iTileSize == THUMBTILESIZE)
			strcat(szSaveFile, "_thumb");

#ifdef PNG_SAVE_FORMAT
		strcat(szSaveFile, ".png");
		IMG_SavePNG(sScreenshot, szSaveFile);
#else
		strcat(szSaveFile, ".bmp");
		SDL_SaveBMP(sScreenshot, szSaveFile);
#endif

		SDL_FreeSurface(sScreenshot);
	}

	g_worldmap.Load(iTileSizes[0]);
	blitdest = screen;
}
