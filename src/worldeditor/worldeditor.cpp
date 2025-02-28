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
#include "EditorBridges.h"
#include "EditorPaths.h"
#include "EditorPathSprites.h"
#include "EditorStages.h"
#include "EditorStageMarkers.h"
#include "EditorStartItems.h"
#include "EditorStructures.h"
#include "EditorTileType.h"
#include "EditorVehicles.h"
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

gfxSprite menu_shade;
gfxSprite spr_largedialog;

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

EditorBackground editorBackground;
EditorBridges editorBridges;
EditorPaths editorPaths;
EditorPathSprites editorPathSprites;
EditorStages editorStages;
EditorStageMarkers editorStageMarkers;
EditorStartItems editorStartItems;
EditorStructures editorStructures;
EditorTileType editorTileType;
EditorVehicles editorVehicles;
EditorVehicleBoundaries editorVehicleBoundaries;
EditorWater editorWater;
EditorWarps editorWarps;
constexpr std::array<EditorBase*, 13> allEditors {
    &editorBackground,
    &editorBridges,
    &editorPaths,
    &editorPathSprites,
    &editorStages,
    &editorStageMarkers,
    &editorStartItems,
    &editorStructures,
    &editorTileType,
    &editorVehicles,
    &editorVehicleBoundaries,
    &editorWarps,
    &editorWater,
};
EditorBase* currentEditor = nullptr;
int enterEditor(EditorBase& editor);

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

bool g_fFullScreen = false;
bool g_fShowStagePreviews = true;

// Stage Mode Menu
UI_Menu* mCurrentMenu;



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

    rm->spr_dialog.init(convertPath("gfx/leveleditor/leveleditor_dialog.png"), colors::MAGENTA, 255);
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

    editorStages.setupMenu(*rm);
    editorVehicles.setupMenu(*rm);

#ifndef USE_SDL2
    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
#endif

    printf("\n---------------- ready, steady, go! ----------------\n");

    editorBackground.setAutoPaint(fAutoPaint);
    editorPaths.setAutoPaint(fAutoPaint);
    editorPathSprites.setAutoPaint(fAutoPaint);

    printf("entering world editor loop...\n");
    EditorBase* prevEditor = nullptr;
    done = false;
    while (!done) {
        if (state != EDITOR_EDIT) {
            prevEditor = currentEditor;
            currentEditor = nullptr;
        }

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
            state = enterEditor(editorStructures);
            edit_mode = 1;
            break;

        case EDITOR_BRIDGES:
            state = enterEditor(editorBridges);
            edit_mode = 1;
            break;

        case EDITOR_VEHICLES:
            if (g_worldmap.stageCount() <= 0) {
                SetDisplayMessage(120, "No Stages", "You need to create", "stages before you", "can create vehicles");
                state = EDITOR_EDIT;
            } else {
                state = enterEditor(editorVehicles);
                edit_mode = 5;
            }
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
            state = enterEditor(editorStartItems);
            currentEditor = prevEditor;
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
            state = enterEditor(editorStages);
            edit_mode = (g_worldmap.stageCount() == 0) ? 1 : 9;
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
                        const Vec2s tilePos(iCol, iRow);

                        const auto it = std::find_if(
                            g_worldmap.getVehicles().cbegin(),
                            g_worldmap.getVehicles().cend(),
                            [tilePos](const WorldVehicle& vehicle) { return vehicle.getCurrentTile() == tilePos; });
                        if (it != g_worldmap.getVehicles().end()) {
                            editorVehicles.setStamp(*it);
                            return EDITOR_VEHICLES;
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
                    }

                    if (event.key.keysym.sym == SDLK_PAGEDOWN) {
                        if (++game_values.worldindex >= worldlist->count())
                            game_values.worldindex = 0;

                        worldlist->next();

                        loadcurrentworld();
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
                    const Vec2s tilePos(iCol, iRow);

                    if (iButtonX >= 0 && iButtonY >= 0 && iButtonX < iWorldWidth * TILESIZE && iButtonY < iWorldHeight * TILESIZE) {
                        const bool isLeftPressed = event.motion.state & SDL_BUTTON_LMASK;
                        const bool isRightPressed = event.motion.state & SDL_BUTTON_RMASK;

                        if (currentEditor && (isLeftPressed || isRightPressed)) {
                            const uint8_t button = isLeftPressed ? SDL_BUTTON_LEFT : SDL_BUTTON_RIGHT;  // Prefer left press if both are down
                            const bool changed = currentEditor->onTileClicked(g_worldmap, tilePos, button);
                            if (changed)
                                updateworldsurface();
                        }
                    }

                    // Scan to see if we are mousing over a stage
                    if (edit_mode == 5) {
                        iStageDisplay = -1;

                        const auto it = std::find_if(
                            g_worldmap.getVehicles().cbegin(),
                            g_worldmap.getVehicles().cend(),
                            [tilePos](const WorldVehicle& vehicle) { return vehicle.getCurrentTile() == tilePos; });
                        if (it != g_worldmap.getVehicles().cend()) {
                            iStageDisplay = it->iActionId;
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
            rm->spr_dialog.draw(224, 176, 0, 0, 192, 128);
            rm->menu_font_large.drawCentered(320, 195, "Exit");
            rm->menu_font_large.drawCentered(320, 220, "Are You Sure?");
            rm->menu_font_large.drawCentered(282, 254, "Yes");
            rm->menu_font_large.drawCentered(356, 254, "No");

            rm->spr_dialog.draw(fSelectedYes ? 250 : 326, 250, 192, 0, 64, 32);
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
                    DisplayStageDetails(iStageDisplay, mouse_x, mouse_y, *rm);
                }
            }

            if (edit_mode == 5 || edit_mode == 8) {  // draw vehicles
                int color = SDL_MapRGB(blitdest->format, 0, 0, 128);
                for (const WorldVehicle& vehicle : g_worldmap.getVehicles()) {
                    short ix = (vehicle.currentTile.x - draw_offset_col) * TILESIZE + draw_offset_x;
                    short iy = (vehicle.currentTile.y - draw_offset_row) * TILESIZE + draw_offset_y;

                    SDL_Rect r = { ix, iy, 32, 32 };
                    SDL_FillRect(blitdest, &r, color);

                    rm->spr_worldvehicle[0].draw(ix, iy, vehicle.iDrawDirection << 5, vehicle.iDrawSprite << 5, 32, 32);

                    if (edit_mode == 5) {
                        rm->spr_worldforegroundspecial[0].draw(ix, iy, (vehicle.iActionId % 10) << 5, (vehicle.iActionId / 10) << 5, 32, 32);
                    }

                    if (edit_mode == 8) {
                        short iBoundary = vehicle.iBoundary - 1;
                        if (iBoundary == -1)
                            rm->spr_worldforegroundspecial[0].draw(ix, iy, 288, 288, 32, 32);
                        else
                            rm->spr_worldforegroundspecial[0].draw(ix, iy, (iBoundary % 10) << 5, (iBoundary / 10) << 5, 32, 32);
                    }
                }

                if (edit_mode == 5) {
                    if (iStageDisplay >= 0) {
                        DisplayStageDetails(iStageDisplay, mouse_x, mouse_y, *rm);
                    }
                }
            }

            rm->menu_font_small.draw(0, 0, szEditModes[edit_mode]);

            if (fAutoPaint)
                rm->menu_font_small.draw(0, 16, "Auto Paint");

            rm->menu_font_small.drawRightJustified(640, 0, worldlist->currentPath().c_str());


            if (--g_musiccategorydisplaytimer > 0) {
                rm->spr_dialog.draw(224, 176, 0, 0, 192, 128);
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

        rm->spr_dialog.draw(224, 176, 0, 0, 192, 128);
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


int enterEditor(EditorBase& editor)
{
    currentEditor = &editor;
    editor.onEnter(g_worldmap);

    bool done = false;
    while (!done) {
        int framestart = SDL_GetTicks();

        game_values.playerInput.ClearPressedKeys(1);

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                done = true;
                continue;
            }

            editor.handleSetupInput(event, g_worldmap);
            game_values.playerInput.Update(event, 1);
        }

        editor.updateMenu(g_worldmap);

        if (editor.isReady()) {
            game_values.playerInput.ResetKeys();
            return EDITOR_EDIT;
        }

        if (editor.isSetupTransparent()) {
            drawmap(false, TILESIZE);
            menu_shade.draw(0, 0);
        } else {
            SDL_FillRect(screen, NULL, 0x0);
        }

        editor.renderSetup(*rm, g_worldmap);

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
    rm->spr_dialog.draw(224, 176, 0, 0, 192, 128);
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
                        rm->spr_dialog.draw(224, 176, 0, 0, 192, 128);
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
                        rm->spr_dialog.draw(224, 176, 0, 0, 192, 128);
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
        for (const WorldVehicle& vehicle : g_worldmap.getVehicles()) {
            const short ix = vehicle.getCurrentTile().x * iTileSize;
            const short iy = vehicle.getCurrentTile().y * iTileSize;
            rm->spr_worldvehicle[iScreenshotSize].draw(ix, iy, vehicle.iDrawDirection * iTileSize, vehicle.iDrawSprite * iTileSize, iTileSize, iTileSize);
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
