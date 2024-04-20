#include "FileList.h"
#include "GameValues.h"
#include "Game.h"
#include "gfx.h"
#include "map.h"
#include "MapList.h"
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
