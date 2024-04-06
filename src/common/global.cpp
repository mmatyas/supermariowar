#include "FileList.h"
#include "GameValues.h"
#include "Game.h"
#include "gfx.h"
#include "map.h"
#include "MapList.h"
#include "MovingObjectTypes.h"
#include "ResourceManager.h"
#include "TilesetManager.h"

#include <cstdio>
#include <algorithm>
#include <array>

//1.8.0.0 == Release to staff
//1.8.0.1 == Second release to staff
//1.8.0.2 == beta1
//1.8.0.3 == beta2
//1.8.0.4 == final
//1.9.0.0 == neagix work-in-progress, not released
//2.0.0.0 == fluffypillow netplay code
int32_t g_iVersion[] = {2, 0, 0, 0};

// main game directory, read from command line argument
#ifdef __ANDROID__
std::string RootDataDirectory = GetHomeDirectory() + "data";
#else
std::string RootDataDirectory = GetRootDirectory() + "data";
#endif

CGame *smw;
CResourceManager *rm;

bool VersionIsEqual(int32_t iVersion[], short iMajor, short iMinor, short iMicro, short iBuild)
{
    return iVersion[0] == iMajor && iVersion[1] == iMinor && iVersion[2] == iMicro && iVersion[3] == iBuild;
}

bool VersionIsEqualOrBefore(int32_t iVersion[], short iMajor, short iMinor, short iMicro, short iBuild)
{
    if (iVersion[0] < iMajor)
        return true;

    if (iVersion[0] == iMajor) {
        if (iVersion[1] < iMinor)
            return true;

        if (iVersion[1] == iMinor) {
            if (iVersion[2] < iMicro)
                return true;

            if (iVersion[2] == iMicro) {
                return iVersion[3] <= iBuild;
            }
        }
    }

    return false;
}

bool VersionIsEqualOrAfter(int32_t iVersion[], short iMajor, short iMinor, short iMicro, short iBuild)
{
    if (iVersion[0] > iMajor)
        return true;

    if (iVersion[0] == iMajor) {
        if (iVersion[1] > iMinor)
            return true;

        if (iVersion[1] == iMinor) {
            if (iVersion[2] > iMicro)
                return true;

            if (iVersion[2] == iMicro) {
                return iVersion[3] >= iBuild;
            }
        }
    }

    return false;
}

CGameValues game_values;

FiltersList *filterslist;  //Filters list must be initiallized before maps list because it is used in maplist constructor
MapList *maplist;
SkinList *skinlist;
AnnouncerList *announcerlist;
MusicList *musiclist;
WorldMusicList *worldmusiclist;
GraphicsList *menugraphicspacklist;
GraphicsList *worldgraphicspacklist;
GraphicsList *gamegraphicspacklist;
SoundsList *soundpacklist;
TourList *tourlist;
WorldList *worldlist;

CMap      *g_map;
CTilesetManager *g_tilesetmanager;


bool g_fLoadMessages = true;

//Network stuff
int g_iNextNetworkID = 0;
int g_iNextMessageID = 0;
char szIPString[32] = "";

//Joystick-Init
SDL_Joystick **joysticks = NULL;
short joystickcount = 0;

/*
0 == poison mushroom
1 == 1up
2 == 2up
3 == 3up
4 == 5up
5 == flower
6 == star
7 == clock
8 == bobomb
9 == pow
10 == bulletbill
11 == hammer
12 == green shell
13 == red shell
14 == spike shell
15 == buzzy shell
16 == mod
17 == feather
18 == mystery mushroom
19 == boomerang
20 == tanooki
21 == ice wand
22 == podoboo
23 == bombs
24 == leaf
25 == pwings
*/


short g_iDefaultPowerupPresets[NUM_POWERUP_PRESETS][NUM_POWERUPS] = {
//   0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25
    {5,10, 4, 2, 1,10, 8, 4, 4, 2, 2, 4, 8, 4, 2, 4, 2, 4, 5, 6, 6, 3, 4, 4, 5, 3}, //Custom 1
    {5,10, 4, 2, 1,10, 8, 4, 4, 2, 2, 4, 8, 4, 2, 4, 2, 4, 5, 6, 6, 3, 4, 4, 5, 3}, //Custom 2
    {5,10, 4, 2, 1,10, 8, 4, 4, 2, 2, 4, 8, 4, 2, 4, 2, 4, 5, 6, 6, 3, 4, 4, 5, 3}, //Custom 3
    {5,10, 4, 2, 1,10, 8, 4, 4, 2, 2, 4, 8, 4, 2, 4, 2, 4, 5, 6, 6, 3, 4, 4, 5, 3}, //Custom 4
    {5,10, 4, 2, 1,10, 8, 4, 4, 2, 2, 4, 8, 4, 2, 4, 2, 4, 5, 6, 6, 3, 4, 4, 5, 3}, //Custom 5
    {5,10, 5, 3, 1,10, 2, 3, 4, 3, 3, 4, 9, 6, 2, 4, 4, 7, 5, 6, 6, 3, 2, 2, 5, 5}, //Balanced
    {5, 0, 0, 0, 0,10, 0, 0, 0, 0, 0, 7, 9, 6, 3, 4, 0, 0, 0, 4, 0, 2, 0, 2, 0, 0}, //Weapons Only
    {0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0}, //Koopa Bros Weapons
    {5,10, 7, 5, 2, 0, 6, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 5, 0, 3, 0, 0, 0, 8, 6}, //Support Items
    {3, 3, 1, 0, 0, 0, 0, 0, 4, 2, 2, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 3, 3, 0, 0}, //Booms and Shakes
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 2, 0, 0, 0, 8, 3}, //Fly and Glide
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 4, 2, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //Shells
    {5, 8, 4, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0}, //Mushrooms Only
    {3, 5, 0, 0, 0, 5, 2, 0, 0, 0, 3, 0, 6, 4, 1, 3, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0}, //Super Mario Bros 1
    {0, 5, 0, 0, 0, 0, 2, 4, 3, 2, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0}, //Super Mario Bros 2
    {0, 3, 0, 0, 0, 8, 4, 0, 0, 0, 5, 2,10, 9, 4, 5, 0, 0, 0, 4, 3, 0, 4, 0, 8, 5}, //Super Mario Bros 3
    {0,10, 0, 0, 0,10, 6, 0, 0, 0, 2, 0, 8, 4, 2, 4, 0, 4, 0, 0, 0, 0, 5, 0, 0, 0}, //Super Mario World
};

//Koopa Bros Weapons
//Mushrooms Only
//Super Mario Bros (All mushrooms ((poison was in lost levels, that counts as SMB1)), fireflower, star, and maybe the shells)
//Super Mario Bros 2 (All mushrooms but poison, star, clock, pow, green/red shell, bombs)
//One I'd like to see is SMWorld (esc{sp}) only stuff...
//0,10,0,1,0,10,6,0,0,0,2,0,8,4,2,4,0,10,0,5,2,0,0

short g_iCurrentPowerupPresets[NUM_POWERUP_PRESETS][NUM_POWERUPS];



void _load_drawmsg(const std::string& f)
{
    if (g_fLoadMessages) {
        /*
        static SDL_Rect r;
        r.x = 0;
        r.y = 0;
        r.w = 500;
        r.h = (Uint16)menu_font_small.getHeight();
        Uint32 col = SDL_MapRGB(screen->format, 189, 251, 255);
        SDL_FillRect(screen, &r, col);      //fill empty area
        */

        rm->menu_font_small.draw(0, 0, f.c_str());
    }
}

void _load_waitforkey()
{
#ifndef __EMSCRIPTEN__
    SDL_Event event;
    while (true) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYDOWN)
                return;
            if (event.type == SDL_JOYBUTTONDOWN)
                return;
        }

        SDL_Delay(10);
    }
#endif
}



short iScoreboardPlayerOffsetsX[3][3] = {{40, 0, 0}, {19, 59, 0}, {6, 40, 74}};
short iKingOfTheHillZoneLimits[4][4] = {{0, 0, 1, 2}, {0, 1, 2, 4}, {0, 2, 4, 7}, {0, 2, 5, 12}};

//Conversion from backgrounds to music categories created by NMcCoy
short g_iMusicCategoryConversion[26] = {0, 3, 8, 5, 1, 9, 3, 4, 10, 8, 1, 0, 9, 0, 0, 7, 4, 1, 1, 6, 4, 7, 6, 3, 0, 4};
const char * g_szMusicCategoryNames[MAXMUSICCATEGORY] = {"Land", "Underground", "Underwater", "Castle", "Platforms", "Ghost", "Bonus", "Battle", "Desert", "Clouds", "Snow"};
short g_iDefaultMusicCategory[MAXMUSICCATEGORY] = {0, 0, 0, 0, 0, 1, 2, 3, 0, 0, 0};

const char * g_szWorldMusicCategoryNames[MAXWORLDMUSICCATEGORY] = {"Grass", "Desert", "Water", "Giant", "Sky", "Ice", "Pipe", "Dark", "Space"};

const char * g_szAutoFilterNames[NUM_AUTO_FILTERS] = {"Death Tiles", "Warps", "Ice", "Item Boxes", "Breakable Blocks", "Throwable Blocks", "On/Off Blocks", "Platforms", "Hazards", "Item Destroyable Blocks", "Hidden Blocks", "Map Items"};
short g_iAutoFilterIcons[NUM_AUTO_FILTERS] = {37, 29, 33, 1, 0, 6, 40, 73, 19, 87, 17, 118};


void LoadCurrentMapBackground()
{
    std::string path = concat("gfx/packs/backgrounds/", g_map->szBackgroundFile);
    path = convertPath(path, gamegraphicspacklist->current_name());

    //if the background file doesn't exist, use the classic background
    if (!File_Exists(path))
        path = convertPath("gfx/packs/backgrounds/Land_Classic.png", gamegraphicspacklist->current_name());

    gfx_loadimagenocolorkey(&rm->spr_background, path);
}


const std::array<TileType, 19> ordered_tile_types {
    tile_nonsolid,
    tile_solid,
    tile_solid_on_top,
    tile_ice,
    tile_death,
    tile_death_on_top,
    tile_death_on_bottom,
    tile_death_on_left,
    tile_death_on_right,
    tile_ice_on_top,
    tile_ice_death_on_bottom,
    tile_ice_death_on_left,
    tile_ice_death_on_right,
    tile_super_death,
    tile_super_death_top,
    tile_super_death_bottom,
    tile_super_death_left,
    tile_super_death_right,
    tile_player_death,
};

TileType NextTileType(TileType type)
{
    auto it = std::find(ordered_tile_types.cbegin(), ordered_tile_types.cend(), type);
    if (it == ordered_tile_types.cend())
        return tile_nonsolid;

    ++it;
    return it == ordered_tile_types.cend() ? ordered_tile_types.front() : *it;
}

TileType PrevTileType(TileType type)
{
    auto it = std::find(ordered_tile_types.cbegin(), ordered_tile_types.cend(), type);
    if (it == ordered_tile_types.cend())
        return tile_nonsolid;

    return it == ordered_tile_types.cbegin() ? ordered_tile_types.back() : *(--it);
}
