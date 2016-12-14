/* This file is part of SMW.

    SMW is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    SMW is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SMW.  If not, see <http://www.gnu.org/licenses/>. */


#define TITLESTRING "Super Mario War"
#define VERSIONNUMBER "2.0"

#include "FileList.h"
#include "GameMode.h"
#include "gamemodes.h"
#include "GameValues.h"
#include "map.h"
#include "MapList.h"
#include "net.h"
#include "linfunc.h"
#include "player.h"
#include "ResourceManager.h"
#include "sfx.h"
#include "TilesetManager.h"

#include "FPSLimiter.h"
#include "GSSplashScreen.h"

#include <ctime>
#include <cmath>
#include <cstdlib> // srand()

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#ifdef _XBOX
#include <xtl.h>
#endif

#ifdef _MSC_VER
#if _MSC_VER >= 1400
    #include <stdio.h>
    FILE _iob[] = {*stdin, *stdout, *stderr};
    extern "C" FILE * __cdecl __iob_func(void) { return _iob; }
#endif
#endif


//now it's really time for an "engine" (aka resource manager)


//------ system stuff ------
SDL_Surface		*screen = NULL;		//for gfx (maybe the gfx system should be improved -> resource manager)
SDL_Surface		*blitdest = NULL;	//the destination surface for all drawing (can be swapped from screen to another surface)

short			x_shake = 0;
short			y_shake = 0;


//------ game relevant stuff ------
CPlayer			*list_players[4];
short			list_players_cnt = 0;

CScore			*score[4];

short			score_cnt;


//Locations for swirl spawn effects
short g_iSwirlSpawnLocations[4][2][25];


CGameMode			*gamemodes[GAMEMODE_LAST];
CGM_Bonus			*bonushousemode = NULL;
CGM_Pipe_MiniGame	*pipegamemode = NULL;
CGM_Boss_MiniGame	*bossgamemode = NULL;
CGM_Boxes_MiniGame	*boxesgamemode = NULL;

short currentgamemode = 0;

//Adds music overrides to the music lists
extern void UpdateMusicWithOverrides();

extern SDL_Joystick     **joysticks;
extern short            joystickcount;

extern short g_iDefaultPowerupPresets[NUM_POWERUP_PRESETS][NUM_POWERUPS];
extern short g_iCurrentPowerupPresets[NUM_POWERUP_PRESETS][NUM_POWERUPS];

extern CMap* g_map;
extern CTilesetManager* g_tilesetmanager;

extern FiltersList* filterslist;
extern MapList* maplist;
extern SkinList* skinlist;
extern AnnouncerList* announcerlist;
extern MusicList* musiclist;
extern WorldMusicList* worldmusiclist;
extern GraphicsList* menugraphicspacklist;
extern GraphicsList* worldgraphicspacklist;
extern GraphicsList* gamegraphicspacklist;
extern SoundsList* soundpacklist;
extern TourList* tourlist;
extern WorldList* worldlist;

extern std::string RootDataDirectory;
extern CGameValues game_values;
extern CResourceManager* rm;

//*************************************
//  MAIN LOOP
//*************************************


#ifdef __EMSCRIPTEN__
void gameloop_frame();
#endif

void gameloop()
{
    SplashScreenState::instance().init();
    GameStateManager::instance().currentState = &SplashScreenState::instance();

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(gameloop_frame, 0, 1);
}

void gameloop_frame()
#else
    while (game_values.gamestate != GS_QUIT)
#endif
    {
        FPSLimiter::instance().frameStart();

        GameStateManager::instance().currentState->update();

        FPSLimiter::instance().beforeFlip();
        gfx_flipscreen();
        FPSLimiter::instance().afterFlip();
    }

#ifndef __EMSCRIPTEN__
}
#endif

void create_globals()
{
    // this instance will contain the other relevant objects
    smw = new CGame(RootDataDirectory.c_str());
    rm = new CResourceManager();
#pragma warning ("delete these or use boost GC shared_ptr")

    g_map = new CMap();
    g_tilesetmanager = new CTilesetManager();

    filterslist = new FiltersList();
    maplist = new MapList(false);

    //TODO: add proper test via size
    if (maplist->IsEmpty()) {
        throw "Empty map directory!";
    }

    skinlist = new SkinList();
    musiclist = new MusicList();
    worldmusiclist = new WorldMusicList();
    soundpacklist = new SoundsList();
    announcerlist = new AnnouncerList();
    tourlist = new TourList();
    worldlist = new WorldList();

    menugraphicspacklist = new GraphicsList();
    worldgraphicspacklist = new GraphicsList();
    gamegraphicspacklist = new GraphicsList();

    announcerlist->SetCurrent(0);
    musiclist->SetCurrent(0);
    worldmusiclist->SetCurrent(0);
    menugraphicspacklist->SetCurrent(0);
    worldgraphicspacklist->SetCurrent(0);
    gamegraphicspacklist->SetCurrent(0);
    soundpacklist->SetCurrent(0);
}

void init_joysticks()
{
    SDL_InitSubSystem(SDL_INIT_JOYSTICK);
    joystickcount = (short)SDL_NumJoysticks();
    joysticks = new SDL_Joystick*[joystickcount];

    for (short i = 0; i < joystickcount; i++)
        joysticks[i] = SDL_JoystickOpen(i);

    SDL_JoystickEventState(SDL_ENABLE);
}

void create_gamemodes()
{
    //set game modes
    gamemodes[0] = new CGM_Classic();
    gamemodes[1] = new CGM_Frag();
    gamemodes[2] = new CGM_TimeLimit();
    gamemodes[3] = new CGM_Jail();
    gamemodes[4] = new CGM_Coins();
    gamemodes[5] = new CGM_Stomp();
    gamemodes[6] = new CGM_Eggs();
    gamemodes[7] = new CGM_CaptureTheFlag();
    gamemodes[8] = new CGM_Chicken();
    gamemodes[9] = new CGM_Tag();
    gamemodes[10] = new CGM_Star();
    gamemodes[11] = new CGM_Domination();
    gamemodes[12] = new CGM_KingOfTheHill();
    gamemodes[13] = new CGM_Race();
    gamemodes[14] = new CGM_Owned();
    gamemodes[15] = new CGM_Frenzy();
    gamemodes[16] = new CGM_Survival();
    gamemodes[17] = new CGM_Greed();
    gamemodes[18] = new CGM_Health();
    gamemodes[19] = new CGM_Collection();
    gamemodes[20] = new CGM_Chase();
    gamemodes[21] = new CGM_ShyGuyTag();

    currentgamemode = 0;
    game_values.gamemode = gamemodes[currentgamemode];

    //Special modes
    bonushousemode = new CGM_Bonus();
    pipegamemode = new CGM_Pipe_MiniGame();
    bossgamemode = new CGM_Boss_MiniGame();
    boxesgamemode = new CGM_Boxes_MiniGame();
}

void init_spawnlocations()
{
        //Calculate the swirl spawn effect locations
    float spawnradius = 100.0f;
    float spawnangle = 0.0f;

    for (short i = 0; i < 25; i++) {
        g_iSwirlSpawnLocations[0][0][i] = (short)(spawnradius * cos(spawnangle));
        g_iSwirlSpawnLocations[0][1][i] = (short)(spawnradius * sin(spawnangle));

        float angle = spawnangle + HALF_PI;
        g_iSwirlSpawnLocations[1][0][i] = (short)(spawnradius * cos(angle));
        g_iSwirlSpawnLocations[1][1][i] = (short)(spawnradius * sin(angle));

        angle = spawnangle + PI;
        g_iSwirlSpawnLocations[2][0][i] = (short)(spawnradius * cos(angle));
        g_iSwirlSpawnLocations[2][1][i] = (short)(spawnradius * sin(angle));

        angle = spawnangle + THREE_HALF_PI;
        g_iSwirlSpawnLocations[3][0][i] = (short)(spawnradius * cos(angle));
        g_iSwirlSpawnLocations[3][1][i] = (short)(spawnradius * sin(angle));

        spawnradius -= 4.0f;
        spawnangle += 0.1f;
    }
}


//*************************************
//  PROGRAM ENTRY POINT
//*************************************


// ------ MAIN ------
int main(int argc, char *argv[])
{
    if (argc >= 2)
        RootDataDirectory = argv[1];


    create_globals();

    printf("-------------------------------------------------------------------------------\n");
    printf(" %s %s\n", TITLESTRING, VERSIONNUMBER);
    printf("-------------------------------------------------------------------------------\n");
    printf("\n---------------- startup ----------------\n");

	gfx_init(smw->ScreenWidth, smw->ScreenHeight, false);		//initialize the graphics (SDL)
    blitdest = screen;

#if	0
    //Comment this in to performance test the preview map loading
    MI_MapField * miMapField = new MI_MapField(&rm->spr_selectfield, 70, 165, "Map", 500, 120);

    for (int k = 0; k < 100; k++) {
        game_values.playerInput.outputControls[3].menu_right.fPressed = true;
        miMapField->SendInput(&game_values.playerInput);
        //printf("Map over-> %s\n", maplist->currentFilename());
    }

    return 0;
#endif

    sfx_init();                     //init the sound system
    net_init();                     //init the networking

    init_joysticks();

    //currently this only sets the title, not the icon.
    //setting the icon isn't implemented in sdl ->  i'll ask on the mailing list
    char title[128];
    sprintf(title, "%s %s", TITLESTRING, VERSIONNUMBER);
    gfx_settitle(title);

    printf("\n---------------- loading ----------------\n");

    for (short iScore = 0; iScore < 4; iScore++)
        score[iScore] = new CScore(iScore);

    game_values.init();

    //Set the default powerup weights for bonus wheel and [?] boxes
    for (short iPreset = 0; iPreset < NUM_POWERUP_PRESETS; iPreset++) {
        for (short iPowerup = 0; iPowerup < NUM_POWERUPS; iPowerup++) {
            g_iCurrentPowerupPresets[iPreset][iPowerup] = g_iDefaultPowerupPresets[iPreset][iPowerup];
        }
    }

    UpdateMusicWithOverrides();

    create_gamemodes();

    game_values.ReadBinaryConfig();

    //Assign the powerup weights to the selected preset
    for (short iPowerup = 0; iPowerup < NUM_POWERUPS; iPowerup++) {
        game_values.powerupweights[iPowerup] = g_iCurrentPowerupPresets[game_values.poweruppreset][iPowerup];
    }

#ifdef _XBOX
    gfx_changefullscreen(false); //Sets flicker filter
    SDL_SetHardwareFilter(game_values.hardwarefilter);
    blitdest = screen;
#else
    if (game_values.fullscreen) {
        gfx_changefullscreen(true);
        blitdest = screen;
    }
#endif

    init_spawnlocations();

    //Load the gfx color palette
    gfx_loadpalette(convertPathCP("gfx/packs/palette.bmp", gamegraphicspacklist->current_name()));

    //Call to setup input optimization
    game_values.playerInput.CheckIfMouseUsed();

    srand((unsigned int)time(NULL));
/*
    bool fLoadOK = LoadAndSplashScreenState();

    if (!fLoadOK) {
        printf("\n---------------- EXIT DURING LOADING ----------------\n\n");
        sfx_close();
        gfx_close();
        net_close();
        return 0;
    }
*/


    //**********************************************************


    gameloop(); // all the game logic happens here


    //**********************************************************


    printf("\n---------------- shutdown ----------------\n");

    for (short i = 0; i < GAMEMODE_LAST; i++)
        delete gamemodes[i];

#ifdef _XBOX
    for (i = 0; i < joystickcount; i++)
        SDL_JoystickClose(joysticks[i]);

    delete[] joysticks;
#endif

    sfx_close();
    gfx_close();
    net_close();

    //Delete player skins
    for (short k = 0; k < MAX_PLAYERS; k++) {
        for (short j = 0; j < PGFX_LAST; j++) {
            delete rm->spr_player[k][j];
            delete rm->spr_shyguy[k][j];
            delete rm->spr_chocobo[k][j];
            delete rm->spr_bobomb[k][j];
        }

        delete [] rm->spr_player[k];
        delete [] rm->spr_shyguy[k];
        delete [] rm->spr_chocobo[k];
        delete [] rm->spr_bobomb[k];

        delete score[k];
    }

    delete [] game_values.pfFilters;
    delete [] game_values.piFilterIcons;

//Return to dash on xbox
#ifdef _XBOX
    LD_LAUNCH_DASHBOARD LaunchData = { XLD_LAUNCH_DASHBOARD_MAIN_MENU };
    XLaunchNewImage( NULL, (LAUNCH_DATA*)&LaunchData );
#endif

	// release all resources
	delete rm;
    delete smw;

    return 0;
}


/*
#ifdef _XBOX

void reconnectjoysticks()
{
    for (int i = 0; i < joystickcount; i++)
        SDL_JoystickClose(joysticks[i]);

    delete[] joysticks;

    SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
    SDL_InitSubSystem(SDL_INIT_JOYSTICK);

    joystickcount = SDL_NumJoysticks();
    joysticks = new SDL_Joystick*[joystickcount];

    for (i = 0; i < joystickcount; i++)
        joysticks[i] = SDL_JoystickOpen(i);

    SDL_JoystickEventState(SDL_ENABLE);
}

#endif
*/
