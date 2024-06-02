#include "FileList.h"
#include "GameValues.h"
#include "gfx.h"
#include "map.h"
#include "MapList.h"
#include "path.h"
#include "ResourceManager.h"
#include "TilesetManager.h"

#include <cstdio>
#include <algorithm>
#include <array>

// main game directory, read from command line argument
#ifdef __ANDROID__
std::string RootDataDirectory = GetHomeDirectory() + "data";
#else
std::string RootDataDirectory = GetRootDirectory() + "data";
#endif

CResourceManager *rm;
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

//Joystick-Init
SDL_Joystick **joysticks = NULL;
short joystickcount = 0;


//Koopa Bros Weapons
//Mushrooms Only
//Super Mario Bros (All mushrooms ((poison was in lost levels, that counts as SMB1)), fireflower, star, and maybe the shells)
//Super Mario Bros 2 (All mushrooms but poison, star, clock, pow, green/red shell, bombs)
//One I'd like to see is SMWorld (esc{sp}) only stuff...
//0,10,0,1,0,10,6,0,0,0,2,0,8,4,2,4,0,10,0,5,2,0,0


//Conversion from backgrounds to music categories created by NMcCoy
const char * g_szMusicCategoryNames[MAXMUSICCATEGORY] = {"Land", "Underground", "Underwater", "Castle", "Platforms", "Ghost", "Bonus", "Battle", "Desert", "Clouds", "Snow"};


void LoadCurrentMapBackground()
{
    std::string path = concat("gfx/packs/backgrounds/", g_map->szBackgroundFile);
    path = convertPath(path, gamegraphicspacklist->currentPath());

    //if the background file doesn't exist, use the classic background
    if (!FileExists(path))
        path = convertPath("gfx/packs/backgrounds/Land_Classic.png", gamegraphicspacklist->currentPath());

    gfx_loadimagenocolorkey(&rm->spr_background, path);
}
