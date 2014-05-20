/* This file is part of SMW.

    SMW is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    Foobar is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SMW.  If not, see <http://www.gnu.org/licenses/>. */

#ifdef _XBOX
#include <xtl.h>
#endif

#include "global.h"				//all the global stuff

#include "Game.h"
#include "menu.h"   // included for match types

#include <time.h>
#include <math.h>

//#include "FPSLimiter.h"

//now it's really time for an "engine" (aka resource manager)
// TODO: check SDL2 Windows libs
#ifdef _WIN32
#pragma comment(lib, "SDL_image.lib")

#ifndef _XBOX
#pragma comment(lib, "SDL.lib")
#pragma comment(lib, "SDLmain.lib")
#pragma comment(lib, "SDL_mixer.lib")

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#ifdef _MSC_VER
#include <crtdbg.h>
#endif

#endif
#endif


//------ system stuff ------
#ifdef USE_SDL2
    SDL_Window       *window;        //the window
    SDL_Renderer     *renderer;      //screen -> texture -> renderer -> window
    SDL_Texture      *screenAsTexture;
#endif
SDL_Surface		*screen;		//for gfx (maybe the gfx system should be improved -> resource manager)
SDL_Surface		*blitdest;		//the destination surface for all drawing (can be swapped from screen to another surface)

short			g_iCurrentDrawIndex = 0;

short			x_shake = 0;
short			y_shake = 0;


//------ game relevant stuff ------
CPlayer			*list_players[4];
short			list_players_cnt = 0;

CScore			*score[4];

short			score_cnt;




extern SDL_KEYTYPE controlkeys[2][2][4][NUM_KEYS];
extern int g_iVersion[];

//Locations for swirl spawn effects
short g_iSwirlSpawnLocations[4][2][25];

CMap			*g_map;
CTilesetManager *g_tilesetmanager;

CObjectContainer noncolcontainer;
CObjectContainer objectcontainer[3];

bool g_fLoadMessages = true;

STextAward awards[PAWARD_LAST] = {
    STextAward("Double Kill", &rm->game_font_small),
    STextAward("Triple Kill", &rm->game_font_small),
    STextAward("Killing Spree",  &rm->game_font_small),
    STextAward("Killing Spree x 2", &rm->game_font_small),
    STextAward("Killing Spree x 3", &rm->game_font_small),
    STextAward("Dominating", &rm->game_font_large),
    STextAward("Dominating x 2", &rm->game_font_large),
    STextAward("Dominating x 3", &rm->game_font_large),
    STextAward("Unstoppable!", &rm->game_font_large)
};

sfxSound sfx_announcer[PANNOUNCER_SOUND_LAST];

bool  fResumeMusic = true;

sfxSound sfx_mip;
sfxSound sfx_deathsound;
sfxSound sfx_jump;
sfxSound sfx_skid;
sfxSound sfx_capejump;

sfxSound sfx_invinciblemusic;
sfxSound sfx_extraguysound;
sfxSound sfx_sprout;
sfxSound sfx_collectpowerup;
sfxSound sfx_collectfeather;
sfxSound sfx_storepowerup;
sfxSound sfx_tailspin;
sfxSound sfx_breakblock;
sfxSound sfx_bump;
sfxSound sfx_coin;
sfxSound sfx_fireball;
sfxSound sfx_springjump;
sfxSound sfx_timewarning;
sfxSound sfx_hit;
sfxSound sfx_chicken;
sfxSound sfx_transform;
sfxSound sfx_yoshi;
sfxSound sfx_pause;
sfxSound sfx_bobombsound;
sfxSound sfx_areatag;
sfxSound sfx_cannon;
sfxSound sfx_burnup;
sfxSound sfx_pipe;
sfxSound sfx_thunder;
sfxSound sfx_slowdownmusic;
sfxSound sfx_flyingsound;
sfxSound sfx_storedpowerupsound;
sfxSound sfx_kicksound;
sfxSound sfx_racesound;
sfxSound sfx_bulletbillsound;
sfxSound sfx_boomerang;
sfxSound sfx_spit;
sfxSound sfx_starwarning;
sfxSound sfx_powerdown;
sfxSound sfx_switchpress;
sfxSound sfx_superspring;
sfxSound sfx_stun;
sfxSound sfx_inventory;
sfxSound sfx_worldmove;
sfxSound sfx_treasurechest;
sfxSound sfx_flamecannon;
sfxSound sfx_wand;
sfxSound sfx_enterstage;
sfxSound sfx_gameover;
sfxSound sfx_pickup;

sfxMusic backgroundmusic[6];

CGameMode			*gamemodes[GAMEMODE_LAST];
CGM_Bonus			*bonushousemode = NULL;
CGM_Pipe_MiniGame	*pipegamemode = NULL;
CGM_Boss_MiniGame	*bossgamemode = NULL;
CGM_Boxes_MiniGame	*boxesgamemode = NULL;

short		currentgamemode = 0;

extern void LoadCurrentMapBackground();

extern bool VersionIsEqual(int iVersion[], short iMajor, short iMinor, short iMicro, short iBuild);

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
WorldMap g_worldmap;

std::vector<MapMusicOverride*> mapmusicoverrides;
std::vector<WorldMusicOverride*> worldmusicoverrides;

//Network stuff
int g_iNextNetworkID = 0;
int g_iNextMessageID = 0;
char szIPString[32] = "";

//NetServer netServer;
//NetClient netClient;


extern void SetupDefaultGameModeSettings();

float CapFallingVelocity(float vel)
{
    //if(vel < -MAXVELY)
    //	return -MAXVELY;

    if(vel > MAXVELY)
        return MAXVELY;

    return vel;
}

float CapSideVelocity(float vel)
{
    if(vel < -MAXSIDEVELY)
        return -MAXSIDEVELY;

    if(vel > MAXSIDEVELY)
        return MAXSIDEVELY;

    return vel;
}

void LoadAll();
void SetGameModeSettingsFromMenu();
void PlayNextMusicTrack();

//Adds music overrides to the music lists
void UpdateMusicWithOverrides();

//Joystick-Init
SDL_Joystick **joysticks = NULL;
short joystickcount = 0;

#ifdef _DEBUG
extern void HashTableTest();
#endif

/*
void EnterBossMode(short type)
{
    if(game_values.gamestate == GS_GAME && game_values.gamemode->gamemode != game_mode_boss_minigame)
    {
        bossgamemode->SetBossType(type);

        game_values.screenfade = 2;
        game_values.screenfadespeed = 2;

        backgroundmusic[0].stop();
        ifsoundonstop(sfx_invinciblemusic);
        ifsoundonstop(sfx_timewarning);
        ifsoundonstop(sfx_slowdownmusic);

        game_values.gamestate = GS_START_GAME;
    }
}*/

gv game_values;


#ifdef	WIN32
int CALLBACK WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
#ifdef _MSC_VER
    // this will print important debugging information to debug console
    _CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_DEBUG );
#endif

    RootDataDirectory = lpCmdLine;

    if (strlen(lpCmdLine) > 0)
        RootDataDirectory = lpCmdLine;
#else
// ------ MAIN ------
int main(int argc, char *argv[])
{
    if (argc >= 2)
        RootDataDirectory = argv[1];
#endif


    // this instance will contain the other relevant objects
    smw = new CGame(RootDataDirectory);
	rm = new CResourceManager();
#pragma warning ("delete these or use boost GC shared_ptr")

    g_map = new CMap();
    g_tilesetmanager = new CTilesetManager();

    filterslist = new FiltersList();
    maplist = new MapList(false);

    //TODO: add proper test via size
    if(maplist->IsEmpty()) {
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

    printf("-------------------------------------------------------------------------------\n");
    printf(" %s %s\n", TITLESTRING, VERSIONNUMBER);
    printf("-------------------------------------------------------------------------------\n");
    printf("\n---------------- startup ----------------\n");

	gfx_init(smw->ScreenWidth, smw->ScreenHeight, false);		//initialize the graphics (SDL)
    blitdest = screen;

#if	0
    //Comment this in to performance test the preview map loading
    MI_MapField * miMapField = new MI_MapField(&rm->spr_selectfield, 70, 165, "Map", 500, 120);

    for(int k = 0; k < 100; k++) {
        game_values.playerInput.outputControls[3].menu_right.fPressed = true;
        miMapField->SendInput(&game_values.playerInput);
        //printf("Map over-> %s\n", maplist->currentFilename());
    }

    return 0;
#endif

    sfx_init();                     //init the sound system
    net_init();                     //init the networking


    //Joystick-Init
    SDL_InitSubSystem(SDL_INIT_JOYSTICK);
    joystickcount = (short)SDL_NumJoysticks();
    joysticks = new SDL_Joystick*[joystickcount];

    for(short i = 0; i < joystickcount; i++)
        joysticks[i] = SDL_JoystickOpen(i);

    SDL_JoystickEventState(SDL_ENABLE);

    //currently this only sets the title, not the icon.
    //setting the icon isn't implemented in sdl ->  i'll ask on the mailing list
    char title[128];
    sprintf(title, "%s %s", TITLESTRING, VERSIONNUMBER);
#ifdef USE_SDL2
    SDL_SetWindowTitle(window, title);
    SDL_SetRelativeMouseMode(SDL_TRUE);
#else
    SDL_WM_SetCaption(title, "smw.ico");
    SDL_ShowCursor(SDL_DISABLE);
#endif

    printf("\n---------------- loading ----------------\n");

    for(short iScore = 0; iScore < 4; iScore++)
        score[iScore] = new CScore(iScore);

    //set standard game values
    game_values.playercontrol[0]	= 1;
    game_values.playercontrol[1]	= 1;
    game_values.showfps				= false;
    game_values.frameadvance		= false;
    game_values.autokill			= false;
    game_values.framelimiter		= WAITTIME;
    game_values.sound				= true;
    game_values.music				= true;
    game_values.gamestate			= GS_SPLASH;
#ifdef _DEBUG
    game_values.fullscreen			= false;
#else
    game_values.fullscreen			= false;
#endif

    game_values.screenResizeX		= 20.0f;
    game_values.screenResizeY		= 20.0f;
    game_values.screenResizeW		= -40.0f;
    game_values.screenResizeH		= -40.0f;

    game_values.flickerfilter		= 5;  //Full flicker filter by default
    game_values.hardwarefilter		= 2;  //Bilinear by default
    game_values.softfilter			= 0;  //No soft filter by default
    game_values.aspectratio10x11	= false;  //No 10x11 aspect ratio by default

#ifdef _XBOX
    SDL_XBOX_SetScreenPosition(game_values.screenResizeX, game_values.screenResizeY);
    SDL_XBOX_SetScreenStretch(game_values.screenResizeW, game_values.screenResizeH);
#endif

    game_values.pausegame			= false;
    game_values.exitinggame			= false;
    game_values.exityes				= false;
    game_values.awardstyle			= award_style_fireworks;
    game_values.spawnstyle			= 2;
    game_values.tournamentgames		= 2;
    game_values.tournamentwinner	= -1;
    game_values.selectedminigame	= 0;
    game_values.matchtype			= MATCH_TYPE_SINGLE_GAME;
    game_values.tourindex			= 0;
    game_values.tourstopcurrent		= 0;
    game_values.tourstoptotal		= 0;
    game_values.worldindex			= 0;
    game_values.slowdownon			= -1;
    game_values.slowdowncounter		= 0;
    game_values.teamcollision		= 0;
    game_values.screencrunch		= true;
    game_values.screenshaketimer	= 0;
    game_values.screenshakeplayerid = -1;
    game_values.screenshaketeamid	= -1;
    game_values.toplayer			= true;
    game_values.loadedannouncer		= -1;
    game_values.loadedmusic			= -1;
    game_values.scoreboardstyle     = 0;
    game_values.teamcolors          = true;
    game_values.cputurn				= -1;
    game_values.shieldtime			= 62;
    game_values.shieldstyle			= 2;
    game_values.musicvolume			= 128;
    game_values.soundvolume			= 128;
    game_values.respawn				= 2;
    game_values.itemrespawntime		= 1860;  //default item respawn is 30 seconds (30 * 62 fps)
    game_values.hiddenblockrespawn	= 1860;  //default item respawn is 30 seconds
    game_values.outofboundstime		= 5;
    game_values.warplockstyle		= 1;	// Lock Warp Exit Only
    game_values.warplocktime		= 186;  // 3 seconds
#ifdef _DEBUG
    game_values.suicidetime			= 0;	// Turn off suicide kills for debug
#else
    game_values.suicidetime			= 310;	// 5 seconds
#endif
    game_values.cpudifficulty		= 2;
    game_values.fireballttl			= 310;  // 5 seconds
    game_values.shellttl			= 496;  // 8 seconds
    game_values.blueblockttl		= 310;  // 5 seconds
    game_values.redblockttl			= 310;  // 5 seconds
    game_values.grayblockttl		= 310;  // 5 seconds
    game_values.hammerdelay			= 25;	// 0.4 second
    game_values.hammerttl			= 49;	// 0.8 second
    game_values.hammerpower			= true; //hammers die on first hit
    game_values.fireballlimit		= 0;	//Unlimited
    game_values.hammerlimit			= 0;	//Unlimited
    game_values.boomerangstyle		= 1;	//SMB3 style
    game_values.boomeranglife		= 248;	// 4 seconds of zelda boomerang
    game_values.boomeranglimit		= 0;	//Unlimited
    game_values.featherjumps		= 1;	//Allow one extra cape jump
    game_values.featherlimit		= 0;	//Unlimited
    game_values.leaflimit			= 0;	//Unlimited
    game_values.pwingslimit			= 0;	//Unlimited
    game_values.tanookilimit		= 0;	//Unlimited
    game_values.bombslimit			= 0;	//Unlimited
    game_values.wandfreezetime		= 310;  //5 seconds of freeze time
    game_values.wandlimit			= 0;	//Unlimited
    game_values.storedpowerupdelay	= 4;
    game_values.bonuswheel			= 1;
    game_values.keeppowerup			= false;
    game_values.showwinningcrown	= false;
    game_values.startgamecountdown	= true;
    game_values.startmodedisplay	= true;
    game_values.deadteamnotice		= true;
    game_values.playnextmusic		= false;
    game_values.pointspeed			= 20;
    game_values.swapstyle			= 1;	//Blink then swap
    game_values.worldpointsbonus	= -1; //no world multiplier until player uses item to boost it
    game_values.singleplayermode	= -1;
    game_values.worldskipscoreboard = false;
    game_values.overridepowerupsettings = 0;
    game_values.minigameunlocked	= false;
    game_values.poweruppreset		= 0;
    game_values.tournamentcontrolstyle = 0;

    game_values.pfFilters			= new bool[NUM_AUTO_FILTERS + filterslist->GetCount()];
    game_values.piFilterIcons		= new short[NUM_AUTO_FILTERS + filterslist->GetCount()];
    game_values.fNeedWriteFilters	= false;

    for(short iFilter = 0; iFilter < NUM_AUTO_FILTERS + filterslist->GetCount(); iFilter++) {
        game_values.pfFilters[iFilter] = false;
        game_values.piFilterIcons[iFilter] = 0;
    }

    //game_values.networktype		= 0;
    //game_values.networkhost		= false;
    //game_values.gamehost			= false;

    //Set the default powerup weights for bonus wheel and [?] boxes
    for(short iPreset = 0; iPreset < NUM_POWERUP_PRESETS; iPreset++) {
        for(short iPowerup = 0; iPowerup < NUM_POWERUPS; iPowerup++) {
            g_iCurrentPowerupPresets[iPreset][iPowerup] = g_iDefaultPowerupPresets[iPreset][iPowerup];
        }
    }

    UpdateMusicWithOverrides();

    announcerlist->SetCurrent(0);
    musiclist->SetCurrent(0);
    worldmusiclist->SetCurrent(0);
    menugraphicspacklist->SetCurrent(0);
    worldgraphicspacklist->SetCurrent(0);
    gamegraphicspacklist->SetCurrent(0);
    soundpacklist->SetCurrent(0);

    for(short iPlayer = 0; iPlayer < 4; iPlayer++) {
        game_values.storedpowerups[iPlayer] = -1;
        game_values.gamepowerups[iPlayer] = -1;
        game_values.teamids[iPlayer][0] = iPlayer;
        game_values.teamcounts[iPlayer] = 1;
        game_values.skinids[iPlayer] = 0;
        game_values.colorids[iPlayer] = iPlayer;
        game_values.randomskin[iPlayer] = false;

        //Setup the default key/button input configurations
        for(short iInputType = 0; iInputType < 2; iInputType++) { //for keyboard/joystick
            game_values.inputConfiguration[iPlayer][iInputType].iDevice = iInputType - 1;

            for(short iInputState = 0; iInputState < 2; iInputState++) { //for game/menu
                for(short iKey = 0; iKey < NUM_KEYS; iKey++) {
                    game_values.inputConfiguration[iPlayer][iInputType].inputGameControls[iInputState].keys[iKey] = controlkeys[iInputType][iInputState][iPlayer][iKey];
                }
            }
        }

        //Set the players input to the default configuration (will be overwritten by options.bin settings)
#ifdef _XBOX
        game_values.inputConfiguration[iPlayer][1].iDevice = iPlayer;
        game_values.playerInput.inputControls[iPlayer] = &game_values.inputConfiguration[iPlayer][1];
#else
        game_values.playerInput.inputControls[iPlayer] = &game_values.inputConfiguration[iPlayer][0];
#endif
    }

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

    SetupDefaultGameModeSettings();

	game_values.ReadBinaryConfig();

    //Assign the powerup weights to the selected preset
    for(short iPowerup = 0; iPowerup < NUM_POWERUPS; iPowerup++) {
        game_values.powerupweights[iPowerup] = g_iCurrentPowerupPresets[game_values.poweruppreset][iPowerup];
    }

#ifdef _XBOX
    gfx_setresolution(smw->ScreenWidth, smw->ScreenHeight, false); //Sets flicker filter
    SDL_SetHardwareFilter(game_values.hardwarefilter);
    blitdest = screen;
#else
    if(game_values.fullscreen) {
        gfx_setresolution(smw->ScreenWidth, smw->ScreenHeight, true);
        blitdest = screen;
    }
#endif

    //Calculate the swirl spawn effect locations
    float spawnradius = 100.0f;
    float spawnangle = 0.0f;

    for(short i = 0; i < 25; i++) {
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

    //Load the gfx color palette
    gfx_loadpalette();

    //Call to setup input optimization
    game_values.playerInput.CheckIfMouseUsed();

    srand((unsigned int)time(NULL));
/*
    bool fLoadOK = LoadAndSplashScreenState();

    if(!fLoadOK) {
        printf("\n---------------- EXIT DURING LOADING ----------------\n\n");
        sfx_close();
        gfx_close();
        net_close();
        return 0;
    }

#if	_DEBUG && !WIN32
	HashTableTest();
#endif
*/


    //**********************************************************


    smw->Go(); // all the game logic happens here


    //**********************************************************



    printf("\n---------------- shutdown ----------------\n");

    for(short i = 0; i < GAMEMODE_LAST; i++)
        delete gamemodes[i];

#ifdef _XBOX
    for(i = 0; i < joystickcount; i++)
        SDL_JoystickClose(joysticks[i]);

    delete[] joysticks;
#endif

    sfx_close();
    gfx_close();
    net_close();

    //Delete player skins
    for(short k = 0; k < MAX_PLAYERS; k++) {
        for(short j = 0; j < PGFX_LAST; j++) {
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
    for(int i = 0; i < joystickcount; i++)
        SDL_JoystickClose(joysticks[i]);

    delete[] joysticks;

    SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
    SDL_InitSubSystem(SDL_INIT_JOYSTICK);

    joystickcount = SDL_NumJoysticks();
    joysticks = new SDL_Joystick*[joystickcount];

    for(i = 0; i < joystickcount; i++)
        joysticks[i] = SDL_JoystickOpen(i);

    SDL_JoystickEventState(SDL_ENABLE);
}

#endif
*/

