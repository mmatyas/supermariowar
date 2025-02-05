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

#define SMW_EDITOR

#ifdef _MSC_VER
#define NOMINMAX
#endif

#include "CmdArgs.h"
#include "FileIO.h"
#include "FileList.h"
#include "GameMode.h"
#include "GameValues.h"
#include "map.h"
#include "MapList.h"
#include "ObjectContainer.h"
#include "path.h"
#include "player.h"
#include "ResourceManager.h"
#include "TilesetManager.h"
#include "world.h"
#include "WorldTourStop.h"
#include "menu/ModeOptionsMenu.h"
#include "objects/blocks/WeaponBreakableBlock.h"
#include "objects/carriable/CO_Egg.h"
#include "objects/carriable/CO_Flag.h"
#include "objects/carriable/CO_PhantoKey.h"
#include "objects/carriable/CO_Star.h"
#include "objects/carriable/CO_ThrowBox.h"
#include "objects/moving/MO_Coin.h"
#include "objects/overmap/WO_Area.h"
#include "ui/MI_Button.h"
#include "ui/MI_Image.h"
#include "ui/MI_ImageSelectField.h"
#include "ui/MI_MapField.h"
#include "ui/MI_Text.h"
#include "ui/MI_TextField.h"

#ifdef PNG_SAVE_FORMAT
// this function was added to SDL2
#ifndef USE_SDL2
#include "savepng.h"
#endif
#endif

#include "sdl12wrapper.h"
#include "SDL_image.h"

#include <algorithm>
#include <cstdlib>
#include <ctype.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#ifdef _MSC_VER
#if _MSC_VER >= 1400
#include <stdio.h>
FILE _iob[] = { *stdin, *stdout, *stderr };
extern "C" FILE* __cdecl __iob_func(void) { return _iob; }
#endif
#endif
#endif

#include "EditorBackground.h"
#include "EditorStageMarkers.h"
#include "EditorPaths.h"
#include "EditorPathSprites.h"
#include "EditorTileType.h"
#include "EditorVehicleBoundaries.h"
#include "EditorWater.h"
#include "EditorWarps.h"
#include "Helpers.h"

// TODO: Fix JS related problems.
#ifdef __EMSCRIPTEN__
#define SDL_Delay(n) ;
#endif

#define MAPTITLESTRING "World Editor"

enum {
    EDITOR_EDIT,
    EDITOR_WATER,
    EDITOR_BACKGROUND,
    EDITOR_STAGEFOREGROUND,
    EDITOR_STRUCTUREFOREGROUND,
    EDITOR_BRIDGES,
    EDITOR_PATHSPRITE,
    EDITOR_VEHICLES,
    EDITOR_QUIT,
    SAVE_AS,
    FIND,
    CLEAR_WORLD,
    NEW_WORLD,
    RESIZE_WORLD,
    SAVE,
    EDITOR_WARP,
    DISPLAY_HELP,
    EDITOR_PATH,
    EDITOR_TYPE,
    EDITOR_BOUNDARY,
    EDITOR_START_ITEMS,
    EDITOR_STAGE,
};

const char* szEditModes[10] = { "Background Mode", "Foreground Mode", "Path Sprite Mode", "Stage Mode", "Path Mode", "Vehicle Mode", "Warp Mode", "Start/Door Mode", "Boundary Mode", "Stage Mode" };


SDL_Surface* screen;
SDL_Surface* blitdest;
SDL_Surface* sMapSurface;

SDL_Rect rectSrcSurface = { 0, 0, 768, 608 };
SDL_Rect rectDstSurface = { 0, 0, 640, 480 };
bool fNeedBlackBackground = false;
short iWorldWidth, iWorldHeight;

SDL_Event event;

gfxSprite spr_dialog;
gfxSprite menu_shade;
gfxSprite spr_largedialog;

gfxSprite spr_vehicleicons;

int set_tile = 0;
bool fAutoPaint = true;

int edit_mode = 0;

int draw_offset_col = 0;  // col and row offset for drawing map to surface
int draw_offset_row = 0;
int draw_offset_x = 0;  // x and y offset for drawing maps smaller than screensize
int draw_offset_y = 0;

int state;
bool selectedtiles[MAPWIDTH][MAPHEIGHT];
bool moveselectedtiles[MAPWIDTH][MAPHEIGHT];
WorldMapTile copiedtiles[MAPWIDTH][MAPHEIGHT];

int mouse_x, mouse_y;

void update_mouse_coords()
{
    mouse_x = event.motion.x;
    mouse_y = event.motion.y;
    if (mouse_x < 0)
        mouse_x = 0;
    if (mouse_y < 0)
        mouse_y = 0;
    if (mouse_x > 640 - 1)
        mouse_x = 640 - 1;
    if (mouse_y > 480 - 1)
        mouse_y = 480 - 1;
}

int bound_to_window_w(int x)
{
    return std::max(0, std::min(x, 640));
}

int bound_to_window_h(int y)
{
    return std::max(0, std::min(y, 480));
}

// Vehicle structure that holds the current vehicle "stamp"
WorldVehicle g_wvVehicleStamp;

//// Global stuff that the map editor doesn't need, but has references to
gfxSprite spr_warplock;
short x_shake = 0;
short y_shake = 0;
extern CGameValues game_values;

void CPlayer::flipsidesifneeded() {}
bool CPlayer::isInvincible() const { return false; }
bool CPlayer::isShielded() const { return false; }
void CPlayer::AddKillerAward(CPlayer*, KillStyle) {}
PlayerKillType CPlayer::KillPlayerMapHazard(bool fForce, KillStyle style, bool fKillCarriedItem, short iPlayerId)
{
    return PlayerKillType::None;
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

// void IO_MovingObject::flipsidesifneeded() {}
// void IO_MovingObject::KillObjectMapHazard(short playerID) {}

void CO_Egg::placeEgg() {}
void CO_Flag::placeFlag() {}
void CO_PhantoKey::placeKey() {}
void CO_Star::placeStar() {}
void MO_Coin::placeCoin() {}
void OMO_Area::placeArea() {}
void OMO_Area::reset() {}
void OMO_Area::setOwner(CPlayer* player) {}

bool CO_ThrowBox::HasKillVelocity()
{
    return false;
}

PlayerKillType PlayerKilledPlayer(short id, CPlayer* killed, PlayerDeathStyle deathstyle, KillStyle style, bool fForce, bool fKillCarriedItem)
{
    return PlayerKillType::None;
}
void AddAwardKill(CPlayer* killer, CPlayer* killed, KillStyle style) {}

CPlayer* GetPlayerFromGlobalID(short id)
{
    return NULL;
}

/*float CapFallingVelocity(float f)
{
    return 0.0f;
}*/
void removeifprojectile(IO_MovingObject* object, bool playsound, bool forcedead) {}
bool LoadMenuSkin(short playerID, short skinID, short colorID, bool fLoadBothDirections)
{
    return false;
}

gfxSprite** spr_player[4];
CGameMode* gamemodes[GAMEMODE_LAST];

extern FiltersList* filterslist;
extern MapList* maplist;
extern GraphicsList* menugraphicspacklist;
extern GraphicsList* gamegraphicspacklist;

extern CMap* g_map;
extern CTilesetManager* g_tilesetmanager;

extern CResourceManager* rm;
extern std::string RootDataDirectory;

gfxSprite spr_overlay, spr_overlayhole;

void SDLCALL musicfinished() {}

CEyecandyContainer eyecandy[3];
gfxSprite spr_frontmap[2];

std::vector<CPlayer*> players;


short LookupTeamID(short id)
{
    return 0;
}

void CScore::AdjustScore(short iValue) {}
///////

CObjectContainer noncolcontainer;
CObjectContainer objectcontainer[3];

int save_as();
int find();
int clear_world();
bool dialog(const char* title, const char* instructions, char* input, int inputsize);
int display_help();

void drawmap(bool fScreenshot, short iBlockSize);

bool UpdateForeground(short iCol, short iRow);
bool UpdateCoastline(short iCol, short iRow);
bool AutoSetTile(short iCol, short iRow);
short AdjustForeground(short iSprite, short iCol, short iRow);

void ReadVehiclesIntoEditor();
void WriteVehiclesIntoWorld();
void AddVehicleToTile(short iCol, short iRow, short iType);
void RemoveVehicleFromTile(short iCol, short iRow);

void AddWarpToTile(short iCol, short iRow, short iType);
void RemoveWarpFromTile(short iCol, short iRow);

extern WorldMap g_worldmap;
extern WorldList* worldlist;
void loadcurrentworld();
int savecurrentworld();
int findcurrentstring();
int new_world();
int resize_world();

int editor_edit();
int editor_structureforeground();
int editor_bridges();
int editor_vehicles();
int editor_stage();
int editor_start_items();

EditorBackground editorBackground;
EditorPaths editorPaths;
EditorPathSprites editorPathSprites;
EditorStageMarkers editorStageMarkers;
EditorTileType editorTileType;
EditorVehicleBoundaries editorVehicleBoundaries;
EditorWater editorWater;
EditorWarps editorWarps;
constexpr std::array<EditorBase*, 8> allEditors {
    &editorBackground,
    &editorPaths,
    &editorPathSprites,
    &editorStageMarkers,
    &editorTileType,
    &editorVehicleBoundaries,
    &editorWarps,
    &editorWater,
};
EditorBase* currentEditor = nullptr;
int enterEditor(EditorBase& editor);

void DisplayStageDetails(bool fForce, short iStageId, short iMouseX, short iMouseY);

void updateworldsurface();
void takescreenshot();

#if defined(USE_SDL2) || defined(__EMSCRIPTEN__)
bool CheckKey(const Uint8* keystate, SDL_Keycode key)
{
    return keystate[SDL_GetScancodeFromKey(key)];
}
#else
bool CheckKey(Uint8* keystate, SDLKey key)
{
    return keystate[key];
}
#endif

bool ignoreclick = false;

char findstring[FILEBUFSIZE] = "";

const char* g_szWorldMusicCategoryNames[MAXWORLDMUSICCATEGORY] {
    "Grass",
    "Desert",
    "Water",
    "Giant",
    "Sky",
    "Ice",
    "Pipe",
    "Dark",
    "Space",
};
short g_musiccategorydisplaytimer = 0;

short g_messagedisplaytimer = 0;
std::string g_szMessageTitle = "";
std::string g_szMessageLine[3];
void SetDisplayMessage(short iTime,
    const char* szTitle,
    const char* szLine1,
    const char* szLine2,
    const char* szLine3);
void DrawMessage();

// Menu keys to use for menus
extern SDL_KEYTYPE controlkeys[2][2][4][NUM_KEYS];

// Vehicle stuff
std::vector<WorldVehicle*> vehiclelist;

bool g_fFullScreen = false;
bool g_fShowStagePreviews = true;

// Stage Mode Menu
UI_Menu* mCurrentMenu;
UI_Menu mStageSettingsMenu;
UI_Menu mBonusItemPicker;

MI_ImageSelectField* miModeField;
MI_SelectField<short>* miGoalField[GAMEMODE_LAST];
MI_Button* miModeSettingsButton;
MI_Button* miBonusItemsButton;
MI_SelectField<short>* miSpecialGoalField[3];

MI_SelectField<bool>* miFinalStageField;
MI_SelectField<short>* miPointsField;

MI_TextField* miNameField;
MI_SelectField<short>* miBonusType;
MI_TextField* miBonusTextField[5];

MI_MapField* miMapField;

MI_Button* miDeleteStageButton;

MI_Image* miDeleteStageDialogImage;
MI_Text* miDeleteStageDialogAreYouText;
MI_Text* miDeleteStageDialogSureText;
MI_Button* miDeleteStageDialogYesButton;
MI_Button* miDeleteStageDialogNoButton;

// Vehicle Creation Menu
UI_Menu mVehicleMenu;
MI_ImageSelectField* miVehicleSpriteField;
MI_ImageSelectField* miVehicleStageField;
MI_SelectField<short>* miVehicleMinMovesField;
MI_SelectField<short>* miVehicleMaxMovesField;
MI_SelectField<bool>* miVehiclePacesField;
MI_SelectField<short>* miVehicleDirectionField;
MI_SelectField<short>* miVehicleBoundaryField;
MI_Button* miVehicleCreateButton;
MI_Text* miTitleText;

UI_ModeOptionsMenu* mModeOptionsMenu;

SDL_Surface* sMapThumbnail = NULL;
short iOldStageId = -1;

// Sets up default mode options
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

void SetStageMode(short iIndex, const char* szModeName, const char* szGoalName, short iIncrement, short iDefault)
{
    if (iIndex < 0 || iIndex >= GAMEMODE_LAST)
        return;

    StageMode* sm = &stagemodes[iIndex];

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

// main main main
int main(int argc, char* argv[])
{
    const cmd::Args cmd = cmd::parse_args(argc, argv);
    if (!cmd.success) {
        return 1;
    }
    if (cmd.show_help) {
        char title[128];
        sprintf(title, "%s %s", TITLESTRING, MAPTITLESTRING);
        cmd::print_help(title, "");
        return 0;
    }
    if (cmd.debug) {
        cmd::show_windows_console();
    }
    if (!cmd.data_root.empty())
        RootDataDirectory = cmd.data_root;

    ensureSettingsDir();

    rm = new CResourceManager();

    g_map = new CMap();
    g_tilesetmanager = new CTilesetManager();
    filterslist = new FiltersList();
    maplist = new MapList(true);
    menugraphicspacklist = new GraphicsList;
    gamegraphicspacklist = new GraphicsList;
    worldlist = new WorldList;

    game_values.sound = false;
    game_values.music = false;

    /* This must occur before any data files are loaded */
    Initialize_Paths();

    bool done;

    printf("-------------------------------------------------------------------------------\n");
    printf(" %s %s\n", TITLESTRING, MAPTITLESTRING);
    printf("-------------------------------------------------------------------------------\n");
    printf("\n---------------- startup ----------------\n");

    int saved_col = 0, saved_row = 0;
    {
        const std::string options_path(GetHomeDirectory() + "worldeditor.bin");
        BinaryFile editor_settings(options_path, "rb");
        if (editor_settings.is_open()) {
            saved_col = editor_settings.read_i32();
            saved_row = editor_settings.read_i32();
            g_fFullScreen = editor_settings.read_bool();
            editor_settings.read_string_long(findstring, FILEBUFSIZE);
        }
    }

    gfx_init(640, 480, g_fFullScreen);
    blitdest = screen;
    g_tilesetmanager->init(convertPath("gfx/Classic/tilesets").c_str());

    char title[128];
    sprintf(title, "%s %s", TITLESTRING, MAPTITLESTRING);
    gfx_settitle(title);

    game_values.toplayer = true;

    printf("\n---------------- loading graphics ----------------\n");

    rm->spr_selectedtile.init(convertPath("gfx/leveleditor/leveleditor_selectedtile.png"), colors::BLACK, 128);

    spr_dialog.init(convertPath("gfx/leveleditor/leveleditor_dialog.png"), colors::MAGENTA, 255);
    menu_shade.init(convertPath("gfx/leveleditor/leveleditor_shade.png"), colors::MAGENTA, 128);
    spr_largedialog.init(convertPath("gfx/leveleditor/leveleditor_platform.png"), colors::MAGENTA, 255);

    rm->menu_font_small.init(convertPath("gfx/packs/Classic/fonts/font_small.png"));
    rm->menu_font_large.init(convertPath("gfx/packs/Classic/fonts/font_large.png"));

    for (EditorBase* editor : allEditors)
        editor->loadAssets();

    printf("\n---------------- load world ----------------\n");

    rm->spr_worldbackground[0].init(convertPath("gfx/packs/Classic/world/world_background.png"), colors::MAGENTA);
    rm->spr_worldbackground[1].init(convertPath("gfx/packs/Classic/world/preview/world_background.png"), colors::MAGENTA);
    rm->spr_worldbackground[2].init(convertPath("gfx/packs/Classic/world/thumbnail/world_background.png"), colors::MAGENTA);

    rm->spr_worldforeground[0].init(convertPath("gfx/packs/Classic/world/world_foreground.png"), colors::MAGENTA);
    rm->spr_worldforeground[1].init(convertPath("gfx/packs/Classic/world/preview/world_foreground.png"), colors::MAGENTA);
    rm->spr_worldforeground[2].init(convertPath("gfx/packs/Classic/world/thumbnail/world_foreground.png"), colors::MAGENTA);

    rm->spr_worldforegroundspecial[0].init(convertPath("gfx/packs/Classic/world/world_foreground_special.png"), colors::MAGENTA);
    rm->spr_worldforegroundspecial[1].init(convertPath("gfx/packs/Classic/world/preview/world_foreground_special.png"), colors::MAGENTA);
    rm->spr_worldforegroundspecial[2].init(convertPath("gfx/packs/Classic/world/thumbnail/world_foreground_special.png"), colors::MAGENTA);

    rm->spr_worldpaths[0].init(convertPath("gfx/packs/Classic/world/world_paths.png"), colors::MAGENTA);
    rm->spr_worldpaths[1].init(convertPath("gfx/packs/Classic/world/preview/world_paths.png"), colors::MAGENTA);
    rm->spr_worldpaths[2].init(convertPath("gfx/packs/Classic/world/thumbnail/world_paths.png"), colors::MAGENTA);

    rm->spr_worldvehicle[0].init(convertPath("gfx/packs/Classic/world/world_vehicles.png"), colors::MAGENTA);
    rm->spr_worldvehicle[1].init(convertPath("gfx/packs/Classic/world/preview/world_vehicles.png"), colors::MAGENTA);
    rm->spr_worldvehicle[2].init(convertPath("gfx/packs/Classic/world/thumbnail/world_vehicles.png"), colors::MAGENTA);

    rm->spr_worlditems.init(convertPath("gfx/packs/Classic/world/world_powerups.png"), colors::MAGENTA);
    rm->spr_worlditempopup.init(convertPath("gfx/packs/Classic/world/world_item_popup.png"), colors::MAGENTA);

    rm->spr_storedpowerupsmall.init(convertPath("gfx/packs/Classic/powerups/small.png"), colors::MAGENTA);
    rm->spr_worlditemssmall.init(convertPath("gfx/packs/Classic/world/world_powerupssmall.png"), colors::MAGENTA);
    rm->spr_worlditemsplace.init(convertPath("gfx/packs/Classic/world/world_bonusplace.png"), colors::MAGENTA);

    rm->menu_dialog.init(convertPath("gfx/packs/Classic/menu/menu_dialog.png"), colors::MAGENTA);

    // Mode Options Menu Gfx
    rm->menu_egg.init(convertPath("gfx/packs/Classic/modeobjects/menu_egg.png"), colors::MAGENTA);
    rm->menu_stomp.init(convertPath("gfx/packs/Classic/modeobjects/menu_stomp.png"), colors::MAGENTA);
    rm->menu_survival.init(convertPath("gfx/packs/Classic/modeobjects/menu_survival.png"), colors::MAGENTA);
    rm->spr_phanto.init(convertPath("gfx/packs/Classic/modeobjects/phanto.png"), colors::MAGENTA);
    rm->menu_plain_field.init(convertPath("gfx/leveleditor/menu_plain_field.png"), colors::MAGENTA);
    rm->menu_slider_bar.init(convertPath("gfx/packs/Classic/menu/menu_slider_bar.png"), colors::MAGENTA);
    rm->spr_selectfield.init(convertPath("gfx/leveleditor/menu_selectfield.png"), colors::MAGENTA);
    rm->menu_verticalarrows.init(convertPath("gfx/packs/Classic/menu/menu_vertical_arrows.png"), colors::MAGENTA);
    rm->spr_storedpoweruplarge.init(convertPath("gfx/packs/Classic/powerups/large.png"), colors::MAGENTA);

    rm->menu_mode_small.init(convertPath("gfx/packs/Classic/menu/menu_mode_small.png"), colors::MAGENTA);
    rm->menu_mode_large.init(convertPath("gfx/packs/Classic/menu/menu_mode_large.png"), colors::MAGENTA);

    spr_vehicleicons.init(convertPath("gfx/leveleditor/vehicle_icons.png"), colors::MAGENTA);

    rm->spr_thumbnail_warps[0].init(convertPath("gfx/packs/Classic/menu/menu_warp_preview.png"), colors::MAGENTA);
    rm->spr_thumbnail_warps[1].init(convertPath("gfx/packs/Classic/menu/menu_warp_thumbnail.png"), colors::MAGENTA);

    rm->spr_thumbnail_mapitems[0].init(convertPath("gfx/packs/Classic/menu/menu_mapitems_preview.png"), colors::MAGENTA);
    rm->spr_thumbnail_mapitems[1].init(convertPath("gfx/packs/Classic/menu/menu_mapitems_thumbnail.png"), colors::MAGENTA);

    rm->spr_tileanimation[1].init(convertPath("gfx/packs/Classic/tilesets/tile_animation_preview.png"), colors::MAGENTA);
    rm->spr_tileanimation[2].init(convertPath("gfx/packs/Classic/tilesets/tile_animation_thumbnail.png"), colors::MAGENTA);

    rm->spr_blocks[1].init(convertPath("gfx/packs/Classic/tilesets/blocks_preview.png"), colors::MAGENTA);
    rm->spr_blocks[2].init(convertPath("gfx/packs/Classic/tilesets/blocks_thumbnail.png"), colors::MAGENTA);

    rm->spr_unknowntile[1].init(convertPath("gfx/packs/Classic/tilesets/unknown_tile_preview.png"), colors::MAGENTA);
    rm->spr_unknowntile[2].init(convertPath("gfx/packs/Classic/tilesets/unknown_tile_thumbnail.png"), colors::MAGENTA);

    rm->spr_hazard_fireball[1].init(convertPath("gfx/packs/Classic/hazards/fireball_preview.png"), colors::MAGENTA);
    rm->spr_hazard_fireball[2].init(convertPath("gfx/packs/Classic/hazards/fireball_thumbnail.png"), colors::MAGENTA);

    rm->spr_hazard_rotodisc[1].init(convertPath("gfx/packs/Classic/hazards/rotodisc_preview.png"), colors::MAGENTA);
    rm->spr_hazard_rotodisc[2].init(convertPath("gfx/packs/Classic/hazards/rotodisc_thumbnail.png"), colors::MAGENTA);

    rm->spr_hazard_bulletbill[1].init(convertPath("gfx/packs/Classic/hazards/bulletbill_preview.png"), colors::MAGENTA);
    rm->spr_hazard_bulletbill[2].init(convertPath("gfx/packs/Classic/hazards/bulletbill_thumbnail.png"), colors::MAGENTA);

    rm->spr_hazard_flame[1].init(convertPath("gfx/packs/Classic/hazards/flame_preview.png"), colors::MAGENTA);
    rm->spr_hazard_flame[2].init(convertPath("gfx/packs/Classic/hazards/flame_thumbnail.png"), colors::MAGENTA);

    rm->spr_hazard_pirhanaplant[1].init(convertPath("gfx/packs/Classic/hazards/pirhanaplant_preview.png"), colors::MAGENTA);
    rm->spr_hazard_pirhanaplant[2].init(convertPath("gfx/packs/Classic/hazards/pirhanaplant_thumbnail.png"), colors::MAGENTA);

    rm->LoadMenuGraphics();

    sMapSurface = SDL_CreateRGBSurface(screen->flags, 768, 608, screen->format->BitsPerPixel, 0, 0, 0, 0);

    worldlist->find(findstring);
    game_values.worldindex = worldlist->currentIndex();
    loadcurrentworld();
    findstring[0] = 0;  // clear out the find string so that pressing "f" will give you the find dialog

    if (saved_row >= 0 && saved_row <= iWorldHeight - 15 && saved_col >= 0 && saved_col <= iWorldWidth - 20) {
        draw_offset_row = saved_row;
        draw_offset_col = saved_col;
        updateworldsurface();
    }

    // Setup input for menus
    game_values.inputConfiguration[0][0].iDevice = DEVICE_KEYBOARD;
    for (short iInputState = 0; iInputState < 2; iInputState++) {  // for game/menu
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

    // Setup The Mode Menu
    mCurrentMenu = &mStageSettingsMenu;

    // Name
    miNameField = new MI_TextField(&rm->spr_selectfield, 70, 20, "Name", 500, 120);
    miNameField->SetDisallowedChars(",");

    miModeField = new MI_ImageSelectField(&rm->spr_selectfield, &rm->menu_mode_small, 70, 60, "Mode", 500, 120, 16, 16);
    // miModeField->SetData(game_values.tourstops[0]->iMode, NULL, NULL);
    // miModeField->SetKey(0);
    miModeField->setItemChangedCode(MENU_CODE_MODE_CHANGED);

    for (short iGameMode = 0; iGameMode < GAMEMODE_LAST; iGameMode++) {
        miModeField->add(stagemodes[iGameMode].szName, iGameMode);

        miGoalField[iGameMode] = new MI_SelectField<short>(&rm->spr_selectfield, 70, 100, stagemodes[iGameMode].szGoal, 352, 120);
        miGoalField[iGameMode]->setVisible(iGameMode == 0);

        for (short iGameModeOption = 0; iGameModeOption < GAMEMODE_NUM_OPTIONS - 1; iGameModeOption++) {
            StageModeOption* option = &stagemodes[iGameMode].options[iGameModeOption];
            miGoalField[iGameMode]->add(option->szName, option->iValue);
        }

        // miGoalField[iGameMode]->SetData(&gamemodes[iGameMode]->goal, NULL, NULL);
        // miGoalField[iGameMode]->SetKey(gamemodes[iGameMode]->goal);
    }

    miModeField->add("Bonus House", 24);
    miModeField->add("Pipe Minigame", 25);
    miModeField->add("Boss Minigame", 26);
    miModeField->add("Boxes Minigame", 27);

    // Create goal field for pipe game
    miSpecialGoalField[0] = new MI_SelectField<short>(&rm->spr_selectfield, 70, 100, "Points", 352, 120);
    miSpecialGoalField[0]->setVisible(false);

    for (short iGameModeOption = 0; iGameModeOption < GAMEMODE_NUM_OPTIONS - 1; iGameModeOption++) {
        short iValue = 10 + iGameModeOption * 10;
        char szName[16];
        sprintf(szName, "%d", iValue);
        miSpecialGoalField[0]->add(szName, iValue);
    }

    // Create goal field for boss game
    miSpecialGoalField[1] = new MI_SelectField<short>(&rm->spr_selectfield, 70, 100, "Lives", 352, 120);
    miSpecialGoalField[1]->setVisible(false);

    for (short iGameLives = 1; iGameLives <= 30; iGameLives++) {
        char szName[16];
        sprintf(szName, "%d", iGameLives);
        miSpecialGoalField[1]->add(szName, iGameLives);
    }

    // Create goal field for boxes game
    miSpecialGoalField[2] = new MI_SelectField<short>(&rm->spr_selectfield, 70, 100, "Lives", 352, 120);
    miSpecialGoalField[2]->setVisible(false);

    for (short iGameLives = 1; iGameLives <= 30; iGameLives++) {
        char szName[16];
        sprintf(szName, "%d", iGameLives);
        miSpecialGoalField[2]->add(szName, iGameLives);
    }

    // Mode Settings Button
    miModeSettingsButton = new MI_Button(&rm->spr_selectfield, 430, 100, "Settings", 140);
    miModeSettingsButton->SetCode(MENU_CODE_TO_MODE_SETTINGS_MENU);

    // Points Field
    miPointsField = new MI_SelectField<short>(&rm->spr_selectfield, 70, 140, "Points", 245, 120);
    for (short iPoints = 0; iPoints <= 20; iPoints++) {
        char szPoints[8];
        sprintf(szPoints, "%d", iPoints);
        miPointsField->add(szPoints, iPoints);
    }

    // Final Stage Field
    miFinalStageField = new MI_SelectField<bool>(&rm->spr_selectfield, 325, 140, "End Stage", 245, 120);
    miFinalStageField->add("No", false);
    miFinalStageField->add("Yes", true);
    miFinalStageField->setAutoAdvance(true);

    // Map Select Field
    miMapField = new MI_MapField(&rm->spr_selectfield, 70, 180, "Map", 500, 120, true);

    // Bonus Item Picker Menu Button
    miBonusItemsButton = new MI_Button(&rm->spr_selectfield, 430, 220, "Bonuses", 140);
    miBonusItemsButton->SetCode(MENU_CODE_TO_BONUS_PICKER_MENU);

    // Bonus Type
    miBonusType = new MI_SelectField<short>(&rm->spr_selectfield, 70, 100, "Type", 500, 120);
    miBonusType->add("Fixed", false);
    miBonusType->add("Random", true);
    miBonusType->setAutoAdvance(true);

    // Bonus House Text * 5
    miBonusTextField[0] = new MI_TextField(&rm->spr_selectfield, 70, 140, "Text", 500, 120);
    miBonusTextField[1] = new MI_TextField(&rm->spr_selectfield, 70, 180, "Text", 500, 120);
    miBonusTextField[2] = new MI_TextField(&rm->spr_selectfield, 70, 220, "Text", 500, 120);
    miBonusTextField[3] = new MI_TextField(&rm->spr_selectfield, 70, 260, "Text", 500, 120);
    miBonusTextField[4] = new MI_TextField(&rm->spr_selectfield, 70, 300, "Text", 500, 120);

    miBonusTextField[0]->SetDisallowedChars(",|");
    miBonusTextField[1]->SetDisallowedChars(",|");
    miBonusTextField[2]->SetDisallowedChars(",|");
    miBonusTextField[3]->SetDisallowedChars(",|");
    miBonusTextField[4]->SetDisallowedChars(",|");

    // Delete Stage Button
    miDeleteStageButton = new MI_Button(&rm->spr_selectfield, 430, 440, "Delete", 140);
    miDeleteStageButton->SetCode(MENU_CODE_DELETE_STAGE_BUTTON);

    // Are You Sure Dialog for Delete Stage
    miDeleteStageDialogImage = new MI_Image(&spr_dialog, 224, 176, 0, 0, 192, 128, 1, 1, 0);
    miDeleteStageDialogAreYouText = new MI_HeaderText("Are You", 320, 195);
    miDeleteStageDialogSureText = new MI_HeaderText("Sure?", 320, 220);
    miDeleteStageDialogYesButton = new MI_Button(&rm->spr_selectfield, 235, 250, "Yes", 80, TextAlign::CENTER);
    miDeleteStageDialogNoButton = new MI_Button(&rm->spr_selectfield, 325, 250, "No", 80, TextAlign::CENTER);

    miDeleteStageDialogYesButton->SetCode(MENU_CODE_DELETE_STAGE_YES);
    miDeleteStageDialogNoButton->SetCode(MENU_CODE_DELETE_STAGE_NO);

    miDeleteStageDialogImage->setVisible(false);
    miDeleteStageDialogAreYouText->setVisible(false);
    miDeleteStageDialogSureText->setVisible(false);
    miDeleteStageDialogYesButton->setVisible(false);
    miDeleteStageDialogNoButton->setVisible(false);

    // Add Name Field
    mStageSettingsMenu.AddControl(miNameField, miDeleteStageButton, miModeField, NULL, NULL);

    // Add Mode Field
    mStageSettingsMenu.AddControl(miModeField, miNameField, miGoalField[0], NULL, NULL);

    // Add Mode Goal Fields
    mStageSettingsMenu.AddControl(miGoalField[0], miModeField, miGoalField[1], NULL, miModeSettingsButton);

    for (short iGoalField = 1; iGoalField < GAMEMODE_LAST - 1; iGoalField++)
        mStageSettingsMenu.AddControl(miGoalField[iGoalField], miGoalField[iGoalField - 1], miGoalField[iGoalField + 1], miGoalField[iGoalField - 1], miModeSettingsButton);

    mStageSettingsMenu.AddControl(miGoalField[GAMEMODE_LAST - 1], miGoalField[GAMEMODE_LAST - 2], miSpecialGoalField[0], miGoalField[GAMEMODE_LAST - 2], miModeSettingsButton);

    mStageSettingsMenu.AddControl(miSpecialGoalField[0], miGoalField[GAMEMODE_LAST - 1], miSpecialGoalField[1], miGoalField[GAMEMODE_LAST - 1], miSpecialGoalField[1]);
    mStageSettingsMenu.AddControl(miSpecialGoalField[1], miSpecialGoalField[0], miSpecialGoalField[2], miSpecialGoalField[0], miSpecialGoalField[2]);
    mStageSettingsMenu.AddControl(miSpecialGoalField[2], miSpecialGoalField[1], miPointsField, miSpecialGoalField[1], miModeSettingsButton);

    // Add Mode Settings Button
    mStageSettingsMenu.AddControl(miModeSettingsButton, miModeField, miFinalStageField, miSpecialGoalField[2], NULL);

    // Add Points Field
    mStageSettingsMenu.AddControl(miPointsField, miSpecialGoalField[2], miMapField, NULL, miFinalStageField);

    // Add Final Stage Field
    mStageSettingsMenu.AddControl(miFinalStageField, miSpecialGoalField[2], miMapField, miPointsField, NULL);

    // Add Map Field
    mStageSettingsMenu.AddControl(miMapField, miFinalStageField, miBonusType, NULL, miBonusItemsButton);

    // Add Bonus House Fields
    mStageSettingsMenu.AddControl(miBonusType, miMapField, miBonusTextField[0], NULL, NULL);
    mStageSettingsMenu.AddControl(miBonusTextField[0], miBonusType, miBonusTextField[1], NULL, NULL);
    mStageSettingsMenu.AddControl(miBonusTextField[1], miBonusTextField[0], miBonusTextField[2], NULL, NULL);
    mStageSettingsMenu.AddControl(miBonusTextField[2], miBonusTextField[1], miBonusTextField[3], NULL, NULL);
    mStageSettingsMenu.AddControl(miBonusTextField[3], miBonusTextField[2], miBonusTextField[4], NULL, NULL);
    mStageSettingsMenu.AddControl(miBonusTextField[4], miBonusTextField[3], miBonusItemsButton, NULL, NULL);

    // Add Bonus Button
    mStageSettingsMenu.AddControl(miBonusItemsButton, miBonusTextField[4], miDeleteStageButton, miMapField, NULL);

    // Add Delete Stage Button
    mStageSettingsMenu.AddControl(miDeleteStageButton, miBonusItemsButton, miNameField, miMapField, NULL);

    // Add Are You Sure Dialog
    mStageSettingsMenu.AddNonControl(miDeleteStageDialogImage);
    mStageSettingsMenu.AddNonControl(miDeleteStageDialogAreYouText);
    mStageSettingsMenu.AddNonControl(miDeleteStageDialogSureText);

    mStageSettingsMenu.AddControl(miDeleteStageDialogYesButton, NULL, NULL, NULL, miDeleteStageDialogNoButton);
    mStageSettingsMenu.AddControl(miDeleteStageDialogNoButton, NULL, NULL, miDeleteStageDialogYesButton, NULL);

    mStageSettingsMenu.setInitialFocus(miNameField);
    mStageSettingsMenu.SetCancelCode(MENU_CODE_EXIT_APPLICATION);

    mBonusItemPicker.SetCancelCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);

    mModeOptionsMenu = new UI_ModeOptionsMenu();

    g_wvVehicleStamp.iDrawSprite = 0;
    g_wvVehicleStamp.iActionId = 0;
    g_wvVehicleStamp.currentTile.x = 0;
    g_wvVehicleStamp.currentTile.y = 0;
    g_wvVehicleStamp.iMinMoves = 5;
    g_wvVehicleStamp.iMaxMoves = 8;
    g_wvVehicleStamp.fSpritePaces = true;
    g_wvVehicleStamp.iDrawDirection = 0;
    g_wvVehicleStamp.iBoundary = 0;

    // Create Vehicle Menu
    miVehicleSpriteField = new MI_ImageSelectField(&rm->spr_selectfield, &spr_vehicleicons, 70, 80, "Sprite", 500, 150, 16, 16);
    miVehicleSpriteField->add("Hammer Brother", 0);
    miVehicleSpriteField->add("Boomerang Brother", 1);
    miVehicleSpriteField->add("Fire Brother", 2);
    miVehicleSpriteField->add("Tank 1", 3);
    miVehicleSpriteField->add("Boat 1", 4);
    miVehicleSpriteField->add("Boat 2", 5);
    miVehicleSpriteField->add("Airship 1", 6);
    miVehicleSpriteField->add("Airship 2", 7);
    miVehicleSpriteField->add("Tank 2", 8);
    miVehicleSpriteField->setOutputPtr(&g_wvVehicleStamp.iDrawSprite);
    miVehicleSpriteField->setCurrentValue(g_wvVehicleStamp.iDrawSprite);

    miVehicleStageField = new MI_ImageSelectField(&rm->spr_selectfield, &rm->menu_mode_small, 70, 120, "Stage", 500, 150, 16, 16);
    miVehicleStageField->setOutputPtr(&g_wvVehicleStamp.iActionId);

    miVehicleMinMovesField = new MI_SelectField<short>(&rm->spr_selectfield, 70, 160, "Min Moves", 500, 150);

    for (short iMinMoves = 0; iMinMoves <= 100; iMinMoves++) {
        char szMinMoves[8];
        sprintf(szMinMoves, "%d", iMinMoves);
        miVehicleMinMovesField->add(szMinMoves, iMinMoves);
    }

    miVehicleMinMovesField->setOutputPtr(&g_wvVehicleStamp.iMinMoves);
    miVehicleMinMovesField->setCurrentValue(g_wvVehicleStamp.iMinMoves);
    miVehicleMinMovesField->setItemChangedCode(MENU_CODE_VEHICLE_MIN_MOVES_CHANGED);
    miVehicleMinMovesField->allowWrap(false);
    miVehicleMinMovesField->allowFastScroll(true);

    miVehicleMaxMovesField = new MI_SelectField<short>(&rm->spr_selectfield, 70, 200, "Max Moves", 500, 150);

    for (short iMaxMoves = 0; iMaxMoves <= 100; iMaxMoves++) {
        char szMaxMoves[8];
        sprintf(szMaxMoves, "%d", iMaxMoves);
        miVehicleMaxMovesField->add(szMaxMoves, iMaxMoves);
    }

    miVehicleMaxMovesField->setOutputPtr(&g_wvVehicleStamp.iMaxMoves);
    miVehicleMaxMovesField->setCurrentValue(g_wvVehicleStamp.iMaxMoves);
    miVehicleMaxMovesField->setItemChangedCode(MENU_CODE_VEHICLE_MAX_MOVES_CHANGED);
    miVehicleMaxMovesField->allowWrap(false);
    miVehicleMaxMovesField->allowFastScroll(true);

    miVehiclePacesField = new MI_SelectField<bool>(&rm->spr_selectfield, 70, 240, "Paces", 500, 150);
    miVehiclePacesField->add("No", false);
    miVehiclePacesField->add("Yes", true);
    miVehiclePacesField->setOutputPtr(&g_wvVehicleStamp.fSpritePaces);
    miVehiclePacesField->setCurrentValue(g_wvVehicleStamp.fSpritePaces ? 1 : 0);
    miVehiclePacesField->setAutoAdvance(true);

    miVehicleDirectionField = new MI_SelectField<short>(&rm->spr_selectfield, 70, 280, "Direction", 500, 150);
    miVehicleDirectionField->add("Left", false);
    miVehicleDirectionField->add("Right", true);
    miVehicleDirectionField->setOutputPtr(&g_wvVehicleStamp.iDrawDirection);
    miVehicleDirectionField->setCurrentValue(g_wvVehicleStamp.iDrawDirection);
    miVehicleDirectionField->setAutoAdvance(true);

    miVehicleBoundaryField = new MI_SelectField<short>(&rm->spr_selectfield, 70, 320, "Boundary", 500, 150);
    miVehicleBoundaryField->add("No Boundary", 0);

    for (short iBoundary = 1; iBoundary <= 100; iBoundary++) {
        char szBoundary[8];
        sprintf(szBoundary, "%d", iBoundary);
        miVehicleBoundaryField->add(szBoundary, iBoundary);
    }

    miVehicleBoundaryField->setOutputPtr(&g_wvVehicleStamp.iBoundary);
    miVehicleBoundaryField->setCurrentValue(g_wvVehicleStamp.iBoundary);
    miVehicleBoundaryField->allowFastScroll(true);

    miVehicleCreateButton = new MI_Button(&rm->spr_selectfield, 430, 360, "OK", 140, TextAlign::CENTER);
    miVehicleCreateButton->SetCode(MENU_CODE_CREATE_VEHICLE);

    miTitleText = new MI_Text("Clicking on the map will add the vehicle configured below", 320, 50, 640, true, TextAlign::CENTER);

    mVehicleMenu.AddNonControl(miTitleText);

    mVehicleMenu.AddControl(miVehicleSpriteField, miVehicleCreateButton, miVehicleStageField, NULL, NULL);
    mVehicleMenu.AddControl(miVehicleStageField, miVehicleSpriteField, miVehicleMinMovesField, NULL, NULL);
    mVehicleMenu.AddControl(miVehicleMinMovesField, miVehicleStageField, miVehicleMaxMovesField, NULL, NULL);
    mVehicleMenu.AddControl(miVehicleMaxMovesField, miVehicleMinMovesField, miVehiclePacesField, NULL, NULL);
    mVehicleMenu.AddControl(miVehiclePacesField, miVehicleMaxMovesField, miVehicleDirectionField, NULL, NULL);
    mVehicleMenu.AddControl(miVehicleDirectionField, miVehiclePacesField, miVehicleBoundaryField, NULL, NULL);
    mVehicleMenu.AddControl(miVehicleBoundaryField, miVehicleDirectionField, miVehicleCreateButton, NULL, NULL);
    mVehicleMenu.AddControl(miVehicleCreateButton, miVehicleBoundaryField, miVehicleSpriteField, NULL, NULL);

    mVehicleMenu.setInitialFocus(miVehicleSpriteField);
    mVehicleMenu.SetCancelCode(MENU_CODE_EXIT_APPLICATION);

#ifndef USE_SDL2
    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
#endif

    printf("\n---------------- ready, steady, go! ----------------\n");

    editorBackground.setAutoPaint(fAutoPaint);
    editorPaths.setAutoPaint(fAutoPaint);
    editorPathSprites.setAutoPaint(fAutoPaint);

    printf("entering world editor loop...\n");
    done = false;
    while (!done) {
        if (state != EDITOR_EDIT)
            currentEditor = nullptr;

        switch (state) {
        case EDITOR_EDIT:
            state = editor_edit();
            break;

        case EDITOR_WATER:
            state = enterEditor(editorWater);
            edit_mode = 7;
            break;

        case EDITOR_BACKGROUND:
            state = enterEditor(editorBackground);
            edit_mode = 0;
            break;

        case EDITOR_STAGEFOREGROUND:
            state = enterEditor(editorStageMarkers);
            edit_mode = 1;
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
            state = enterEditor(editorPaths);
            edit_mode = 2;
            break;

        case EDITOR_PATHSPRITE:
            state = enterEditor(editorPathSprites);
            edit_mode = 4;
            break;

        case EDITOR_WARP:
            state = enterEditor(editorWarps);
            edit_mode = 6;
            break;

        case EDITOR_START_ITEMS:
            state = editor_start_items();
            break;

        case EDITOR_BOUNDARY:
            state = enterEditor(editorVehicleBoundaries);
            edit_mode = 8;
            break;

        case EDITOR_TYPE:
            state = enterEditor(editorTileType);
            edit_mode = 3;
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
    g_worldmap.Save(convertPath("worlds/ZZworldeditor.txt").c_str());

    {
        const std::string options_path(GetHomeDirectory() + "worldeditor.bin");
        BinaryFile editor_settings(options_path, "wb");
        if (editor_settings.is_open()) {
            editor_settings.write_i32(draw_offset_col);
            editor_settings.write_i32(draw_offset_row);
            editor_settings.write_bool(g_fFullScreen);
            editor_settings.write_string_long(worldlist->currentPath().c_str());
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
            // handle messages
            while (SDL_PollEvent(&event)) {
                switch (event.type) {
                case SDL_KEYDOWN: {
#ifdef USE_SDL2
                    SDL_Keycode key = event.key.keysym.sym;
#else
                    SDLKey key = event.key.keysym.sym;
#endif

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
            // handle messages
            while (SDL_PollEvent(&event)) {
#if defined(USE_SDL2) || defined(__EMSCRIPTEN__)
                const Uint8* keystate = SDL_GetKeyboardState(NULL);
#else
                Uint8* keystate = SDL_GetKeyState(NULL);
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
                        short iButtonX = mouse_x - draw_offset_x;
                        short iButtonY = mouse_y - draw_offset_y;
                        short iCol = iButtonX / TILESIZE + draw_offset_col;
                        short iRow = iButtonY / TILESIZE + draw_offset_row;

                        std::vector<WorldVehicle*>::iterator itr = vehiclelist.begin(), lim = vehiclelist.end();
                        while (itr != lim) {
                            WorldVehicle* vehicle = *itr;
                            if (vehicle->currentTile.x == iCol && vehicle->currentTile.y == iRow) {
                                g_wvVehicleStamp.iDrawSprite = vehicle->iDrawSprite;
                                g_wvVehicleStamp.iActionId = vehicle->iActionId;
                                g_wvVehicleStamp.iMinMoves = vehicle->iMinMoves;
                                g_wvVehicleStamp.iMaxMoves = vehicle->iMaxMoves;
                                g_wvVehicleStamp.fSpritePaces = vehicle->fSpritePaces;
                                g_wvVehicleStamp.iDrawDirection = vehicle->iDrawDirection;
                                g_wvVehicleStamp.iBoundary = vehicle->iBoundary;

                                miVehicleSpriteField->setCurrentValue(g_wvVehicleStamp.iDrawSprite);
                                miVehicleStageField->setCurrentValue(g_wvVehicleStamp.iActionId);
                                miVehicleMinMovesField->setCurrentValue(g_wvVehicleStamp.iMinMoves);
                                miVehicleMaxMovesField->setCurrentValue(g_wvVehicleStamp.iMaxMoves);
                                miVehiclePacesField->setCurrentValue(g_wvVehicleStamp.fSpritePaces ? 1 : 0);
                                miVehicleDirectionField->setCurrentValue(g_wvVehicleStamp.iDrawDirection);
                                miVehicleBoundaryField->setCurrentValue(g_wvVehicleStamp.iBoundary);

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

                    if (event.key.keysym.sym == SDLK_a) {
                        fAutoPaint = !fAutoPaint;
                        editorBackground.setAutoPaint(fAutoPaint);
                        editorPaths.setAutoPaint(fAutoPaint);
                        editorPathSprites.setAutoPaint(fAutoPaint);
                    }

                    if (event.key.keysym.sym == SDLK_r) {
                        if (g_musiccategorydisplaytimer > 0 && ++g_worldmap.iMusicCategory >= MAXWORLDMUSICCATEGORY)
                            g_worldmap.iMusicCategory = 0;

                        g_musiccategorydisplaytimer = 90;
                    }

                    if (event.key.keysym.sym == SDLK_s) {
                        if (CheckKey(keystate, SDLK_LSHIFT) || CheckKey(keystate, SDLK_RSHIFT))
                            return SAVE_AS;

                        return SAVE;
                    }

                    if (event.key.keysym.sym == SDLK_f) {
                        if (CheckKey(keystate, SDLK_LSHIFT) || CheckKey(keystate, SDLK_RSHIFT) || findstring[0] == '\0')
                            return FIND;

                        findcurrentstring();
                    }

                    if (event.key.keysym.sym == SDLK_DELETE && (CheckKey(keystate, SDLK_LCTRL) || CheckKey(keystate, SDLK_RCTRL))) {
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
                            game_values.worldindex = worldlist->count() - 1;

                        worldlist->prev();

                        loadcurrentworld();

                        iOldStageId = -1;
                    }

                    if (event.key.keysym.sym == SDLK_PAGEDOWN) {
                        if (++game_values.worldindex >= worldlist->count())
                            game_values.worldindex = 0;

                        worldlist->next();

                        loadcurrentworld();

                        iOldStageId = -1;
                    }

                    break;
                }

                case SDL_KEYUP: {
                    if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_RIGHT) {
                        view_repeat_direction = -1;
                        view_repeat_timer = 0;
                    }

                    break;
                }

                case SDL_MOUSEBUTTONDOWN: {
                    short iButtonX = bound_to_window_w(event.button.x) - draw_offset_x;
                    short iButtonY = bound_to_window_h(event.button.y) - draw_offset_y;
                    short iCol = iButtonX / TILESIZE + draw_offset_col;
                    short iRow = iButtonY / TILESIZE + draw_offset_row;

                    if (iButtonX >= 0 && iButtonY >= 0 && iButtonX < iWorldWidth * TILESIZE && iButtonY < iWorldHeight * TILESIZE) {
                        if (currentEditor) {
                            const bool changed = currentEditor->onTileClicked(g_worldmap, {iCol, iRow}, event.button.button);
                            if (changed)
                                updateworldsurface();
                        }

                        if (event.button.button == SDL_BUTTON_LEFT && !ignoreclick) {
                            if (edit_mode == 5) {  // selected vehicle
                                AddVehicleToTile(iCol, iRow, set_tile);
                            } else if (edit_mode == 9) {
                                // if the stage was placed on a start tile
                                if (g_worldmap.tiles.at(iCol, iRow).iType == 1) {
                                    g_worldmap.tiles.at(iCol, iRow).iForegroundSprite = 0;
                                    updateworldsurface();
                                }

                                g_worldmap.tiles.at(iCol, iRow).iType = set_tile;
                            }
                        } else if (event.button.button == SDL_BUTTON_RIGHT) {
                            if (edit_mode == 5) {
                                RemoveVehicleFromTile(iCol, iRow);
                                iStageDisplay = -1;
                            } else if (edit_mode == 9) {  // stage
                                g_worldmap.tiles.at(iCol, iRow).iType = 0;
                                iStageDisplay = -1;
                            }
                        }
                    }

                    break;
                }
                    // Painting tiles with mouse movement
                case SDL_MOUSEMOTION: {
                    update_mouse_coords();
                    short iButtonX = bound_to_window_w(event.motion.x) - draw_offset_x;
                    short iButtonY = bound_to_window_h(event.motion.y) - draw_offset_y;
                    short iCol = (iButtonX >> 5) + draw_offset_col;
                    short iRow = (iButtonY >> 5) + draw_offset_row;

                    if (iButtonX >= 0 && iButtonY >= 0 && iButtonX < iWorldWidth * TILESIZE && iButtonY < iWorldHeight * TILESIZE) {
                        const bool isLeftPressed = event.motion.state & SDL_BUTTON_LMASK;
                        const bool isRightPressed = event.motion.state & SDL_BUTTON_RMASK;

                        if (currentEditor && (isLeftPressed || isRightPressed)) {
                            const uint8_t button = isLeftPressed ? SDL_BUTTON_LEFT : SDL_BUTTON_RIGHT;  // Prefer left press if both are down
                            const bool changed = currentEditor->onTileClicked(g_worldmap, {iCol, iRow}, button);
                            if (changed)
                                updateworldsurface();
                        }

                        if (event.motion.state == SDL_BUTTON(SDL_BUTTON_LEFT) && !ignoreclick) {
                            if (edit_mode == 5) {
                                AddVehicleToTile(iCol, iRow, set_tile);
                            } else if (edit_mode == 9) {  // stage
                                                          // if the stage was placed on a start tile
                                if (g_worldmap.tiles.at(iCol, iRow).iType == 1) {
                                    g_worldmap.tiles.at(iCol, iRow).iForegroundSprite = 0;
                                    updateworldsurface();
                                }

                                g_worldmap.tiles.at(iCol, iRow).iType = set_tile;
                            }
                        } else if (event.motion.state == SDL_BUTTON(SDL_BUTTON_RIGHT)) {
                            if (edit_mode == 5) {  // vehicles
                                RemoveVehicleFromTile(iCol, iRow);
                                iStageDisplay = -1;
                            } else if (edit_mode == 9) {  // stage
                                g_worldmap.tiles.at(iCol, iRow).iType = 0;
                                iStageDisplay = -1;
                            }
                        }
                    }

                    // Scan to see if we are mousing over a stage
                    if (edit_mode == 5) {
                        iStageDisplay = -1;

                        std::vector<WorldVehicle*>::iterator itr = vehiclelist.begin(), lim = vehiclelist.end();
                        while (itr != lim) {
                            WorldVehicle* vehicle = *itr;
                            if (vehicle->currentTile.x == iCol && vehicle->currentTile.y == iRow) {
                                iStageDisplay = vehicle->iActionId;
                                break;
                            }

                            itr++;
                        }
                    } else if (edit_mode == 9) {
                        iStageDisplay = -1;
                        if (iCol >= 0 && iRow >= 0 && iCol < iWorldWidth && iRow < iWorldHeight) {
                            short iType = g_worldmap.tiles.at(iCol, iRow).iType - 6;
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

            // Allow auto-scrolling of world when the arrow keys are held down
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

        // Ask if you are sure you want to exit
        if (fExiting) {
            spr_dialog.draw(224, 176, 0, 0, 192, 128);
            rm->menu_font_large.drawCentered(320, 195, "Exit");
            rm->menu_font_large.drawCentered(320, 220, "Are You Sure?");
            rm->menu_font_large.drawCentered(282, 254, "Yes");
            rm->menu_font_large.drawCentered(356, 254, "No");

            spr_dialog.draw(fSelectedYes ? 250 : 326, 250, 192, 0, 64, 32);
        } else {
            if (currentEditor) {
                currentEditor->renderEdit(g_worldmap, {draw_offset_col, draw_offset_row}, {draw_offset_x, draw_offset_y}, *rm);
            }
            if (edit_mode == 9) {  // draw stages
                int color = SDL_MapRGB(blitdest->format, 0, 0, 255);
                for (short iRow = draw_offset_row; iRow < draw_offset_row + 15 && iRow < iWorldHeight; iRow++) {
                    for (short iCol = draw_offset_col; iCol <= draw_offset_col + 20 && iCol < iWorldWidth; iCol++) {
                        short iType = g_worldmap.tiles.at(iCol, iRow).iType - 6;

                        if (iType >= 0) {
                            short ix = (iCol - draw_offset_col) * TILESIZE + draw_offset_x;
                            short iy = (iRow - draw_offset_row) * TILESIZE + draw_offset_y;
                            SDL_Rect r = { ix, iy, 32, 32 };
                            SDL_FillRect(blitdest, &r, color);

                            rm->spr_worldforegroundspecial[0].draw(ix, iy, (iType % 10) << 5, (iType / 10) << 5, 32, 32);
                        }
                    }
                }

                if (iStageDisplay >= 0 && g_fShowStagePreviews) {
                    DisplayStageDetails(false, iStageDisplay, mouse_x, mouse_y);
                }
            }

            if (edit_mode == 5 || edit_mode == 8) {  // draw vehicles
                std::vector<WorldVehicle*>::iterator itr = vehiclelist.begin(), lim = vehiclelist.end();
                int color = SDL_MapRGB(blitdest->format, 0, 0, 128);
                while (itr != lim) {
                    WorldVehicle* vehicle = *itr;

                    short ix = (vehicle->currentTile.x - draw_offset_col) * TILESIZE + draw_offset_x;
                    short iy = (vehicle->currentTile.y - draw_offset_row) * TILESIZE + draw_offset_y;

                    SDL_Rect r = { ix, iy, 32, 32 };
                    SDL_FillRect(blitdest, &r, color);

                    rm->spr_worldvehicle[0].draw(ix, iy, vehicle->iDrawDirection << 5, vehicle->iDrawSprite << 5, 32, 32);

                    if (edit_mode == 5) {
                        rm->spr_worldforegroundspecial[0].draw(ix, iy, (vehicle->iActionId % 10) << 5, (vehicle->iActionId / 10) << 5, 32, 32);
                    }

                    if (edit_mode == 8) {
                        short iBoundary = vehicle->iBoundary - 1;
                        if (iBoundary == -1)
                            rm->spr_worldforegroundspecial[0].draw(ix, iy, 288, 288, 32, 32);
                        else
                            rm->spr_worldforegroundspecial[0].draw(ix, iy, (iBoundary % 10) << 5, (iBoundary / 10) << 5, 32, 32);
                    }

                    itr++;
                }

                if (edit_mode == 5) {
                    if (iStageDisplay >= 0) {
                        DisplayStageDetails(false, iStageDisplay, mouse_x, mouse_y);
                    }
                }
            }

            rm->menu_font_small.draw(0, 0, szEditModes[edit_mode]);

            if (fAutoPaint)
                rm->menu_font_small.draw(0, 16, "Auto Paint");

            rm->menu_font_small.drawRightJustified(640, 0, worldlist->currentPath().c_str());


            if (--g_musiccategorydisplaytimer > 0) {
                spr_dialog.draw(224, 176, 0, 0, 192, 128);
                rm->menu_font_small.drawCentered(320, 195, "Music Category");
                rm->menu_font_large.drawCentered(320, 220, g_szWorldMusicCategoryNames[g_worldmap.iMusicCategory]);

                rm->menu_font_small.drawCentered(320, 255, "Press 'R' Again");
                rm->menu_font_small.drawCentered(320, 270, "To Change");
            }

            DrawMessage();
        }

        gfx_flipscreen();

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
        rm->menu_font_large.drawCentered(320, 195, g_szMessageTitle.c_str());
        rm->menu_font_large.drawCentered(320, 220, g_szMessageLine[0].c_str());
        rm->menu_font_large.drawCentered(320, 240, g_szMessageLine[1].c_str());
        rm->menu_font_large.drawCentered(320, 260, g_szMessageLine[2].c_str());
    }
}

void GetForegroundTileValues(short iCol, short iRow, short iOldTiles[9])
{
    short iIndex = 0;
    for (short iAutoRow = iRow - 1; iAutoRow <= iRow + 1; iAutoRow++) {
        for (short iAutoCol = iCol - 1; iAutoCol <= iCol + 1; iAutoCol++) {
            if (iAutoRow >= 0 && iAutoRow < iWorldHeight && iAutoCol >= 0 && iAutoCol < iWorldWidth) {
                iOldTiles[iIndex++] = g_worldmap.tiles.at(iAutoCol, iAutoRow).iForegroundSprite;
            }
        }
    }

    for (short i = iIndex; i < 9; i++)
        iOldTiles[i] = 0;
}

void ReadVehiclesIntoEditor()
{
    std::vector<WorldVehicle*>::iterator itr = vehiclelist.begin(), lim = vehiclelist.end();
    while (itr != lim) {
        delete (*itr);
        itr++;
    }

    vehiclelist.clear();

    for (const WorldVehicle& vehicle : g_worldmap.vehicles) {
        WorldVehicle* vehiclecopy = new WorldVehicle();

        vehiclecopy->iDrawSprite = vehicle.iDrawSprite;
        vehiclecopy->iActionId = vehicle.iActionId;
        vehiclecopy->currentTile.x = vehicle.currentTile.x;
        vehiclecopy->currentTile.y = vehicle.currentTile.y;
        vehiclecopy->iMinMoves = vehicle.iMinMoves;
        vehiclecopy->iMaxMoves = vehicle.iMaxMoves;
        vehiclecopy->fSpritePaces = vehicle.fSpritePaces;
        vehiclecopy->iDrawDirection = vehicle.iDrawDirection;
        vehiclecopy->iBoundary = vehicle.iBoundary;

        vehiclelist.push_back(vehiclecopy);
    }
}

void WriteVehiclesIntoWorld()
{
    // Cleanup old vehicles
    g_worldmap.vehicles.clear();

    // Insert new vehicles
    g_worldmap.vehicles.reserve(vehiclelist.size());

    for (const WorldVehicle* vehicle : vehiclelist) {
        WorldVehicle vehiclecopy;
        vehiclecopy.iDrawSprite = vehicle->iDrawSprite;
        vehiclecopy.iActionId = vehicle->iActionId;
        vehiclecopy.currentTile.x = vehicle->currentTile.x;
        vehiclecopy.currentTile.y = vehicle->currentTile.y;
        vehiclecopy.iMinMoves = vehicle->iMinMoves;
        vehiclecopy.iMaxMoves = vehicle->iMaxMoves;
        vehiclecopy.fSpritePaces = vehicle->fSpritePaces;
        vehiclecopy.iDrawDirection = vehicle->iDrawDirection;
        vehiclecopy.iBoundary = vehicle->iBoundary;
        g_worldmap.vehicles.emplace_back(std::move(vehiclecopy));
    }
}

void AddVehicleToTile(short iCol, short iRow, short iType)
{
    std::vector<WorldVehicle*>::iterator itr = vehiclelist.begin(), lim = vehiclelist.end();
    WorldVehicle* newvehicle = NULL;
    while (itr != lim) {
        WorldVehicle* vehicle = *itr;
        if (vehicle->currentTile.x == iCol && vehicle->currentTile.y == iRow) {
            newvehicle = vehicle;
            break;
        }

        itr++;
    }

    if (!newvehicle) {
        newvehicle = new WorldVehicle();
        newvehicle->currentTile.x = iCol;
        newvehicle->currentTile.y = iRow;
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
        WorldVehicle* vehicle = *itr;
        if (vehicle->currentTile.x == iCol && vehicle->currentTile.y == iRow) {
            delete (*itr);

            itr = vehiclelist.erase(itr);
            lim = vehiclelist.end();

            return;
        }

        itr++;
    }
}

// Convert foreground sprite to match the background sprite
short AdjustForeground(short fgSprite, short iCol, short iRow)
{
    const WorldMapTile& tile = g_worldmap.tiles.at(iCol, iRow);
    return adjustedForeground(fgSprite, tile.iBackgroundSprite);
}

bool UpdateForeground(short iCol, short iRow)
{
    WorldMapTile& tile = g_worldmap.tiles.at(iCol, iRow);
    return updateForeground(tile);
}

bool UpdateCoastline(short iCol, short iRow)
{
    return updateCoastline(g_worldmap, {iCol, iRow});
}

bool AutoSetTile(short iCol, short iRow)
{
    return autoSetTile(g_worldmap, {iCol, iRow});
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

        // handle messages
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
                short iButtonX = bound_to_window_w(event.button.x);
                short iButtonY = bound_to_window_h(event.button.y);

                if (event.button.button == SDL_BUTTON_LEFT || event.button.button == SDL_BUTTON_RIGHT) {
                    if (g_worldmap.iNumInitialBonuses < 32) {
                        for (short iItem = 0; iItem < NUM_POWERUPS + NUM_WORLD_POWERUPS; iItem++) {
                            if (iButtonX >= rItemDst[iItem].x && iButtonX < rItemDst[iItem].w + rItemDst[iItem].x && iButtonY >= rItemDst[iItem].y && iButtonY < rItemDst[iItem].h + rItemDst[iItem].y) {
                                g_worldmap.iInitialBonuses[g_worldmap.iNumInitialBonuses++] = iItem;
                                break;
                            }
                        }
                    }

                    for (short iRemoveItem = 0; iRemoveItem < g_worldmap.iNumInitialBonuses; iRemoveItem++) {
                        if (iButtonX >= rPickedItemDst[iRemoveItem].x && iButtonX < rPickedItemDst[iRemoveItem].w + rPickedItemDst[iRemoveItem].x && iButtonY >= rPickedItemDst[iRemoveItem].y && iButtonY < rPickedItemDst[iRemoveItem].h + rPickedItemDst[iRemoveItem].y) {
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
            rm->spr_storedpoweruplarge.draw(rItemDst[iItem].x, rItemDst[iItem].y, iItem << 5, 0, 32, 32);
        }

        for (short iWorldItem = 0; iWorldItem < NUM_WORLD_POWERUPS; iWorldItem++) {
            rm->spr_worlditems.draw(rItemDst[iWorldItem + NUM_POWERUPS].x, rItemDst[iWorldItem + NUM_POWERUPS].y, iWorldItem << 5, 0, 32, 32);
        }

        for (short iPopup = 0; iPopup < 4; iPopup++) {
            rm->spr_worlditempopup.draw(0, 416 - (iPopup << 6), 0, 0, 320, 64);
            rm->spr_worlditempopup.draw(320, 416 - (iPopup << 6), 192, 0, 320, 64);
        }

        for (short iPickedItem = 0; iPickedItem < g_worldmap.iNumInitialBonuses; iPickedItem++) {
            short iPowerup = g_worldmap.iInitialBonuses[iPickedItem];
            if (iPowerup >= NUM_POWERUPS)
                rm->spr_worlditems.draw(rPickedItemDst[iPickedItem].x, rPickedItemDst[iPickedItem].y, (iPowerup - NUM_POWERUPS) << 5, 0, 32, 32);
            else
                rm->spr_storedpoweruplarge.draw(rPickedItemDst[iPickedItem].x, rPickedItemDst[iPickedItem].y, iPowerup << 5, 0, 32, 32);
        }

        rm->menu_font_small.drawRightJustified(640, 0, worldlist->currentPath().c_str());

        DrawMessage();
        gfx_flipscreen();

        int delay = WAITTIME - (SDL_GetTicks() - framestart);
        if (delay < 0)
            delay = 0;
        else if (delay > WAITTIME)
            delay = WAITTIME;

        SDL_Delay(delay);
    }

    return EDITOR_QUIT;
}


int enterEditor(EditorBase& editor)
{
    currentEditor = &editor;
    editor.onEnter();

    bool done = false;
    while (!done) {
        int framestart = SDL_GetTicks();

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                done = true;
                continue;
            }

            editor.handleSetupInput(event);
        }

        if (editor.isReady())
            return EDITOR_EDIT;

        if (editor.isSetupTransparent()) {
            drawmap(false, TILESIZE);
            menu_shade.draw(0, 0);
        } else {
            SDL_FillRect(screen, NULL, 0x0);
        }

        editor.renderSetup(*rm);

        DrawMessage();
        gfx_flipscreen();

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

        // handle messages
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
                    short iButtonX = bound_to_window_w(event.button.x) / TILESIZE;
                    short iButtonY = bound_to_window_h(event.button.y) / TILESIZE;

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

        rm->spr_worldforegroundspecial[0].draw(0, 0, 320, 224, 128, 32);

        DrawMessage();
        gfx_flipscreen();

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

        // handle messages
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
                    short iButtonX = bound_to_window_w(event.button.x) / TILESIZE;
                    short iButtonY = bound_to_window_h(event.button.y) / TILESIZE;

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

        rm->spr_worldforeground[0].draw(0, 0, 0, 0, 416, 480);
        rm->spr_worldforeground[0].draw(416, 0, 512, 0, 32, 480);

        DrawMessage();
        gfx_flipscreen();

        int delay = WAITTIME - (SDL_GetTicks() - framestart);
        if (delay < 0)
            delay = 0;
        else if (delay > WAITTIME)
            delay = WAITTIME;

        SDL_Delay(delay);
    }

    return EDITOR_QUIT;
}

// Display stages over vehicles
// allow setting of stages on vehicles

int editor_vehicles()
{
    if (g_worldmap.iNumStages <= 0) {
        SetDisplayMessage(120, "No Stages", "You need to create", "stages before you", "can create vehicles");
        return EDITOR_EDIT;
    }

    mCurrentMenu = &mVehicleMenu;
    mCurrentMenu->ResetMenu();

    bool done = false;

    miVehicleStageField->clear();

    for (short iStage = 0; iStage < g_worldmap.iNumStages; iStage++) {
        TourStop* ts = game_values.tourstops[iStage];
        char szStageName[256];
        sprintf(szStageName, "(%d) %s", iStage + 1, ts->szName);

        SF_ListItem<short>& item = miVehicleStageField->add(szStageName, iStage);
        item.iconOverride = ts->iStageType == 1 ? 24 : (ts->iMode >= 1000 ? ts->iMode - 975 : ts->iMode);
    }

    miVehicleStageField->setCurrentValue(g_wvVehicleStamp.iActionId);

    while (!done) {
        int framestart = SDL_GetTicks();

        game_values.playerInput.ClearPressedKeys(1);

        MenuCodeEnum code = MENU_CODE_NONE;

        // handle messages
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
                short iButtonX = bound_to_window_w(event.button.x);
                short iButtonY = bound_to_window_h(event.button.y);

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
            short iMaxMoves = miVehicleMaxMovesField->currentValue();
            if (miVehicleMinMovesField->currentValue() > iMaxMoves) {
                miVehicleMinMovesField->setCurrentValue(iMaxMoves);
            }
        } else if (MENU_CODE_VEHICLE_MAX_MOVES_CHANGED == code) {
            short iMinMoves = miVehicleMinMovesField->currentValue();
            if (miVehicleMaxMovesField->currentValue() < iMinMoves) {
                miVehicleMaxMovesField->setCurrentValue(iMinMoves);
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

        rm->menu_font_small.drawRightJustified(640, 0, worldlist->currentPath().c_str());

        DrawMessage();
        gfx_flipscreen();

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
    TourStop* ts = game_values.tourstops[iStageId];

    // If we're pointing to a new stage or no stage at all
    if (iStageId != iOldStageId || fForce) {
        if (ts->iStageType == 1) {
            if (sMapThumbnail) {
                SDL_FreeSurface(sMapThumbnail);
                sMapThumbnail = NULL;
            }
        } else {
            if (!ts->pszMapFile.empty()) {
                if (sMapThumbnail) {
                    SDL_FreeSurface(sMapThumbnail);
                    sMapThumbnail = NULL;
                }

                if (maplist->findexact(ts->pszMapFile.c_str(), false)) {
                    g_map->loadMap(maplist->currentFilename(), read_type_preview);
                    sMapThumbnail = g_map->createThumbnailSurface(true);
                } else {  // otherwise show a unknown map icon
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
        iMode = ts->iMode - 975;  // Convert mode id from map file to internal id for special stage modes

    // Make sure we're displaying it on the screen
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
        // Make sure we're displaying it on the screen
        if (iMouseY > 392)
            iMouseY = 392;

        spr_largedialog.draw(iMouseX, iMouseY, 0, 0, 116, 44);
        spr_largedialog.draw(iMouseX + 116, iMouseY, 140, 0, 116, 44);
        spr_largedialog.draw(iMouseX, iMouseY + 44, 0, 180, 116, 44);
        spr_largedialog.draw(iMouseX + 116, iMouseY + 44, 140, 180, 116, 44);
    }

    rm->menu_mode_large.draw(iMouseX + 16, iMouseY + 16, iMode << 5, 0, 32, 32);

    rm->menu_font_small.drawChopRight(iMouseX + 52, iMouseY + 16, 164, ts->szName);

    char szPrint[128];
    if (iMode != 24) {
        sprintf(szPrint, "Goal: %d", ts->iGoal);
        rm->menu_font_small.drawChopRight(iMouseX + 52, iMouseY + 34, 164, szPrint);

        sprintf(szPrint, "Points: %d", ts->iPoints);
        rm->menu_font_small.drawChopRight(iMouseX + 16, iMouseY + 176, 100, szPrint);

        sprintf(szPrint, "End: %s", ts->fEndStage ? "Yes" : "No");
        rm->menu_font_small.drawChopRight(iMouseX + 126, iMouseY + 176, 80, szPrint);

        for (short iBonus = 0; iBonus < ts->iNumBonuses; iBonus++) {
            WorldStageBonus* wsb = &ts->wsbBonuses[iBonus];
            rm->spr_worlditemsplace.draw(iMouseX + iBonus * 20 + 16, iMouseY + 194, wsb->iWinnerPlace * 20, 0, 20, 20);

            short iBonusIcon = wsb->iBonus;
            gfxSprite* spr_icon = iBonusIcon < NUM_POWERUPS ? &rm->spr_storedpowerupsmall : &rm->spr_worlditemssmall;
            spr_icon->draw(iMouseX + iBonus * 20 + 18, iMouseY + 196, (iBonusIcon < NUM_POWERUPS ? iBonusIcon : iBonusIcon - NUM_POWERUPS) << 4, 0, 16, 16);
        }

        if (sMapThumbnail) {
            SDL_Rect rSrc = { 0, 0, 160, 120 };
            SDL_Rect rDst = { iMouseX + 16, iMouseY + 52, 160, 120 };

            SDL_BlitSurface(sMapThumbnail, &rSrc, blitdest, &rDst);
        }
    } else {
        sprintf(szPrint, "Sort: %s", ts->iBonusType == 0 ? "Fixed" : "Random");
        rm->menu_font_small.drawChopRight(iMouseX + 52, iMouseY + 34, 164, szPrint);

        for (short iBonus = 0; iBonus < ts->iNumBonuses; iBonus++) {
            short iBonusIcon = ts->wsbBonuses[iBonus].iBonus;
            gfxSprite* spr_icon = NULL;
            short iSrcX = 0, iSrcY = 0;

            if (iBonusIcon < NUM_POWERUPS) {
                spr_icon = &rm->spr_storedpowerupsmall;
                iSrcX = iBonusIcon << 4;
                iSrcY = 0;
            } else if (iBonusIcon < NUM_POWERUPS + NUM_WORLD_POWERUPS) {
                spr_icon = &rm->spr_worlditemssmall;
                iSrcX = (iBonusIcon - NUM_POWERUPS) << 4;
                iSrcY = 0;
            } else if (iBonusIcon < NUM_POWERUPS + NUM_WORLD_POWERUPS + 10) {
                spr_icon = &rm->spr_worlditemssmall;
                iSrcX = (iBonusIcon - NUM_POWERUPS - NUM_WORLD_POWERUPS) << 4;
                iSrcY = 16;
            } else {
                spr_icon = &rm->spr_worlditemssmall;
                iSrcX = (iBonusIcon - NUM_POWERUPS - NUM_WORLD_POWERUPS - 10) << 4;
                iSrcY = 32;
            }

            spr_icon->draw(iMouseX + iBonus * 20 + 18, iMouseY + 52, iSrcX, iSrcY, 16, 16);
        }
    }
}

int g_iNumGameModeSettings[GAMEMODE_LAST] = { 2, 2, 3, 4, 3, 10, 9, 6, 2, 1, 3, 5, 3, 3, 0, 22, 6, 4, 3, 4, 4, 3 };

SDL_Rect rItemDst[NUM_WORLD_ITEMS];

void SetBonusString(char* szString, short iPlace, short iItem, short iStageType)
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

void TestAndSetBonusItem(TourStop* ts, short iPlace, short iButtonX, short iButtonY)
{
    short iMaxBonusesAllowed = 10;

    if (ts->iStageType == 1)
        iMaxBonusesAllowed = MAX_BONUS_CHESTS;

    if (ts->iNumBonuses < iMaxBonusesAllowed) {
        short iNumSelectableItems = NUM_WORLD_ITEMS;
        if (ts->iStageType == 0)
            iNumSelectableItems = NUM_POWERUPS + NUM_WORLD_POWERUPS;

        for (short iItem = 0; iItem < iNumSelectableItems; iItem++) {
            if (iButtonX >= rItemDst[iItem].x && iButtonX < rItemDst[iItem].w + rItemDst[iItem].x && iButtonY >= rItemDst[iItem].y && iButtonY < rItemDst[iItem].h + rItemDst[iItem].y) {
                // If this is a normal stage, then alert the player that they need to select the place for this item
                if (ts->iStageType == 0 && iPlace == 0) {
                    SetDisplayMessage(120, "Use Number Keys", "Hover over item", "use keys 1 to 4", "to select bonus");
                    return;
                }

                // Set the bonus item for the place selected
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

void AdjustBonuses(TourStop* ts)
{
    // No need to do anything if we were a stage and we're still a stage
    //(or a house and still a house)
    if (ts->iStageType == iLastStageType)
        return;

    if (ts->iStageType == 0) {
        // Remove any score bonuses
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

        // Add places to bonuses
        for (short iBonus = 0; iBonus < ts->iNumBonuses; iBonus++) {
            if (ts->wsbBonuses[iBonus].iWinnerPlace < 0 || ts->wsbBonuses[iBonus].iWinnerPlace > 3)
                ts->wsbBonuses[iBonus].iWinnerPlace = 0;

            SetBonusString(ts->wsbBonuses[iBonus].szBonusString, ts->wsbBonuses[iBonus].iWinnerPlace + 1, ts->wsbBonuses[iBonus].iBonus, ts->iStageType);
        }
    } else if (ts->iStageType == 1) {
        // Cap the number of
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
    // Set the number of game mode settings to the maximum so we write them all out
    game_values.tourstops[iEditStage]->fUseSettings = true;
    game_values.tourstops[iEditStage]->iNumUsedSettings = g_iNumGameModeSettings[game_values.tourstops[iEditStage]->iMode];

    // Copy the working values back into the structure that will be saved
    memcpy(&game_values.tourstops[iEditStage]->gmsSettings, &game_values.gamemodemenusettings, sizeof(GameModeSettings));

    if (game_values.tourstops[iEditStage]->iMode >= 25 && game_values.tourstops[iEditStage]->iMode <= 27)
        game_values.tourstops[iEditStage]->iMode += 975;
}

void EditStage(short iEditStage)
{
    memcpy(&game_values.gamemodemenusettings, &game_values.tourstops[iEditStage]->gmsSettings, sizeof(GameModeSettings));

    // Set fields to write data to the selected stage
    TourStop* ts = game_values.tourstops[iEditStage];

    miModeField->setOutputPtr(&ts->iMode);
    miNameField->SetData(ts->szName, 128);

    miPointsField->setOutputPtr(&ts->iPoints);
    miFinalStageField->setOutputPtr(&ts->fEndStage);

    if (ts->pszMapFile.empty())
        ts->pszMapFile = maplist->currentShortmapname();

    miMapField->SetMap(ts->pszMapFile.c_str(), false);

    miBonusType->setOutputPtr(&ts->iBonusType);
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

    // Show fields applicable for this mode
    miPointsField->setVisible(iStageType == 0);
    miFinalStageField->setVisible(iStageType == 0);
    miMapField->setVisible(iStageType == 0);

    miBonusType->setVisible(iStageType == 1);
    miBonusTextField[0]->setVisible(iStageType == 1);
    miBonusTextField[1]->setVisible(iStageType == 1);
    miBonusTextField[2]->setVisible(iStageType == 1);
    miBonusTextField[3]->setVisible(iStageType == 1);
    miBonusTextField[4]->setVisible(iStageType == 1);

    miSpecialGoalField[0]->setVisible(iMode == 1000);
    miSpecialGoalField[1]->setVisible(iMode == 1001);
    miSpecialGoalField[2]->setVisible(iMode == 1002);

    miBonusItemsButton->SetPosition(430, iStageType == 0 ? 220 : 340);

    if (iStageType == 0 && iMode >= 0 && iMode < GAMEMODE_LAST) {
        miModeField->setCurrentValue(iMode);

        miModeSettingsButton->setVisible(iMode != game_mode_owned);

        for (short iGameMode = 0; iGameMode < GAMEMODE_LAST; iGameMode++) {
            miGoalField[iGameMode]->setVisible(iMode == iGameMode);
        }

        miGoalField[iMode]->setOutputPtr(&game_values.tourstops[iEditStage]->iGoal);
        miGoalField[iMode]->setCurrentValue(game_values.tourstops[iEditStage]->iGoal);
        miPointsField->setCurrentValue(game_values.tourstops[iEditStage]->iPoints);
        miFinalStageField->setCurrentValue(game_values.tourstops[iEditStage]->fEndStage ? 1 : 0);

        game_values.tourstops[iEditStage]->fUseSettings = true;
        game_values.tourstops[iEditStage]->iNumUsedSettings = g_iNumGameModeSettings[iMode];
    } else {
        // Show the settings button for boss mode
        miModeSettingsButton->setVisible(iMode == 1001);

        for (short iGameMode = 0; iGameMode < GAMEMODE_LAST; iGameMode++) {
            miGoalField[iGameMode]->setVisible(false);
        }

        if (iStageType == 1) {  // Bonus House
            miModeField->setCurrentValue(24);
        } else if (iMode >= 1000 && iMode <= 1002) {  // Pipe, Boss and Boxes Game
            miModeField->setCurrentValue(iMode - 975);
            miSpecialGoalField[iMode - 1000]->setOutputPtr(&game_values.tourstops[iEditStage]->iGoal);
            miSpecialGoalField[iMode - 1000]->setCurrentValue(game_values.tourstops[iEditStage]->iGoal);
            miPointsField->setCurrentValue(game_values.tourstops[iEditStage]->iPoints);
            miFinalStageField->setCurrentValue(game_values.tourstops[iEditStage]->fEndStage ? 1 : 0);

            // Have to set this back again since the setCurrentValue() above will set it to 25
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

void NewStage(short* iEditStage)
{
    TourStop* ts = new TourStop();

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

    sprintf(ts->szName, "Tour Stop %d", game_values.tourstops.size() + 1);

    ts->fEndStage = false;

    // Copy in default values first
    memcpy(&ts->gmsSettings, &game_values.gamemodemenusettings, sizeof(GameModeSettings));

    game_values.tourstops.push_back(ts);
    g_worldmap.iNumStages++;

    *iEditStage = game_values.tourstops.size() - 1;
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

        // Reset the keys that were down the last frame
        game_values.playerInput.ClearPressedKeys(1);

        MenuCodeEnum code = MENU_CODE_NONE;

        // handle messages
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT: {
                done = true;
                break;
            }

            case SDL_KEYDOWN: {
                // Do not allow saving world by pressing 's' key
                // World data structures are not in the correct state to be saved
                // until exiting the stage editor menu in (MENU_CODE_EXIT_APPLICATION == code) below

                if (iEditStage == -1 && event.key.keysym.sym == SDLK_n) {
                    NewStage(&iEditStage);
                } else if ((event.key.keysym.sym == SDLK_ESCAPE || event.key.keysym.sym == SDLK_e) && iEditStage == -1) {
                    if (g_worldmap.iNumStages == 0) {
                        edit_mode = 1;
                    } else if (set_tile < 6 || set_tile >= g_worldmap.iNumStages + 6) {
                        set_tile = 6;
                        edit_mode = 9;
                    } else {
                        edit_mode = 9;  // change to edit mode using stages
                    }

                    return EDITOR_EDIT;
                } else if (mCurrentMenu == &mBonusItemPicker && event.key.keysym.sym >= SDLK_1 && event.key.keysym.sym <= SDLK_4) {
                    TourStop* ts = game_values.tourstops[iEditStage];
                    if (ts->iStageType == 0) {
                        short iPlace = event.key.keysym.sym - SDLK_1 + 1;

                        TestAndSetBonusItem(ts, iPlace, mouse_x, mouse_y);
                    }
                } else if ((event.key.keysym.sym == SDLK_PAGEUP && iEditStage > 0) || (event.key.keysym.sym == SDLK_PAGEDOWN && iEditStage < g_worldmap.iNumStages - 1)) {
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
                short iTileX = bound_to_window_w(event.button.x) / TILESIZE;
                short iTileY = bound_to_window_h(event.button.y) / TILESIZE;
                short iButtonX = bound_to_window_w(event.button.x);
                short iButtonY = bound_to_window_h(event.button.y);

                if (event.button.button == SDL_BUTTON_LEFT) {
                    // Stages
                    if (iEditStage == -1) {
                        if (iTileX >= 0 && iTileX < g_worldmap.iNumStages - (iTileY * 20) && iTileY >= 0 && iTileY <= (g_worldmap.iNumStages - 1) / 20) {
                            set_tile = iTileX + (iTileY * 20) + 6;

                            edit_mode = 9;  // change to edit mode using warps

                            // The user must release the mouse button before trying to add a tile
                            ignoreclick = true;

                            return EDITOR_EDIT;
                        }
                        // New stage button
                        else if (iButtonX >= 256 && iButtonX < 384 && iButtonY >= 420 && iButtonY < 452) {
                            NewStage(&iEditStage);
                        }
                    } else if (mCurrentMenu == &mBonusItemPicker) {
                        TourStop* ts = game_values.tourstops[iEditStage];

                        // See if we clicked an item and add it if we did
                        TestAndSetBonusItem(ts, 0, iButtonX, iButtonY);

                        // See if we clicked an already added item and remove it
                        for (short iRemoveItem = 0; iRemoveItem < ts->iNumBonuses; iRemoveItem++) {
                            SDL_Rect* rects = rStageBonusDst;

                            if (ts->iStageType == 1)
                                rects = rHouseBonusDst;

                            if (iButtonX >= rects[iRemoveItem].x && iButtonX < rects[iRemoveItem].w + rects[iRemoveItem].x && iButtonY >= rects[iRemoveItem].y && iButtonY < rects[iRemoveItem].h + rects[iRemoveItem].y) {
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
                update_mouse_coords();
                iStageDisplay = -1;

                if (iEditStage == -1) {
                    //						if (event.button.x >= 0 && event.button.y >= 0)
                    {
                        short iMouseX = bound_to_window_w(event.button.x) / TILESIZE;
                        short iMouseY = bound_to_window_h(event.button.y) / TILESIZE;

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
                // Save the current stage
                SaveStage(iEditStage);

                // We are no longer working on a specific stage
                iEditStage = -1;
            } else if (MENU_CODE_MODE_CHANGED == code) {
                short iMode = miModeField->currentValue();

                miPointsField->setVisible(iMode != 24);
                miFinalStageField->setVisible(iMode != 24);

                miMapField->setVisible(iMode != 24);

                miBonusType->setVisible(iMode == 24);
                miBonusTextField[0]->setVisible(iMode == 24);
                miBonusTextField[1]->setVisible(iMode == 24);
                miBonusTextField[2]->setVisible(iMode == 24);
                miBonusTextField[3]->setVisible(iMode == 24);
                miBonusTextField[4]->setVisible(iMode == 24);

                miSpecialGoalField[0]->setVisible(iMode == 25);
                miSpecialGoalField[1]->setVisible(iMode == 26);
                miSpecialGoalField[2]->setVisible(iMode == 27);

                miBonusItemsButton->SetPosition(430, iMode != 24 ? 220 : 340);

                if (iMode >= 0 && iMode < GAMEMODE_LAST) {
                    miModeSettingsButton->setVisible(iMode != game_mode_owned);

                    for (short iGameMode = 0; iGameMode < GAMEMODE_LAST; iGameMode++) {
                        miGoalField[iGameMode]->setVisible(iMode == iGameMode);
                    }

                    miGoalField[iMode]->setOutputPtr(&game_values.tourstops[iEditStage]->iGoal);
                    miGoalField[iMode]->updateOutput();

                    game_values.tourstops[iEditStage]->iStageType = 0;

                    game_values.tourstops[iEditStage]->fUseSettings = true;
                    game_values.tourstops[iEditStage]->iNumUsedSettings = g_iNumGameModeSettings[iMode];
                } else {
                    // Show the settings button for boss mode
                    miModeSettingsButton->setVisible(iMode == 26);

                    for (short iGameMode = 0; iGameMode < GAMEMODE_LAST; iGameMode++)
                        miGoalField[iGameMode]->setVisible(false);

                    if (iMode == 24) {
                        game_values.tourstops[iEditStage]->iStageType = 1;
                        game_values.tourstops[iEditStage]->iBonusTextLines = 5;
                    } else if (iMode >= 25 && iMode <= 27) {
                        game_values.tourstops[iEditStage]->iStageType = 0;
                    }
                }

                // Removes bonuses if we went from a stage to a bonus house
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

                // Look to see if this is the boss mode and go to boss settings
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
                miDeleteStageDialogImage->setVisible(true);
                miDeleteStageDialogAreYouText->setVisible(true);
                miDeleteStageDialogSureText->setVisible(true);
                miDeleteStageDialogYesButton->setVisible(true);
                miDeleteStageDialogNoButton->setVisible(true);

                EnableStageMenu(false);

                mStageSettingsMenu.RememberCurrent();

                mStageSettingsMenu.setInitialFocus(miDeleteStageDialogNoButton);
                mStageSettingsMenu.SetCancelCode(MENU_CODE_DELETE_STAGE_NO);
                mStageSettingsMenu.ResetMenu();
            } else if (MENU_CODE_DELETE_STAGE_YES == code || MENU_CODE_DELETE_STAGE_NO == code) {
                miDeleteStageDialogImage->setVisible(false);
                miDeleteStageDialogAreYouText->setVisible(false);
                miDeleteStageDialogSureText->setVisible(false);
                miDeleteStageDialogYesButton->setVisible(false);
                miDeleteStageDialogNoButton->setVisible(false);

                mStageSettingsMenu.setInitialFocus(miNameField);
                mStageSettingsMenu.SetCancelCode(MENU_CODE_EXIT_APPLICATION);
                mStageSettingsMenu.RestoreCurrent();

                // Yes was selected to delete this stage
                if (MENU_CODE_DELETE_STAGE_YES == code) {
                    // Scan the grid of stages and remove any references to this stage
                    // and decrement stage numbers greater than this stage
                    for (short iRow = 0; iRow < iWorldHeight; iRow++) {
                        for (short iCol = 0; iCol < iWorldWidth; iCol++) {
                            if (g_worldmap.tiles.at(iCol, iRow).iType == iEditStage + 6) {
                                g_worldmap.tiles.at(iCol, iRow).iType = 0;
                            } else if (g_worldmap.tiles.at(iCol, iRow).iType > iEditStage + 6) {
                                g_worldmap.tiles.at(iCol, iRow).iType--;
                            }
                        }
                    }

                    // Scan vehicles and remove references to deleted stage
                    std::vector<WorldVehicle*>::iterator itrVehicle = vehiclelist.begin(), limVehicle = vehiclelist.end();
                    while (itrVehicle != limVehicle) {
                        WorldVehicle* vehicle = *itrVehicle;
                        if (vehicle->iActionId == iEditStage) {
                            RemoveVehicleFromTile(vehicle->currentTile.x, vehicle->currentTile.y);
                        } else if (vehicle->iActionId > iEditStage) {
                            vehicle->iActionId--;
                        }

                        itrVehicle++;
                    }

                    // Remove stage from tourstops vector
                    std::vector<TourStop*>::iterator itr = game_values.tourstops.begin(), lim = game_values.tourstops.end();

                    short iIndex = 0;
                    while (itr != lim) {
                        if (iIndex == iEditStage) {
                            delete (*itr);

                            game_values.tourstops.erase(itr);
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

                SDL_Rect r = { ix, iy, 32, 32 };
                SDL_FillRect(blitdest, &r, color);

                rm->spr_worldforegroundspecial[0].draw(ix, iy, (iStage % 10) << 5, (iStage / 10) << 5, 32, 32);
            }

            if (iStageDisplay >= 0) {
                DisplayStageDetails(fForceStageDisplay, iStageDisplay, mouse_x, mouse_y);
                fForceStageDisplay = false;
            }

            // Display New button
            rm->spr_selectfield.draw(256, 420, 0, 0, 64, 32);
            rm->spr_selectfield.draw(320, 420, 448, 0, 64, 32);

            rm->menu_font_large.drawCentered(320, 425, "New Stage");

            rm->menu_font_small.draw(0, 480 - rm->menu_font_small.getHeight(), "[LMB] Select Stage, [RMB] Edit Stage, [n] New Stage");
        } else {
            mCurrentMenu->Update();
            mCurrentMenu->Draw();

            if (mCurrentMenu != &mBonusItemPicker) {
                short ix = 20;
                short iy = 20;

                SDL_Rect r = { ix, iy, 32, 32 };
                SDL_FillRect(blitdest, &r, color);

                rm->spr_worldforegroundspecial[0].draw(ix, iy, (iEditStage % 10) << 5, (iEditStage / 10) << 5, 32, 32);
            }
        }

        if (mCurrentMenu == &mBonusItemPicker) {
            TourStop* ts = game_values.tourstops[iEditStage];

            // Game powerups
            for (short iItem = 0; iItem < NUM_POWERUPS; iItem++) {
                rm->spr_storedpoweruplarge.draw(rItemDst[iItem].x, rItemDst[iItem].y, iItem << 5, 0, 32, 32);
            }

            // World Powerups
            for (short iWorldItem = 0; iWorldItem < NUM_WORLD_POWERUPS; iWorldItem++) {
                rm->spr_worlditems.draw(rItemDst[iWorldItem + NUM_POWERUPS].x, rItemDst[iWorldItem + NUM_POWERUPS].y, iWorldItem << 5, 0, 32, 32);
            }

            // Score Bonuses
            if (ts->iStageType == 1) {
                for (short iScoreBonus = 0; iScoreBonus < NUM_WORLD_SCORE_BONUSES; iScoreBonus++) {
                    rm->spr_worlditems.draw(rItemDst[iScoreBonus + NUM_POWERUPS + NUM_WORLD_POWERUPS].x, rItemDst[iScoreBonus + NUM_POWERUPS + NUM_WORLD_POWERUPS].y, iScoreBonus < 10 ? iScoreBonus << 5 : (iScoreBonus - 10) << 5, iScoreBonus < 10 ? 32 : 64, 32, 32);
                }
            }

            // Draw background container
            rm->spr_worlditempopup.draw(0, 344, 0, 0, 320, 64);
            rm->spr_worlditempopup.draw(320, 344, 192, 0, 320, 64);

            SDL_Rect* rects = rStageBonusDst;

            if (ts->iStageType == 1)
                rects = rHouseBonusDst;

            for (short iPickedItem = 0; iPickedItem < ts->iNumBonuses; iPickedItem++) {
                short iBonus = ts->wsbBonuses[iPickedItem].iBonus;
                short iPlace = ts->wsbBonuses[iPickedItem].iWinnerPlace;

                // Draw place behind bonus
                if (ts->iStageType == 0)
                    rm->spr_worlditempopup.draw(rects[iPickedItem].x - 8, rects[iPickedItem].y - 8, iPlace * 48, 256, 48, 48);

                if (iBonus >= NUM_POWERUPS + NUM_WORLD_POWERUPS) {
                    short iBonusIndex = iBonus - NUM_POWERUPS - NUM_WORLD_POWERUPS;
                    rm->spr_worlditems.draw(rects[iPickedItem].x, rects[iPickedItem].y, iBonusIndex < 10 ? iBonusIndex << 5 : (iBonusIndex - 10) << 5, iBonusIndex < 10 ? 32 : 64, 32, 32);
                } else if (iBonus >= NUM_POWERUPS) {
                    rm->spr_worlditems.draw(rects[iPickedItem].x, rects[iPickedItem].y, (iBonus - NUM_POWERUPS) << 5, 0, 32, 32);
                } else {
                    rm->spr_storedpoweruplarge.draw(rects[iPickedItem].x, rects[iPickedItem].y, iBonus << 5, 0, 32, 32);
                }
            }

            if (ts->iStageType == 0)
                rm->menu_font_small.draw(0, 480 - rm->menu_font_small.getHeight(), "[1-4] Select Items, [LMB] Remove Items");
            else
                rm->menu_font_small.draw(0, 480 - rm->menu_font_small.getHeight(), "[LMB] Select Items, [LMB] Remove Items");
        }

        rm->menu_font_small.drawRightJustified(640, 0, worldlist->currentPath().c_str());

        DrawMessage();
        gfx_flipscreen();

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
    drawmap(false, TILESIZE);
    menu_shade.draw(0, 0);
    rm->menu_font_large.drawCentered(320, 15, "Help");

    constexpr std::array<const char* const, 24> column1 {
        "Modes:",
        "[1] - Water Mode",
        "[2] - Land Mode",
        "[3] - Stage Objects Mode",
        "[4] - Path Mode",
        "[5] - Objects Mode",
        "[6] - Bridges Mode",
        "[p] - Connection Mode",
        "[w] - Warp Mode",
        "[v] - Vehicle",
        "      [c] - Copy Vehicle",
        "[t] - Start and Doors",
        "[b] - Vehicle Boundaries",
        "[i] - Initial Powerups",
        "[e] - Edit Stages",
        "",
        "File:",
        "[n] - New World",
        "[s] - Save World",
        "[shift] + [s] - Save As",
        "[f] - Find World",
        "[shift] + [f] - New Search",
        "[pageup] - Go To Previous World",
        "[pagedown] - Go To Next World",
    };
    int offsety = 55;
    int offsetx = 30;
    for (const char* const line : column1) {
        rm->menu_font_small.drawC(offsetx, offsety, line);
        offsety += rm->menu_font_small.getHeight() + 2;
    }

    constexpr std::array<const char* const, 13> column2 {
        "Place Tiles:",
        "[Left Mouse Button] - Place Item",
        "[Right Mouse Button] - Remove Item",
        "",
        // "Move Mode:",
        // "[Right Mouse Button] - Select Area",
        // "[Left Mouse Button] - Unselect Area",
        // "Select And Drag - Move Selections",
        // "Hold [shift] - Multiple Selections",
        // "Hold [ctrl] - Freehand Selections",
        // "[delete] - Delete Selection",
        // "[c] - Copy Selection",
        // "",
        "Miscellaneous:",
        "[r] - Change Music Category",
        "[Arrow Keys] - Navigate World",
        "[a] - Automatic Path/Land",
        "[k] - Resize World",
        "[ctrl] + [delete] - Clear All",
        "[insert] - Screenshot",
        "[alt] + [enter] - Full Screen/Window",
        "[space] - Toggle Stage Previews",
    };
    offsetx = 300;
    offsety = 55;
    for (const char* const line : column2) {
        rm->menu_font_small.draw(offsetx, offsety, line);
        offsety += rm->menu_font_small.getHeight() + 2;
    }

    gfx_flipscreen();

    while (true) {
        int framestart = SDL_GetTicks();

        // handle messages
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
        game_values.worldindex = worldlist->currentIndex();
        savecurrentworld();
        loadcurrentworld();
    }

    return 0;
}

bool dialog(const char* title, const char* instructions, char* input, int inputsize)
{
    unsigned int currentChar = 0;

    drawmap(false, TILESIZE);
    menu_shade.draw(0, 0);
    spr_dialog.draw(224, 176, 0, 0, 192, 128);
    rm->menu_font_large.drawCentered(320, 200, title);
    rm->menu_font_small.draw(240, 235, instructions);
    rm->menu_font_small.drawRightJustified(640, 0, worldlist->currentPath().c_str());
    gfx_flipscreen();

    while (true) {
        int framestart = SDL_GetTicks();

        // handle messages
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
                        input[currentChar - 1] = '\0';

                        drawmap(false, TILESIZE);
                        menu_shade.draw(0, 0);
                        spr_dialog.draw(224, 176, 0, 0, 192, 128);
                        rm->menu_font_large.drawCentered(320, 200, title);
                        rm->menu_font_small.draw(240, 235, instructions);
                        rm->menu_font_small.draw(240, 255, input);
                        rm->menu_font_small.drawRightJustified(640, 0, worldlist->currentPath().c_str());
                        gfx_flipscreen();

                        currentChar--;
                    }
                } else {

                    /* I realize the if statement below is long and can be substituted with
                    the function isalnum(event.key.keysym.sym) but I did it this way because
                    isalnum acts funny (ie wrong) when the number pad is pressed. */
                    if ((isdigit(event.key.keysym.sym) || event.key.keysym.sym == 45 || event.key.keysym.sym == 32 || event.key.keysym.sym == 61 || (event.key.keysym.sym >= 95 && event.key.keysym.sym <= 122)) && currentChar < (unsigned)inputsize - 1) {
                        // insert character into fileName and onScreenText and increment current char
                        Uint8 key = event.key.keysym.sym;

#if defined(USE_SDL2) || defined(__EMSCRIPTEN__)
                        const Uint8* keystate = SDL_GetKeyboardState(NULL);
#else
                        Uint8* keystate = SDL_GetKeyState(NULL);
#endif
                        if (CheckKey(keystate, SDLK_LSHIFT) || CheckKey(keystate, SDLK_RSHIFT)) {
                            if (event.key.keysym.sym == 45)
                                key = 95;
                            else if (event.key.keysym.sym >= 95 && event.key.keysym.sym <= 122)
                                key -= 32;  // Capitalize
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
                        rm->menu_font_large.drawCentered(320, 200, title);
                        rm->menu_font_small.draw(240, 235, instructions);
                        rm->menu_font_small.draw(240, 255, input);
                        rm->menu_font_small.drawRightJustified(640, 0, worldlist->currentPath().c_str());
                        gfx_flipscreen();
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
    // char mapLocation[FILEBUFSIZE] = "maps/";

    if (dialog("Find Map", "Enter name:", fileName, 64)) {
        strcpy(findstring, fileName);

        if (worldlist->find(findstring)) {
            game_values.worldindex = worldlist->currentIndex();
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
    game_values.worldindex = worldlist->currentIndex();
    g_worldmap = WorldMap(worldlist->at(game_values.worldindex), TILESIZE);
    ReadVehiclesIntoEditor();

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
    g_worldmap.Save(worldlist->at(game_values.worldindex));
    return 0;
}

void SetDisplayMessage(short iTime,
    const char* szTitle,
    const char* szLine1,
    const char* szLine2,
    const char* szLine3)
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
            game_values.worldindex = worldlist->currentIndex();
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

    if (dialog("New World", "Enter name:", fileName, 64) && dialog("New World", "Width:", szWidth, 4) && dialog("New World", "Height:", szHeight, 4)) {
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

        g_worldmap = WorldMap(iWidth, iHeight);
        worldlist->add(convertPath(strcat(worldLocation, strcat(fileName, ".txt"))).c_str());
        worldlist->find(fileName);
        game_values.worldindex = worldlist->currentIndex();
        savecurrentworld();
        loadcurrentworld();
    }

    return 0;
}


int resize_world()
{
    char szWidth[5], szHeight[5];

    if (dialog("Resize World", "Width:", szWidth, 4) && dialog("Resize World", "Height:", szHeight, 4)) {
        short iWidth = atoi(szWidth);
        short iHeight = atoi(szHeight);

        if (iWidth < 1)
            iWidth = 1;

        if (iHeight < 1)
            iHeight = 1;

        std::vector<WorldVehicle*>::iterator itrVehicle = vehiclelist.begin(), limVehicle = vehiclelist.end();
        while (itrVehicle != limVehicle) {
            if ((*itrVehicle)->currentTile.x >= iWidth || (*itrVehicle)->currentTile.y >= iHeight) {
                RemoveVehicleFromTile((*itrVehicle)->currentTile.x, (*itrVehicle)->currentTile.y);
                // List was modified, restart.
                itrVehicle = vehiclelist.begin();
                limVehicle = vehiclelist.end();
            } else
                itrVehicle++;
        }

        g_worldmap.Resize(iWidth, iHeight);

        savecurrentworld();
        loadcurrentworld();
    }

    return 0;
}

// take screenshots in full and thumbnail sizes
void takescreenshot()
{
    short iTileSizes[3] = { 32, 16, 8 };

    for (short iScreenshotSize = 0; iScreenshotSize < 3; iScreenshotSize++) {
        short iTileSize = iTileSizes[iScreenshotSize];
        g_worldmap = WorldMap(worldlist->at(game_values.worldindex), iTileSize);

        short w, h;
        g_worldmap.GetWorldSize(&w, &h);

        // Draw most of the world to screenshot
        SDL_Surface* sScreenshot = SDL_CreateRGBSurface(screen->flags, iTileSize * w, iTileSize * h, screen->format->BitsPerPixel, 0, 0, 0, 0);
        blitdest = sScreenshot;

        g_worldmap.DrawMapToSurface(sScreenshot);

        // Draw vehicles to screenshot
        std::vector<WorldVehicle*>::iterator itr = vehiclelist.begin(), lim = vehiclelist.end();
        while (itr != lim) {
            WorldVehicle* vehicle = *itr;

            short ix = vehicle->currentTile.x * iTileSize;
            short iy = vehicle->currentTile.y * iTileSize;

            rm->spr_worldvehicle[iScreenshotSize].draw(ix, iy, vehicle->iDrawDirection * iTileSize, vehicle->iDrawSprite * iTileSize, iTileSize, iTileSize);

            itr++;
        }

        // Draw warps to screenshot
        editorWarps.renderScreenshot(g_worldmap, iScreenshotSize);

        // Save the screenshot with the same name as the map file
        std::string szSaveFile("worlds/screenshots/");
        szSaveFile += GetNameFromFileName(worldlist->currentPath());

        if (iTileSize == PREVIEWTILESIZE)
            szSaveFile += "_preview";
        else if (iTileSize == THUMBTILESIZE)
            szSaveFile += "_thumb";

#ifdef PNG_SAVE_FORMAT
        szSaveFile += ".png";
        IMG_SavePNG(sScreenshot, convertPath(szSaveFile).c_str());
#else
        szSaveFile += ".bmp";
        SDL_SaveBMP(sScreenshot, convertPath(szSaveFile).c_str());
#endif

        SDL_FreeSurface(sScreenshot);
    }

    g_worldmap = WorldMap(worldlist->at(game_values.worldindex), iTileSizes[0]);
    blitdest = screen;
}
