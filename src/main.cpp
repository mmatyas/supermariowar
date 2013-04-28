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

/*
Procedure for adding a new game mode:
1) Add class to gamemodes.cpp and gamemodes.h
2) Add game mode type to GameModeType enum in gamemodes.h
3) Add new game mode to gamemodes array in main.cpp (gamemodes[X] line 1187)
4) Update MI_SelectField * miGoalField[22]; line in menu.h to match the new number of modes
5) Update UI_Menu mModeSettingsMenu[22]; line in modeoptionsmenu.h to accomodate a new settings menu
6) Update modeoptionsmenu.cpp: ModeOptionsMenu::Refresh()
7) Add game mode options to GameModeSettings in global.h
8) Set default settings for settings in main.cpp
9) Add menu fields to support these new options in menu.cpp and menu.h
10) Update ParseTourStopLine() and WriteTourStopLine() in global.cpp
11) Update SetRandomGameModeSettings() in modeoptionsmenu.cpp
12) Add new mode gfx to gfx\packs\Classic\menu\menu_mode_large.png and menu_mode_small.png
13) Update fShowSettingsButton[] array in menu.cpp
14) Remove old options.bin (new settings will now be read from it)
15) Change line 3186 in main.cpp: if(iMode == game_mode_pipe_minigame)
16) Change line 3274 in main.cpp: if(iMode == game_mode_pipe_minigame)
17) Update tours/0smw.txt to have documentation of mode and options
18) Update worldeditor.cpp: "miModeField = new MI_ImageSelectField" to add new game mode
19) Update worldeditor.cpp: "g_iNumGameModeSettings[]"


Procedure for adding a new game mode option:
1) Add game mode options to GameModeSettings in global.h
2) Add to SetupDefaultGameModeSettings() in global.cpp
3) Add menu fields to support these new options in modeoptionsmenu.cpp and modeoptionsmenu.h
4) Update ParseTourStopLine() and WriteTourStopLine() in global.cpp
5) Update SetRandomGameModeSettings() in modeoptionsmenu.cpp
6) Remove old options.bin (new settings will now be read from it)
7) Update tours/0smw.txt to have documentation of new mode option
8) Update worldeditor.cpp: miModeField = new MI_ImageSelectField to add new game mode
9) Update worldeditor.cpp: g_iNumGameModeSettings[]


Procedure for adding a new powerup:
1) Add class to object.cpp and object.h inhieriting from MO_Powerup
2) Update CPlayer::move() to handle using new stored powerup
3) Update NUM_POWERUPS in global.h
4) Update CPlayer::SetPowerup() to use this powerup
5) Add new gfx to gfx\packs\Classic\powerups\large.png and small.png
6) Update iPowerupPositionMap[] in uicontrol.cpp
7) Add to kill style array potentially for greed mode
*/

#ifdef _XBOX
#include <xtl.h>
#endif

#include "global.h"				//all the global stuff

#include "Game.h"

#include <time.h>
#include <math.h>

//now it's really time for an "engine" (aka resource manager)
#ifdef _WIN32
#pragma comment(lib, "SDL_image.lib")

#ifndef _XBOX
#pragma comment(lib, "SDL.lib")
#pragma comment(lib, "SDLmain.lib")
#pragma comment(lib, "SDL_mixer.lib")

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#endif
#endif


//------ system stuff ------
SDL_Surface		*screen;		//for gfx (maybe the gfx system should be improved -> resource manager)
SDL_Surface		*blitdest;		//the destination surface for all drawing (can be swapped from screen to another surface)

short			g_iCurrentDrawIndex = 0;

short			x_shake = 0;
short			y_shake = 0;

//Vars that keep track of spinning the screen
float spinangle = 0.0f;
float spinspeed = 0.0f;
short spindirection = 1;
short spintimer = 0;

//------ game relevant stuff ------
CPlayer			*list_players[4];
short			list_players_cnt = 0;

CScore			*score[4];

short			score_cnt;
short			scoreoffsets[3] = {2, 36, 70};
short			scorepowerupoffsets[3][3] = {{37, 0, 0},
    {71, 89, 0},
    {105, 123, 141}
};
short			respawn[4] = {0, 0, 0, 0};
short			respawnanimationtimer[4] = {0, 0, 0, 0};
short			respawnanimationframe[4] = {0, 0, 0, 0};

short			projectiles[4];

extern short controlkeys[2][2][4][NUM_KEYS];
extern int g_iVersion[];

//Locations for swirl spawn effects
short g_iSwirlSpawnLocations[4][2][25];

CMap			*g_map;
CTilesetManager *g_tilesetmanager;

CEyecandyContainer eyecandy[3];
SpotlightManager spotlightManager;

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

short g_iWinningPlayer;

extern short g_iPowerupToIcon[6];

extern void SetupScoreBoard(bool fOrderMatters);
extern void ShowScoreBoard();

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

extern short g_iCollisionMap[MOVINGOBJECT_LAST][MOVINGOBJECT_LAST];

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

//handles a collision between a powerup and an object
void collisionhandler_o2o(IO_MovingObject * o1, IO_MovingObject * o2)
{
    o2->collide(o1);
}

short CountAliveTeams(short * lastteam)
{
    short findlastteam = 0;

    bool teamalive[4] = {false, false, false, false};
    for(short k = 0; k < list_players_cnt; k++) {
        if(!list_players[k]->isdead())
            teamalive[list_players[k]->teamID] = true;
    }

    short numteams = 0;
    for(short k = 0; k < 4; k++) {
        if(teamalive[k]) {
            findlastteam = k;
            numteams++;
        }
    }

    if(lastteam != NULL) {
        if(numteams == 1)
            *lastteam = findlastteam;
        else
            *lastteam = -1;
    }

    return numteams;
}

void CleanDeadPlayers()
{
    bool fCheckForGameOver = false;

    for(short i = 0; i < list_players_cnt; i++) {
        if(list_players[i]->state == player_dead) {
            fCheckForGameOver = true;

            if(respawn[list_players[i]->globalID] <= 0)
                list_players[i]->die(0, true, false);

            //Set this to zero so we don't display a spawn egg when player is removed from game
            respawn[list_players[i]->globalID] = 0;

            if(game_values.gamemode->tagged == list_players[i])
                game_values.gamemode->tagged = NULL;

            if(game_values.gamemode->chicken == list_players[i])
                game_values.gamemode->chicken = NULL;

            delete list_players[i];

            for(short j = i; j < list_players_cnt - 1; j++) {
                list_players[j] = list_players[j + 1];
                list_players[j]->localID = j;
            }

            list_players_cnt--;
            list_players[list_players_cnt] = NULL;
        }
    }

    if(fCheckForGameOver && game_values.gamemode->gamemode != game_mode_bonus && game_values.gamemode->gamemode != game_mode_boss_minigame) {
        short lastteam = -1;
        if(!game_values.gamemode->gameover && CountAliveTeams(&lastteam) <= 1) {
            game_values.gamemode->gameover = true;
            game_values.gamemode->winningteam = lastteam;
            SetupScoreBoard(true);  //pass true because the order the players died in matters
            ShowScoreBoard();
        }
    }
}

//Must only be called after organizeteams() is called
short LookupTeamID(short id, short * teamID, short * subTeamID)
{
    for(short i = 0; i < score_cnt; i++) {
        for(short j = 0; j < game_values.teamcounts[i]; j++) {
            if(game_values.teamids[i][j] == id) {
                if(teamID)
                    *teamID = i;

                if(subTeamID)
                    *subTeamID = j;

                return i;
            }
        }
    }

    if(teamID)
        *teamID = -1;

    if(subTeamID)
        *subTeamID = -1;

    return -1;
}

short LookupTeamID(short id)
{
    return LookupTeamID(id, NULL, NULL);
}

CPlayer * GetPlayerFromGlobalID(short iGlobalID)
{
    for(short i = 0; i < list_players_cnt; i++) {
        if(list_players[i]->globalID == iGlobalID)
            return list_players[i];
    }

    return NULL;
}

sfxSound * g_PlayingSoundChannels[NUM_SOUND_CHANNELS];

void DECLSPEC soundfinished(int channel)
{
    if(!g_PlayingSoundChannels[channel])
        printf("Error: SoundFinished() tried to clear a channel that was already cleared!\n");
    else {
        g_PlayingSoundChannels[channel]->clearchannel();
        g_PlayingSoundChannels[channel] = NULL;
    }
}

void DECLSPEC musicfinished()
{
    if(!game_values.music)
        return;

    if(game_values.gamestate == GS_GAME && !game_values.gamemode->gameover) {
        if(game_values.playnextmusic) {
            musiclist->SetNextMusic(g_map->musicCategoryID, maplist->currentShortmapname(), g_map->szBackgroundFile);
            backgroundmusic[0].load(musiclist->GetCurrentMusic()); //In Game Music
        }

        backgroundmusic[0].play(game_values.playnextmusic, false);
    } else {
        if(fResumeMusic) {
            backgroundmusic[3].play(false, false);
        }
    }
}

short GetModeIconIndexFromMode(short iMode)
{
    if(iMode == game_mode_pipe_minigame)
        iMode = 25;
    else if(iMode == game_mode_boss_minigame)
        iMode = 26;
    else if(iMode == game_mode_boxes_minigame)
        iMode = 27;

    return iMode;
}

void RunGame();
void CleanUp();
bool LoadAndSplashScreen();
void LoadAll();
void SetGameModeSettingsFromMenu();
void LoadMapObjects(bool fPreview);
extern void LoadMapHazards(bool fPreview);
void UpdateScoreBoard();
void PlayNextMusicTrack();
bool IsExitAllowed();
bool IsPauseAllowed();

//Adds music overrides to the music lists
void UpdateMusicWithOverrides();

//Joystick-Init
SDL_Joystick **joysticks = NULL;
short joystickcount = 0;

#ifdef _DEBUG
bool g_fAutoTest = false;
bool g_fRecordTest = false;
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

//Move the screen in a small circle
void SpinScreen()
{
    if(spindirection == 0 || spindirection == 2) {
        if(++spintimer >= 300) {
            spindirection++;
            spintimer = 0;
        }
    } else if(spindirection == 1) {
        spinspeed += 0.0008f;

        if(spinspeed >= 0.05f) {
            spinspeed = 0.05f;
            spindirection++;
        }
    } else {
        spinspeed -= 0.0008f;

        if(spinspeed <= -0.05f) {
            spinspeed = -0.05f;
            spindirection = 0;
        }
    }

    spinangle += spinspeed;

    if(spinangle >= TWO_PI) {
        spinangle -= TWO_PI;
    } else if(spinangle < 0.0f) {
        spinangle += TWO_PI;
    }

    float shakey = spinspeed * smw->ScreenWidth * sin(spinangle);
    if(shakey < 0.0f)
        shakey -= 1.0f;

    x_shake = (short)(spinspeed * smw->ScreenWidth * cos(spinangle));
    y_shake = (short)(shakey);
}

Menu g_Menu;
gv game_values;


#ifdef	WIN32
int CALLBACK WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
	// this will print important debugging information to debug console
	_CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_DEBUG );
	
	RootDataDirectory = lpCmdLine;

    if (strlen(RootDataDirectory) == 0) {
        fprintf(stderr, "Please specify root data directory\n");
        return -1;
    }
#else
// ------ MAIN ------
int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Please specify root data directory\n");
        return -1;
    }

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
    SDL_WM_SetCaption(title, "smw.ico");
    SDL_ShowCursor(SDL_DISABLE);

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
    game_values.gamestate			= GS_MENU;
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

        projectiles[iPlayer] = 0;
        respawn[iPlayer] = 0;

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

    bool fLoadOK = LoadAndSplashScreen();

    if(!fLoadOK) {
        printf("\n---------------- EXIT DURING LOADING ----------------\n\n");
        sfx_close();
        gfx_close();
        //net_close();
        return 0;
    }

#if	_DEBUG
	HashTableTest();
#endif

    // all the game logic happens here
    smw->Go();

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
    //net_close();

    //netServer.cleanup();
    //netClient.cleanup();

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





//-----------------------------------------------------------------------------
// THE GAME LOOP
//-----------------------------------------------------------------------------

bool coldec_player2player(CPlayer * o1, CPlayer * o2);
bool coldec_player2obj(CPlayer * o1, CObject * o2);
bool coldec_obj2obj(CObject * o1, CObject * o2);

extern SDL_Rect iCountDownNumbers[4][4][2];
extern short iCountDownTimes[28];
extern short iCountDownRectSize[28];
extern short iCountDownRectGroup[28];
extern short iCountDownAnnounce[28];

#ifdef _DEBUG
short iSledgeBrotherSetting = 0;
short iSledgeBrotherAttribute = 0;
#endif

void RunGame()
{
    unsigned int	framestart, ticks;
    SDL_Event		event;
    short			i, j;
    float			realfps = 0, flipfps = 0;

    short iCountDownState = 0;
    short iCountDownTimer = 0;

    if(game_values.startgamecountdown && game_values.singleplayermode == -1) {
        iCountDownState = 28;
        iCountDownTimer = iCountDownTimes[0];
    }

    //Reset the screen spin variables
    spinangle = 0.0f;
    spinspeed = 0.0f;
    spindirection = 1;
    spintimer = 0;

    //Reset Secret Counters
    for(short iPlayer = 0; iPlayer < 4; iPlayer++)
        game_values.unlocksecret1part1[iPlayer] = false;

    game_values.unlocksecret1part2 = 0;
    game_values.unlocksecret2part1 = false;
    game_values.unlocksecret2part2 = 0;
    game_values.unlocksecret3part1[0] = 0;
    game_values.unlocksecret3part1[1] = 0;
    game_values.unlocksecret3part1[2] = 0;
    game_values.unlocksecret3part1[3] = 0;
    game_values.unlocksecret3part2[0] = 0;
    game_values.unlocksecret3part2[1] = 0;
    game_values.unlocksecret3part2[2] = 0;
    game_values.unlocksecret3part2[3] = 0;
    game_values.unlocksecretunlocked[0] = false;
    game_values.unlocksecretunlocked[1] = false;
    game_values.unlocksecretunlocked[2] = false;
    game_values.unlocksecretunlocked[3] = false;


    //Reset the keys each time we switch from menu to game and back
    game_values.playerInput.ResetKeys();

#ifdef _DEBUG
    static int exitgametimer = 0;
    exitgametimer = 0;
#endif

    y_shake = 0;
    x_shake = 0;

    //Create players for this game
    for(short iPlayer = 0; iPlayer < 4; iPlayer++) {
        projectiles[iPlayer] = 0;
        respawn[iPlayer] = 0;

        if(game_values.singleplayermode == -1 || game_values.singleplayermode == iPlayer) {
            if(game_values.playercontrol[iPlayer] > 0) {
                short teamid, subteamid;
                LookupTeamID(iPlayer, &teamid, &subteamid);

                CPlayerAI * ai = NULL;
                if(game_values.playercontrol[iPlayer] == 2)
                    ai = new CPlayerAI();

                list_players[list_players_cnt] = new CPlayer(iPlayer, list_players_cnt, teamid, subteamid, game_values.colorids[iPlayer], rm->spr_player[iPlayer], score[teamid], &(respawn[iPlayer]), ai);
                list_players_cnt++;
            } else if(!game_values.keeppowerup) {
                //Reset off player's stored powerups if they are not playing
                game_values.storedpowerups[iPlayer] = -1;
            }
        }

        //If the gamemode allows stored powerups, then assign the game stored slot to the powerup this player has
        if(game_values.gamemode->HasStoredPowerups())
            game_values.gamepowerups[iPlayer] = game_values.storedpowerups[iPlayer];
        else {
            game_values.gamepowerups[iPlayer] = -1;
        }

        game_values.bulletbilltimer[iPlayer] = 0;
        game_values.bulletbillspawntimer[iPlayer] = 0;
    }

    game_values.pausegame = false;
    game_values.exitinggame = false;
    game_values.exityes = false;
    game_values.teamdeadcounter = 0;
    game_values.screenshaketimer = 0;
    game_values.slowdownon = -1;
    game_values.slowdowncounter	= 0;
    game_values.showscoreboard = false;
    game_values.scorepercentmove = 0.0f;
    game_values.playskidsound = false;
    game_values.playinvinciblesound = false;
    game_values.playflyingsound = false;
    game_values.swapplayers = false;
    game_values.swapplayersposition = 0.0f;
    game_values.swapplayersblink = false;
    game_values.swapplayersblinkcount = 0;
    game_values.screenfade = 255;
    game_values.screenfadespeed = -8;
    game_values.noexit = false;
    game_values.noexittimer = 0;
    game_values.forceexittimer = 0;
    game_values.gamewindx = 0.0f;
    game_values.gamewindy = 0.0f;

    game_values.windaffectsplayers = false;
    game_values.spinscreen = false;
    game_values.reversewalk = false;
    game_values.spotlights = false;

    //Initialize game mode
    game_values.gamemode->init();

    short totalspace = 0;
    for(i = 0; i < score_cnt; i++) {
        totalspace += 56 + game_values.teamcounts[i] * 34;
    }
    totalspace += 20 * (score_cnt - 1);

    for(i = 0; i < score_cnt; i++) {
        if(game_values.scoreboardstyle == 0 || game_values.scoreboardstyle == 1) {
            score[i]->x = ((smw->ScreenWidth - totalspace) >> 1);

            for(short k = 0; k < i; k++)
                score[i]->x += 76 + game_values.teamcounts[k] * 34;

            short iScoreOffsetY = 0;
            if(game_values.gamemode->gamemode == game_mode_health || game_values.gamemode->gamemode == game_mode_collection)
                iScoreOffsetY = 18;

            score[i]->y = 5 + (game_values.scoreboardstyle == 1 ? 429 - iScoreOffsetY : 0);
        } else {
            short iScoreOffsetY = 0;
            if(game_values.gamemode->gamemode == game_mode_health || game_values.gamemode->gamemode == game_mode_collection)
                iScoreOffsetY = 18;

            score[i]->x = 5 + (574 - (34 * game_values.teamcounts[i])) * (i % 2);
            score[i]->y = 5 + (429 - iScoreOffsetY) * (i > 1 ? 1 : 0);
        }

        score[i]->fromx = score[i]->x;
        score[i]->fromy = score[i]->y;
        score[i]->place = i;
        score[i]->order = -1;
    }

    for(short iEyeCandyLayer = 0; iEyeCandyLayer < 3; iEyeCandyLayer++) {
        //Clouds
        if(g_map->eyecandy[iEyeCandyLayer] & 1) {
            for(i = 0; i < 4; i++) {
                float velx;			//speed of cloud, small clouds are slower than big ones
                short srcy, w, h;

                if(smw->rng->GetRandBool()) {
                    velx = (short)(smw->rng->GetRandMax(51) - 25) / 10.0f;	//big clouds: -3 - +3 pixel/frame
                    srcy = 0;
                    w = 60;
                    h = 28;
                } else {
                    velx = (short)(smw->rng->GetRandMax(41) - 20) / 10.0f;	//small clouds: -2 - +2 pixel/frame
                    srcy = 28;
                    w = 28;
                    h = 12;
                }

                velx = velx < 0.5f && velx > -0.5f ? 1 : velx;	//no static clouds please

                //add cloud to eyecandy array
				eyecandy[iEyeCandyLayer].add(new EC_Cloud(&rm->spr_clouds, (float)(smw->rng->GetRandMax(smw->ScreenWidth)), (float)(smw->rng->GetRandMax(100)), velx, 0, srcy, w, h));
            }
        }

        //Ghosts
        if(g_map->eyecandy[iEyeCandyLayer] & 2) {
            for(i = 0; i < 8; i++) {
                short iGhostSrcY = (short)(smw->rng->GetRandMax(3)) << 5;	//ghost type
                float velx = (short)(smw->rng->GetRandMax(51) - 25) / 10.0f;	//big clouds: -3 - +3 pixel/frame

                velx = velx < 0.5f && velx > -0.5f ? (smw->rng->GetRandMax(1) ? 1.0f : -1.0f) : velx;	//no static clouds please

                //add cloud to eyecandy array
				eyecandy[iEyeCandyLayer].add(new EC_Ghost(&rm->spr_ghosts, (float)(smw->rng->GetRandMax(smw->ScreenWidth)), (float)smw->rng->GetRandMax(100), velx, 8, 2, velx < 0.0f ? 64 : 0, iGhostSrcY, 32, 32));
            }
        }

        //Leaves
        if(g_map->eyecandy[iEyeCandyLayer] & 4) {
            for(i = 0; i < 15; i++)
                eyecandy[iEyeCandyLayer].add(new EC_Leaf(&rm->spr_leaves, (float)(smw->rng->GetRandMax(smw->ScreenWidth)), (float)smw->rng->GetRandMax(smw->ScreenHeight)));
        }

        //Snow
        if(g_map->eyecandy[iEyeCandyLayer] & 8) {
            for(i = 0; i < 15; i++)
				eyecandy[iEyeCandyLayer].add(new EC_Snow(&rm->spr_snow, (float)(smw->rng->GetRandMax(smw->ScreenWidth)), (float)smw->rng->GetRandMax(smw->ScreenHeight), 0));
        }

        //Fish
        short iFishWeights[] = {20, 20, 15, 10, 10, 5, 10, 10};
        short iFishSettings[][4] = { {0, 0, 64, 44}, {0, 44, 64, 44}, {0, 44, 48, 44}, {32, 32, 16, 12}, {32, 44, 16, 12}, {32, 16, 16, 28}, {32, 0, 32, 28}, {32, 44, 32, 28}};
        if(g_map->eyecandy[iEyeCandyLayer] & 16) {
            for(i = 0; i < 8; i++) {
                float velx = (short)(smw->rng->GetRandMax(41) - 20) / 10.0f;
                velx = velx < 0.5f && velx > -0.5f ? 1.0f : velx; //Keep fish from moving too slowly

                short srcx = iFishSettings[0][0], srcy = iFishSettings[0][1], w = iFishSettings[0][2], h = iFishSettings[0][3];

                short iRandomFish = smw->rng->GetRandMax(100);

                short iFishWeightCount = 0;
                for(short iFish = 0; iFish < 8; iFish++) {
                    iFishWeightCount += iFishWeights[iFish];

                    if(iRandomFish < iFishWeightCount) {
                        srcx = iFishSettings[iFish][0];
                        srcy = iFishSettings[iFish][1];
                        w = iFishSettings[iFish][2];
                        h = iFishSettings[iFish][3];
                        break;
                    }
                }

                //add cloud to eyecandy array
				short iPossibleY = (smw->ScreenHeight - h) / 10;
                float dDestY = (float)(smw->rng->GetRandMax(iPossibleY) + iPossibleY * i);
                eyecandy[iEyeCandyLayer].add(new EC_Cloud(&rm->spr_fish, (float)(smw->rng->GetRandMax(smw->ScreenWidth)), dDestY, velx, srcx + (velx > 0.0f ? 64 : 0), srcy, w, h));
            }
        }

        //Rain
        if(g_map->eyecandy[iEyeCandyLayer] & 32) {
            for(i = 0; i < 20; i++)
                eyecandy[iEyeCandyLayer].add(new EC_Rain(&rm->spr_rain, (float)(smw->rng->GetRandMax(smw->ScreenWidth)), smw->rng->GetRandMax(smw->ScreenHeight)));
        }

        //Bubbles
        if(g_map->eyecandy[iEyeCandyLayer] & 64) {
            for(i = 0; i < 10; i++)
                eyecandy[iEyeCandyLayer].add(new EC_Bubble(&rm->spr_rain, (float)(smw->rng->GetRandMax(smw->ScreenWidth)), smw->rng->GetRandMax(smw->ScreenHeight)));
        }
    }

    short iScoreTextOffset[4];
    for(short iTeam = 0; iTeam < score_cnt; iTeam++) {
        iScoreTextOffset[iTeam] = 34 * game_values.teamcounts[iTeam] + 1;
    }


    short iWindTimer = 0;
    float dNextWind = (float)(smw->rng->GetRandMax(41) - 20) / 4.0f;
    game_values.gamewindx = (float)((smw->rng->GetRandMax(41)) - 20) / 4.0f;

    //Initialize players after game init has finished
    for(short iPlayer = 0; iPlayer < list_players_cnt; iPlayer++)
        list_players[iPlayer]->Init();


    //This is the main game loop
    while (true) {
        framestart = SDL_GetTicks();

        if(iWindTimer <= 0) {
            //Then trigger next wind event
            if(game_values.gamewindx < dNextWind) {
                game_values.gamewindx += 0.02f;

                if(game_values.gamewindx >= dNextWind)
                    iWindTimer = (smw->rng->GetRandMax(60)) + 30;
            } else if(game_values.gamewindx >= dNextWind) {
                game_values.gamewindx -= 0.02f;

                if(game_values.gamewindx <= dNextWind)
                    iWindTimer = (smw->rng->GetRandMax(60)) + 30;
            }
        } else {
            if(--iWindTimer <= 0) {
                dNextWind = (float)((smw->rng->GetRandMax(41)) - 20) / 4.0f;
            }
        }

        /*
        #ifdef _XBOX
                if(joystickcount != SDL_NumJoysticks())
                    reconnectjoysticks();
        #endif
        */
#ifdef _DEBUG

        static short endgametimer = (short)(smw->rng->GetRandMax(200));
        if(g_fAutoTest && !game_values.swapplayers) {
            for(short k = 0; k < list_players_cnt; k++) {
                if(list_players[k]->isready()) {
                    //Detect player is in center of tile only
                    short x = (list_players[k]->ix + HALFPW) / TILESIZE;

					if(list_players[k]->ix + HALFPW >= smw->ScreenWidth)
						x = (list_players[k]->ix + HALFPW - smw->ScreenWidth) / TILESIZE;

                    short y = (list_players[k]->iy + HALFPH) / TILESIZE;

                    int tile = tile_flag_nonsolid;
                    IO_Block * block = NULL;
                    short blocktype = -1;

					if(list_players[k]->iy + HALFPH >= 0 && list_players[k]->iy + HALFPH < smw->ScreenHeight) {
                        tile = g_map->map(x, y);
                        block = g_map->block(x, y);
                        blocktype = g_map->blockat(x, y)->iType;
                    }

                    if((tile & tile_flag_solid) ||
                            (block && blocktype != 3 && blocktype < 11)) {
                        game_values.pausegame = true;
                        game_values.frameadvance = true;
                        g_fAutoTest = false;
                        break;
                    }

                    //Detect if any corner of player is in a tile
                    short actualvalues[2][2];
                    actualvalues[0][0] = list_players[k]->ix;

                    if(actualvalues[0][0] < 0)
						actualvalues[0][0] += smw->ScreenWidth;

                    actualvalues[0][1] = list_players[k]->ix + PW;

					if(actualvalues[0][1] >= smw->ScreenWidth)
						actualvalues[0][1] -= smw->ScreenWidth;

                    actualvalues[1][0] = list_players[k]->iy;
                    actualvalues[1][1] = list_players[k]->iy + PH;

                    short corners[2][2];
                    corners[0][0] = list_players[k]->ix / TILESIZE;

                    if(list_players[k]->ix < 0)
						corners[0][0] = (list_players[k]->ix + smw->ScreenWidth) / TILESIZE;

                    corners[0][1] = (list_players[k]->ix + PW) / TILESIZE;

                    if(list_players[k]->ix + PW >= smw->ScreenWidth)
						corners[0][1] = (list_players[k]->ix + PW - smw->ScreenWidth) / TILESIZE;

                    corners[1][0] = list_players[k]->iy / TILESIZE;
                    corners[1][1] = (list_players[k]->iy + PH) / TILESIZE;

                    for(short i = 0; i < 2; i++) {
                        for(short j = 0; j < 2; j++) {
                            int tile = tile_flag_nonsolid;
                            IO_Block * block = NULL;
                            short blocktype = -1;

                            if(actualvalues[0][j] >= 0 && actualvalues[0][j] < smw->ScreenWidth && actualvalues[1][i] > 0 && actualvalues[1][i] < smw->ScreenHeight) {
                                tile = g_map->map(corners[0][j], corners[1][i]);
                                block = g_map->block(corners[0][j], corners[1][i]);
                                blocktype = g_map->blockat(corners[0][j], corners[1][i])->iType;
                            }

                            if( (tile & tile_flag_solid) ||
                                    (block && blocktype != 3 && blocktype < 11)) {
                                game_values.pausegame = true;
                                game_values.frameadvance = true;
                                g_fAutoTest = false;
                                break;
                            }
                        }

                        if(game_values.pausegame)
                            break;
                    }
                }

                if(game_values.pausegame)
                    break;
            }

#if	0
            //Kill off players to test spawning
            static short chooseplayer = 0;
            static short killtimer = 0;
            if(++killtimer > 20)
            {
                killtimer = 0;

                if(++chooseplayer >= list_players_cnt)
                    chooseplayer = 0;

                list_players[chooseplayer]->DeathAwards();

                if( !game_values.gamemode->playerkilledself(*(list_players[chooseplayer])) )
                {
                    list_players[chooseplayer]->die(0, false);
                }

            //Automatically run menus
            if(game_values.showscoreboard) {
                if(--endgametimer < 0) {
                    endgametimer = (short)(smw->rng->GetRandMax(200));

                    if(game_values.matchtype != MATCH_TYPE_SINGLE_GAME && game_values.matchtype != MATCH_TYPE_QUICK_GAME && game_values.matchtype != MATCH_TYPE_MINIGAME)
                        UpdateScoreBoard();

                    CleanUp();
                    game_values.gamestate = GS_MENU;

                    return;
                }
            } else {
                if(++exitgametimer >= 8000) {
                    CleanUp();
                    game_values.exitinggame = false;
                    game_values.exityes = false;
                    game_values.gamestate = GS_MENU;

                    return;
                }
            }
        }

#endif

        game_values.playerInput.ClearPressedKeys(game_values.exitinggame ? 1 : 0);
        //handle messages

        while(SDL_PollEvent(&event)) {
            switch(event.type) {
#ifndef _XBOX
            case SDL_QUIT: {
                CleanUp();
                game_values.gamestate = GS_QUIT;
                return;
            }
            break;
#endif
            case SDL_KEYDOWN: {
#ifndef _XBOX
                if(event.key.keysym.mod & (KMOD_LALT | KMOD_RALT)) {
                    if(event.key.keysym.sym == SDLK_F4) {
                        CleanUp();
                        game_values.gamestate = GS_QUIT;
                        return;
                    } else if(event.key.keysym.sym == SDLK_RETURN) {
                        game_values.fullscreen = !game_values.fullscreen;
						gfx_setresolution(smw->ScreenWidth, smw->ScreenHeight, game_values.fullscreen);
                        blitdest = screen;

                        g_Menu.miFullscreenField->SetKey(game_values.fullscreen ? 1 : 0);

                        //Continue with input -> don't feed this event to the input
                        //otherwise it will pause the game when switching to full/windowed screen
                        continue;
                    }
                }
#endif
                if(event.key.keysym.sym == SDLK_F1) {
                    game_values.showfps = !game_values.showfps;
                } else if(event.key.keysym.sym == SDLK_ESCAPE) {
                    game_values.playerInput.outputControls[0].game_cancel.fPressed = true;
                } else if(event.key.keysym.sym == SDLK_TAB) {
                    PlayNextMusicTrack();
                }
#ifdef _DEBUG
                else if(event.key.keysym.sym == SDLK_LEFTBRACKET) {
                    game_values.framelimiter++;
                } else if(event.key.keysym.sym == SDLK_RIGHTBRACKET) {
                    if(game_values.framelimiter > 0)
                        game_values.framelimiter--;
                } else if(event.key.keysym.sym == SDLK_F2) {
                    game_values.frameadvance = !game_values.frameadvance;
                } else if(event.key.keysym.sym == SDLK_F5) {
                    game_values.autokill = !game_values.autokill;
                } else if(event.key.keysym.sym == SDLK_z) {
                    for(short k = 0; k < list_players_cnt; k++) {
                        list_players[k]->DeathAwards();

                        if(game_values.gamemode->playerkilledself(*(list_players[k]), kill_style_environment) == player_kill_normal)
                            list_players[k]->die(0, false, false);
                    }
                } else if(event.key.keysym.sym == SDLK_x) {
                    short iplayer = smw->rng->GetRandMax(list_players_cnt);
                    list_players[iplayer]->makefrozen(300);
                } else if(event.key.keysym.sym == SDLK_c) {
                    short iplayer = smw->rng->GetRandMax(list_players_cnt);
                    list_players[iplayer]->shield = smw->rng->GetRandMax(3) + 1;
                    list_players[iplayer]->shieldtimer = 620;
                } else if(event.key.keysym.sym == SDLK_1) {
                    if(event.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL))
                        objectcontainer[0].add(new PU_IceWandPowerup(&rm->spr_icewandpowerup, list_players[0]->ix + 32, list_players[0]->iy, 1, 0, 30, 30, 1, 1));
                    else if(event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT))
                        objectcontainer[0].add(new PU_BobombPowerup(&rm->spr_bobombpowerup, list_players[0]->ix + 32, list_players[0]->iy - 1, 1, true, 0, 30, 30, 1, 1));
                    else
                        objectcontainer[0].add(new PU_StarPowerup(&rm->spr_starpowerup, list_players[0]->ix + 32, list_players[0]->iy, 4, true, 2, 30, 30, 1, 1));
                } else if(event.key.keysym.sym == SDLK_2) {
                    if(event.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL))
                        objectcontainer[0].add(new PU_BombPowerup(&rm->spr_bombpowerup, list_players[0]->ix + 32, list_players[0]->iy, 1, 0, 30, 30, 1, 1));
                    else if(event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT))
                        objectcontainer[0].add(new PU_PowPowerup(&rm->spr_powpowerup, list_players[0]->ix + 32, list_players[0]->iy - 1, 8, true, 8, 30, 30, 1, 1));
                    else
                        objectcontainer[0].add(new PU_ExtraGuyPowerup(&rm->spr_1uppowerup, list_players[0]->ix + 32, list_players[0]->iy, 1, true, 0, 30, 30, 1, 1, 1));
                } else if(event.key.keysym.sym == SDLK_3) {
                    if(event.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL))
                        objectcontainer[0].add(new PU_PodoboPowerup(&rm->spr_podobopowerup, list_players[0]->ix + 32, list_players[0]->iy, 1, 0, 30, 30, 1, 1));
                    else if(event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT))
                        objectcontainer[0].add(new PU_BulletBillPowerup(&rm->spr_bulletbillpowerup, list_players[0]->ix + 32, list_players[0]->iy - 1, 1, true, 0, 30, 30, 1, 1));
                    else
                        objectcontainer[0].add(new PU_ExtraGuyPowerup(&rm->spr_2uppowerup, list_players[0]->ix + 32, list_players[0]->iy, 1, true, 0, 30, 30, 1, 1, 2));
                } else if(event.key.keysym.sym == SDLK_4) {
                    if(event.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL))
                        objectcontainer[0].add(new PU_Tanooki(list_players[0]->ix + 32, list_players[0]->iy));
                    else if(event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT))
                        objectcontainer[1].add(new CO_Shell(0, list_players[0]->ix + 32, list_players[0]->iy, true, true, true, false));
                    else
                        objectcontainer[0].add(new PU_ExtraGuyPowerup(&rm->spr_3uppowerup, list_players[0]->ix + 32, list_players[0]->iy, 1, true, 0, 30, 30, 1, 1, 3));
                } else if(event.key.keysym.sym == SDLK_5) {
                    if(event.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL))
                        objectcontainer[0].add(new PU_PWingsPowerup(&rm->spr_pwingspowerup, list_players[0]->ix + 32, list_players[0]->iy));
                    else if(event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT))
                        objectcontainer[1].add(new CO_Shell(1, list_players[0]->ix + 32, list_players[0]->iy, false, true, true, false));
                    else
                        objectcontainer[0].add(new PU_ExtraGuyPowerup(&rm->spr_5uppowerup, list_players[0]->ix + 32, list_players[0]->iy, 1, true, 0, 30, 30, 1, 1, 5));
                } else if(event.key.keysym.sym == SDLK_6) {
                    if(event.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL))
                        objectcontainer[1].add(new CO_Spring(&rm->spr_spring, list_players[0]->ix + 32, list_players[0]->iy, true));
                    else if(event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT))
                        objectcontainer[1].add(new CO_Shell(2, list_players[0]->ix + 32, list_players[0]->iy, false, false, true, true));
                    else
                        objectcontainer[0].add(new PU_FirePowerup(&rm->spr_firepowerup, list_players[0]->ix + 32, list_players[0]->iy, 1, true, 0, 30, 30, 1, 1));
                } else if(event.key.keysym.sym == SDLK_7) {
                    if(event.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL))
                        objectcontainer[1].add(new CO_ThrowBox(&rm->spr_throwbox, list_players[0]->ix + 32, list_players[0]->iy, (smw->rng->GetRandMax(NUM_POWERUPS) + 3) - 3));
                    else if(event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT))
                        objectcontainer[1].add(new CO_Shell(3, list_players[0]->ix + 32, list_players[0]->iy, false, true, false, false));
                    else
                        objectcontainer[0].add(new PU_HammerPowerup(&rm->spr_hammerpowerup, list_players[0]->ix + 32, list_players[0]->iy, 1, true, 0, 30, 30, 1, 1));
                } else if(event.key.keysym.sym == SDLK_8) {
                    if(event.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL))
                        objectcontainer[1].add(new CO_Spike(&rm->spr_spike, list_players[0]->ix + 32, list_players[0]->iy));
                    else if(event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT))
                        objectcontainer[0].add(new PU_ModPowerup(&rm->spr_modpowerup, list_players[0]->ix + 32, list_players[0]->iy, 8, true, 8, 30, 30, 1, 1));
                    else
                        objectcontainer[0].add(new PU_PoisonPowerup(&rm->spr_poisonpowerup, list_players[0]->ix + 32, list_players[0]->iy, 1, true, 0, 30, 30, 1, 1));
                } else if(event.key.keysym.sym == SDLK_9) {
                    if(event.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL))
                        objectcontainer[1].add(new CO_KuriboShoe(&rm->spr_kuriboshoe, list_players[0]->ix + 32, list_players[0]->iy, true));
                    else if(event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT))
                        objectcontainer[0].add(new PU_FeatherPowerup(&rm->spr_featherpowerup, list_players[0]->ix + 32, list_players[0]->iy - 1, 1, 0, 30, 30, 1, 1));
                    else
                        objectcontainer[0].add(new PU_ClockPowerup(&rm->spr_clockpowerup, list_players[0]->ix + 32, list_players[0]->iy - 1, 1, true, 0, 30, 30, 1, 1));
                } else if(event.key.keysym.sym == SDLK_0) {
                    if(event.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL))
                        objectcontainer[0].add(new PU_LeafPowerup(&rm->spr_leafpowerup, list_players[0]->ix + 32, list_players[0]->iy - 1, 1, 0, 30, 30, 1, 1));
                    else if(event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT))
                        objectcontainer[0].add(new PU_BoomerangPowerup(&rm->spr_boomerangpowerup, list_players[0]->ix + 32, list_players[0]->iy - 1, 1, true, 0, 30, 30, 1, 1));
                    else
                        objectcontainer[0].add(new PU_MysteryMushroomPowerup(&rm->spr_mysterymushroompowerup, list_players[0]->ix + 32, list_players[0]->iy - 1, 1, true, 0, 30, 30, 1, 1));
                } else if(event.key.keysym.sym == SDLK_INSERT) {
                    g_fAutoTest = !g_fAutoTest;
                }
#endif
                break;
            }

#ifdef _XBOX
            case SDL_JOYBUTTONDOWN: {
                if(event.jbutton.state == SDL_PRESSED && event.jbutton.button == 5) {
                    PlayNextMusicTrack();
                }

                break;
            }
#endif

            default:
                break;
            }

            //Feed the player control structures with input data
            //Use menu controls when exit game dialog is up
            game_values.playerInput.Update(event, (game_values.exitinggame ? 1 : 0));
        }

        if(game_values.screenfade == 0 && iCountDownState <= COUNTDOWN_START_INDEX) {
            //If the cancel button is pressed
            if(game_values.forceexittimer > 0) {
                if(--game_values.forceexittimer <= 0) {
                    game_values.gamestate = GS_END_GAME;
                    game_values.screenfade = 8;
                    game_values.screenfadespeed = 8;
                }
            }

            for(int iPlayer = 0; iPlayer < 4; iPlayer++) {
                COutputControl * playerKeys = &game_values.playerInput.outputControls[iPlayer];

                //If the start key is pressed (pause key)
                if(playerKeys->game_start.fPressed && IsPauseAllowed()) {
                    if(!game_values.showscoreboard && !game_values.exitinggame) {
                        game_values.pausegame = !game_values.pausegame;

                        if(game_values.pausegame) {
                            rm->menu_shade.setalpha(SCREEN_BACKGROUND_FADE	);
                            rm->menu_shade.draw(0, 0);

                            //Stop the pwings sound if it is on
                            if(sfx_flyingsound.isplaying())
                                ifsoundonstop(sfx_flyingsound);
                        }

                        //ifsoundonpause(sfx_invinciblemusic);
                        //ifsoundonpause(sfx_slowdownmusic);
                        ifsoundonplay(sfx_pause);
                    }
                }

                //Only player 1 is allowed to exit a game
                //if(iPlayer != 0)
                //	continue;

                if((playerKeys->game_cancel.fPressed || (playerKeys->game_start.fPressed && game_values.gamemode->gameover)) && IsExitAllowed()) {
                    if(game_values.gamemode->gameover) {
                        if(game_values.matchtype == MATCH_TYPE_TOUR || game_values.matchtype == MATCH_TYPE_TOURNAMENT)
                            UpdateScoreBoard();

                        CleanUp();
                        game_values.gamestate = GS_MENU;

                        return;
                    } else {
                        if(!game_values.pausegame && !game_values.exitinggame) {
                            rm->menu_shade.setalpha(SCREEN_BACKGROUND_FADE	);
                            rm->menu_shade.draw(0, 0);
                            game_values.exitinggame = true;
                            //ifsoundonpause(sfx_invinciblemusic);
                            //ifsoundonpause(sfx_slowdownmusic);

                            //Reset the keys each time we switch from menu to game and back
                            game_values.playerInput.ResetKeys();
                        }
                    }
                }

                //Deal with input to game exit dialog box
                if(game_values.exitinggame) {
                    if(playerKeys->menu_left.fPressed)
                        game_values.exityes = true;
                    else if(playerKeys->menu_right.fPressed)
                        game_values.exityes = false;

                    if(playerKeys->menu_select.fPressed) {
                        if(game_values.exityes) {
                            CleanUp();
                            game_values.exitinggame = false;
                            game_values.exityes = false;
                            game_values.gamestate = GS_MENU;
                            return;
                        } else {
                            game_values.exitinggame = false;
                            //ifsoundonpause(sfx_invinciblemusic);
                            //ifsoundonpause(sfx_slowdownmusic);

                            //Reset the keys each time we switch from menu to game and back
                            game_values.playerInput.ResetKeys();
                        }
                    }
                }
            }
        }

        if(!game_values.pausegame && !game_values.exitinggame) {
            if(!game_values.swapplayers && game_values.screenfade == 0) {
                //Count down start timer before each game
                if(iCountDownState > 0 && --iCountDownTimer <= 0) {
                    //There is one extra count here so we hit all the numbers.  When we
                    //reach 0 that means we are done counting (index 28 would be out of
                    //bounds on this array)
                    if(--iCountDownState != 0) {
                        //28 is the magic number as there are 28 frames of animation
                        //spread over a few seconds for the countdown numbers
                        iCountDownTimer = iCountDownTimes[28 - iCountDownState];

                        short countDownAnnounce = iCountDownAnnounce[28 - iCountDownState];
                        if(countDownAnnounce >= 0)
                            ifsoundonandreadyplay(sfx_announcer[countDownAnnounce]);
                    }
                }

                //Make updates to background stuff (animate map while countdown is counting)
                if(iCountDownState > COUNTDOWN_START_INDEX) {
                    //Move platforms
                    g_map->updatePlatforms();

                    //Keep updating map hazards
                    objectcontainer[0].cleandeadobjects();
                    objectcontainer[1].cleandeadobjects();
                    objectcontainer[2].cleandeadobjects();
                    noncolcontainer.cleandeadobjects();

                    noncolcontainer.update();
                    objectcontainer[0].update();
                    objectcontainer[1].update();
                    objectcontainer[2].update();

                    eyecandy[0].cleandeadobjects();
                    eyecandy[1].cleandeadobjects();
                    eyecandy[2].cleandeadobjects();

                    eyecandy[0].update();
                    eyecandy[1].update();
                    eyecandy[2].update();

                    g_map->update();
                } else {
                    //Shake screen

                    if(game_values.screenshaketimer > 0) {
                        game_values.screenshaketimer--;

                        static bool shakeleft = false;
                        if(shakeleft) {
                            x_shake -= 2;
                            if(x_shake <= -2) {
                                shakeleft = false;
                            }
                        } else {
                            x_shake += 2;
                            if(x_shake >= 2) {
                                shakeleft = true;
                            }
                        }

                        //Kill players touching the ground (or in air for MOd blocks)
                        short iNumKillPlayers = 0;
                        CPlayer * pKillPlayers[4];

                        CPlayer * killer = GetPlayerFromGlobalID(game_values.screenshakeplayerid);

                        for(short k = 0; k < list_players_cnt; k++) {
                            CPlayer * player = list_players[k];

                            //Don't kill the player that triggered the POW/MOd
                            if(player->globalID == game_values.screenshakeplayerid)
                                continue;

                            //Don't kill players on his team either (if friendly fire is off)
                            if(game_values.teamcollision != 2 && game_values.screenshaketeamid == player->teamID)
                                continue;

                            //Kill other players
                            if(!player->invincible && player->shield == 0 && player->iKuriboShoe == 0 && player->isready()) {
                                if(game_values.screenshakekillinair == player->inair) {
                                    pKillPlayers[iNumKillPlayers++] = player;

                                    if(killer) {
                                        game_values.screenshakekillscount++;

                                        if(killer->inair)
                                            killer->killsinrowinair--;  //Don't want to give both shake and in air award
                                    }
                                }
                            }
                        }

                        //Randomize the order in which the players are killed (so that game modes where order matters is fair)
                        if(iNumKillPlayers > 0) {
                            short iRandPlayer = smw->rng->GetRandMax( iNumKillPlayers);
                            for(short iPlayer = 0; iPlayer < iNumKillPlayers; iPlayer++) {
                                PlayerKilledPlayer(game_values.screenshakeplayerid, pKillPlayers[iRandPlayer], death_style_jump, kill_style_pow, false, false);

                                if(++iRandPlayer >= iNumKillPlayers)
                                    iRandPlayer = 0;
                            }
                        }

                        //Kill goombas and koopas
                        for(short k = 0; k < objectcontainer[0].list_end; k++) {
                            CObject * object = objectcontainer[0].list[k];
                            if(object->getObjectType() == object_moving) {
                                IO_MovingObject * movingobject = (IO_MovingObject *)object;
                                MovingObjectType type = movingobject->getMovingObjectType();

                                if((type == movingobject_goomba || type == movingobject_koopa || type == movingobject_buzzybeetle || type == movingobject_spiny)
                                        && game_values.screenshakekillinair == movingobject->inair) {
                                    CPlayer * killer = GetPlayerFromGlobalID(game_values.screenshakeplayerid);

                                    if(killer) {
                                        if(!game_values.gamemode->gameover)
                                            killer->score->AdjustScore(1);

                                        ifsoundonplay(sfx_kicksound);
                                        ((MO_WalkingEnemy*)movingobject)->DieAndDropShell(true, true);

                                        game_values.screenshakekillscount++;

                                        if(killer->inair)
                                            killer->killsinrowinair--;  //Don't want to give both shake and in air award
                                    }
                                }
                            }
                        }

                        //Destroy throw blocks and flip shells over
                        for(short k = 0; k < objectcontainer[1].list_end; k++) {
                            CObject * object = objectcontainer[1].list[k];
                            if(object->getObjectType() == object_moving) {
                                IO_MovingObject * movingobject = (IO_MovingObject *)object;

                                if(game_values.screenshakekillinair == movingobject->inair) {
                                    if(movingobject->getMovingObjectType() == movingobject_shell) {
                                        CO_Shell * shell = (CO_Shell*)movingobject;
                                        if(shell->frozen || !shell->owner || shell->owner->inair == game_values.screenshakekillinair)
                                            shell->Flip();  //also breaks shells if frozen
                                    } else if(movingobject->getMovingObjectType() == movingobject_throwblock) {
                                        CO_ThrowBlock * throwblock = (CO_ThrowBlock*)movingobject;
                                        if(throwblock->frozen || !throwblock->owner || throwblock->owner->inair == game_values.screenshakekillinair)
                                            throwblock->Die();
                                    } else if(movingobject->getMovingObjectType() == movingobject_throwbox) {
                                        CO_ThrowBox * throwbox = (CO_ThrowBox*)movingobject;
                                        if(throwbox->frozen)
                                            throwbox->Die();
                                    } else if(movingobject->getMovingObjectType() == movingobject_pirhanaplant) {
                                        MO_PirhanaPlant * plant = (MO_PirhanaPlant*)movingobject;
                                        plant->KillPlant();
                                    } else if(movingobject->getMovingObjectType() == movingobject_bulletbill) {
                                        MO_BulletBill * bulletbill = (MO_BulletBill*)movingobject;
                                        bulletbill->Die();
                                    }
                                }
                            }
                        }

                        //Add kills in row for kills from pow and mod
                        if(game_values.screenshakekillscount > 1 && game_values.awardstyle != award_style_none) {
                            game_values.screenshakekillscount = 0;

                            CPlayer * killer = GetPlayerFromGlobalID(game_values.screenshakeplayerid);

                            if(killer)
                                killer->AddKillsInRowInAirAward();
                        }
                    } else {
                        //Make sure we zero out the shake value after it is done
                        x_shake = 0;
                    }

                    if(game_values.spinscreen) {
                        SpinScreen();
                    }

                    for(short iPlayer = 0; iPlayer < 4; iPlayer++) {
                        if(game_values.bulletbilltimer[iPlayer] > 0) {
                            game_values.bulletbilltimer[iPlayer]--;

                            if(--game_values.bulletbillspawntimer[iPlayer] <= 0) {
                                game_values.bulletbillspawntimer[iPlayer] = (short)(smw->rng->GetRandMax(20) + 25);
                                float speed = ((float)(smw->rng->GetRandMax(21) + 20)) / 10.0f;
                                objectcontainer[2].add(new MO_BulletBill(&rm->spr_bulletbill, &rm->spr_bulletbilldead, 0, (short)(smw->rng->GetRandMax(448)), (smw->rng->GetRandMax(2) ? speed : -speed), iPlayer, false));
                                ifsoundonplay(sfx_bulletbillsound);
                            }
                        }
                    }

                    if(game_values.matchtype == MATCH_TYPE_WORLD && game_values.gamemode->gameover && game_values.forceexittimer <= 0) {
                        if(--game_values.noexittimer <= 0)
                            game_values.noexit = false;
                    }

                    //------------- update objects -----------------------

#ifdef _DEBUG
                    if(game_values.autokill) {
                        for(short k = 0; k < list_players_cnt; k++) {
                            list_players[k]->DeathAwards();

                            if(!game_values.gamemode->playerkilledself(*(list_players[k]), kill_style_environment))
                                list_players[k]->die(0, false, false);
                        }
                    }
#endif

                    //Advance the cpu's turn (AI only calculates player's actions 1 out of 4 frames)
                    if(++game_values.cputurn > 3)
                        game_values.cputurn = 0;

                    //Player to player collisions
                    for(i = 0; i < list_players_cnt; i++) {
                        CPlayer * player1 = list_players[i];
                        if(player1->state > player_dead) {
                            for(j = i + 1; j < list_players_cnt; j++) {
                                CPlayer * player2 = list_players[j];
                                if(player2->state > player_dead) {
                                    if(coldec_player2player(player1, player2)) {
                                        collisionhandler_p2p(player1, player2);

                                        //if player was killed by another player, continue with next player for collision detection
                                        if(player1->state <= player_dead)
                                            break;
                                    }
                                }
                            }
                        }
                    }

                    //Move platforms
                    g_map->updatePlatforms();

                    game_values.playskidsound = false;
                    game_values.playinvinciblesound = false;
                    game_values.playflyingsound = false;

                    for(i = 0; i < list_players_cnt; i++)
                        list_players[i]->move();	//move all objects before doing object-object collision detection in
                    //->think(), so we test against the new position after object-map collision detection

                    //Play sound for skidding players
                    if(game_values.playskidsound) {
                        if(!sfx_skid.isplaying())
                            ifsoundonplay(sfx_skid);
                    } else {
                        if(sfx_skid.isplaying())
                            ifsoundonstop(sfx_skid);
                    }

                    //Play sound for players using PWings
                    if(game_values.playflyingsound) {
                        if(!sfx_flyingsound.isplaying())
                            ifsoundonplay(sfx_flyingsound);
                    } else {
                        if(sfx_flyingsound.isplaying())
                            ifsoundonstop(sfx_flyingsound);
                    }

                    noncolcontainer.update();
                    objectcontainer[0].update();
                    objectcontainer[1].update();
                    objectcontainer[2].update();

                    //Collide player with objects
                    for(short iPlayer = 0; iPlayer < list_players_cnt; iPlayer++) {
                        CPlayer * player = list_players[iPlayer];
                        if(player->state != player_ready)
                            continue;

                        //Collide with objects
                        for(short iLayer = 0; iLayer < 3; iLayer++) {
                            for(short iObject = 0; iObject < objectcontainer[iLayer].list_end; iObject++) {
                                CObject * object = objectcontainer[iLayer].list[iObject];

                                if(!object->GetDead()) {
                                    if(coldec_player2obj(player, object)) {
                                        if(collisionhandler_p2o(player, object))
                                            break;
                                    }
                                }
                            }

                            //if the object killed the player, then continue with the other players
                            if(player->state != player_ready)
                                break;

                            //If player collided with a swap mushroom, the break from colliding with everything else
                            if(game_values.swapplayers)
                                goto SWAPBREAK;
                        }
                    }

                    for(short iLayer1 = 0; iLayer1 < 3; iLayer1++) {
                        short iContainerEnd1 = objectcontainer[iLayer1].list_end;
                        for(short iObject1 = 0; iObject1 < iContainerEnd1; iObject1++) {
                            CObject * object1 = objectcontainer[iLayer1].list[iObject1];

                            if(object1->getObjectType() != object_moving)
                                continue;

                            IO_MovingObject * movingobject1 = (IO_MovingObject*)object1;

                            for(short iLayer2 = iLayer1; iLayer2 < 3; iLayer2++) {
                                short iContainerEnd2 = objectcontainer[iLayer2].list_end;
                                for(short iObject2 = (iLayer1 == iLayer2 ? iObject1 + 1 : 0); iObject2 < iContainerEnd2; iObject2++) {
                                    CObject * object2 = objectcontainer[iLayer2].list[iObject2];

                                    if(object2->getObjectType() != object_moving)
                                        continue;

                                    IO_MovingObject * movingobject2 = (IO_MovingObject*)object2;

                                    //if(g_iCollisionMap[movingobject1->getMovingObjectType()][movingobject2->getMovingObjectType()])
                                    //	continue;

                                    //if(iLayer1 == iLayer2 && iObject1 == iObject2)
                                    //	continue;

                                    if(object2->GetDead())
                                        continue;

                                    MovingObjectType iType1 = movingobject1->getMovingObjectType();
                                    MovingObjectType iType2 = movingobject2->getMovingObjectType();
                                    if(g_iCollisionMap[iType1][iType2]) {
                                        if(coldec_obj2obj(movingobject1, movingobject2)) {
                                            collisionhandler_o2o(movingobject1, movingobject2);
                                        }
                                    } else if(g_iCollisionMap[iType2][iType1]) {
                                        if(coldec_obj2obj(movingobject2, movingobject1)) {
                                            collisionhandler_o2o(movingobject2, movingobject1);
                                        }
                                    }

                                    if(object1->GetDead())
                                        goto CONTINUEOBJECT1;
                                }
                            }

                            //Labeled break
CONTINUEOBJECT1:
                            continue;
                        }
                    }

                    //Commit all player actions at this point (after we have collided with any objects
                    //that the player might have picked up)
                    for(i = 0; i < list_players_cnt; i++)
                        list_players[i]->CommitAction();

                    eyecandy[0].cleandeadobjects();
                    eyecandy[1].cleandeadobjects();
                    eyecandy[2].cleandeadobjects();

                    objectcontainer[2].cleandeadobjects();
                    objectcontainer[1].cleandeadobjects();
                    objectcontainer[0].cleandeadobjects();
                    noncolcontainer.cleandeadobjects();

                    CleanDeadPlayers();

                    eyecandy[0].update();
                    eyecandy[1].update();
                    eyecandy[2].update();

                    game_values.gamemode->think();

                    if(game_values.slowdownon != -1 && ++game_values.slowdowncounter > 580) {
                        game_values.slowdownon = -1;
                        game_values.slowdowncounter = 0;
                    }

                    g_map->update();

                    if(y_shake > 0 && !game_values.spinscreen) {
                        y_shake -= CRUNCHVELOCITY;

                        if(y_shake < 0)
                            y_shake = 0;
                    }

                    if(game_values.showscoreboard) {
                        if(game_values.scorepercentmove < 1.0f) {
                            game_values.scorepercentmove += 0.01f;

                            if(game_values.scorepercentmove >= 1.0f)
                                game_values.scorepercentmove = 1.0f;
                        } else {
                            game_values.scorepercentmove = 1.0f;
                        }

                        for(i = 0; i < score_cnt; i++) {
                            score[i]->x = (short)((float)(score[i]->destx - score[i]->fromx) * game_values.scorepercentmove) + score[i]->fromx;
                            score[i]->y = (short)((float)(score[i]->desty - score[i]->fromy) * game_values.scorepercentmove) + score[i]->fromy;
                        }
                    }
                }
            }
            //Go to this label if a player collects a swap mushroom and we need to break out of two loops
SWAPBREAK:

            if(game_values.swapplayers) {
                for(i = 0; i < list_players_cnt; i++) {
                    list_players[i]->updateswap();
                }
            }

            if(game_values.screenfade == 255) {
                if(game_values.gamestate == GS_START_GAME) {
                    CleanUp();
                    SetGameModeSettingsFromMenu();
                    game_values.gamestate = GS_GAME;

                    /*
                    //This code will help stop a game midway and load a new map and mode
                    //It was used for the secret boss mode in 1.7 AFE
                    game_values.gamemode = bossgamemode;  //boss type has already been set at this point
                    bossgamemode->SetBossType(0);

                    if(bossgamemode->GetBossType() == 0)
                        g_map->loadMap(convertPath("maps/special/dungeon.map"), read_type_full);
                    else if(bossgamemode->GetBossType() == 1)
                        g_map->loadMap(convertPath("maps/special/hills.map"), read_type_full);
                    else if(bossgamemode->GetBossType() == 2)
                        g_map->loadMap(convertPath("maps/special/volcano.map"), read_type_full);

                    LoadCurrentMapBackground();

                    g_map->predrawbackground(rm->spr_background, rm->spr_backmap[0]);
                    g_map->predrawforeground(rm->spr_frontmap[0]);

                    g_map->predrawbackground(rm->spr_background, rm->spr_backmap[1]);
                    g_map->predrawforeground(rm->spr_frontmap[1]);

                    g_map->SetupAnimatedTiles();
                    LoadMapObjects(false);
                    ///////////////////
                    */

                    if(game_values.music) {
                        musiclist->SetRandomMusic(g_map->musicCategoryID, "", "");
                        backgroundmusic[0].load(musiclist->GetCurrentMusic());
                        backgroundmusic[0].play(game_values.playnextmusic, false);
                    }

                    return;
                } else if(game_values.gamestate == GS_END_GAME) {
                    CleanUp();
                    game_values.gamestate = GS_MENU;
                    game_values.screenfadespeed = -8;

                    return;
                }
            }

            //--------------- draw everything ----------------------
            rm->spr_backmap[g_iCurrentDrawIndex].draw(0, 0);

            //draw back eyecandy behind players
            g_map->drawPlatforms(0);

            eyecandy[0].draw();
            noncolcontainer.draw();

            game_values.gamemode->draw_background();

            objectcontainer[0].draw();

            g_map->drawPlatforms(1);

            if(!game_values.swapplayers) {
                for(i = 0; i < list_players_cnt; i++)
                    list_players[i]->draw();
            }

            eyecandy[1].draw();

            objectcontainer[1].draw();

            g_map->drawPlatforms(2);

#ifdef _XBOX
            g_map->drawfrontlayer();
#else
            if(game_values.toplayer)
                g_map->drawfrontlayer();
#endif
            g_map->drawWarpLocks();

            g_map->drawPlatforms(3);

            objectcontainer[2].draw();
            eyecandy[2].draw();
            game_values.gamemode->draw_foreground();

            g_map->drawPlatforms(4);

            if(game_values.spotlights)
                spotlightManager.DrawSpotlights();

            g_iWinningPlayer = -1;

            //Draw scoreboards for all games (except special cases where we have a single player walking the map)
            if(game_values.singleplayermode == -1) {
                short highestScore = 0;

                bool fReverseScoring = game_values.gamemode->GetReverseScoring();
                if(fReverseScoring)
                    highestScore = 32000;

                for(i = 0; i < score_cnt; i++) {
                    int scoreValue = score[i]->score;
                    if(game_values.gamemode->gamemode == game_mode_boxes_minigame)
                        scoreValue = score[i]->subscore[0];

                    if((scoreValue > highestScore && !fReverseScoring) || (scoreValue < highestScore && fReverseScoring)) {
                        highestScore = scoreValue;
                        g_iWinningPlayer = i;
                    } else if(scoreValue == highestScore) {
                        g_iWinningPlayer = -1;
                    }
                }

                //big end game scoreboard (sorted)
                if(game_values.showscoreboard) {
                    char gameovertext[128] = "";
                    if(game_values.gamemode->winningteam > -1) {
                        if(game_values.teamcounts[game_values.gamemode->winningteam] == 1)
                            sprintf(gameovertext, "Player %d Wins!", game_values.teamids[game_values.gamemode->winningteam][0] + 1);
                        else
                            sprintf(gameovertext, "Team %d Wins!", game_values.gamemode->winningteam + 1);
                    } else {
                        sprintf(gameovertext, "Tie Game");
                    }

                    rm->game_font_large.drawCentered(smw->ScreenHeight/2, 90, gameovertext);

                }

                //in game scoreboards
                for(i = 0; i < score_cnt; i++) {
                    if(game_values.gamemode->gamemode == game_mode_health || game_values.gamemode->gamemode == game_mode_collection || game_values.gamemode->gamemode == game_mode_boxes_minigame)
                        rm->spr_shade[game_values.teamcounts[i] - 1].draw(score[i]->x, score[i]->y);
                    else
                        rm->spr_shade[game_values.teamcounts[i] - 1].draw(score[i]->x, score[i]->y, 0, 0, 256, 41);

                    for(short k = 0; k < game_values.teamcounts[i]; k++) {
                        short globalID = game_values.teamids[i][k];

                        //If player is respawning, draw an animated egg counter
                        if(respawn[globalID] > 0 && !game_values.gamemode->gameover) {
                            if(++respawnanimationtimer[globalID] > 8) {
                                respawnanimationtimer[globalID] = 0;
                                respawnanimationframe[globalID] += 32;

                                if(respawnanimationframe[globalID] > 32)
                                    respawnanimationframe[globalID] = 0;
                            }

                            short scorex = score[i]->x + scoreoffsets[k];
                            short scorey = score[i]->y + 2;
                            rm->spr_egg.draw(scorex, scorey, respawnanimationframe[globalID], game_values.colorids[globalID] << 5, 32, 32);
                            rm->spr_eggnumbers.draw(scorex, scorey, ((respawn[globalID] - 1) >> 1) << 5, game_values.colorids[globalID] << 5, 32, 32);
                        } else { //otherwise draw the player's skin in the scoreboard
                            short iScoreboardSprite;
                            if(game_values.gamemode->gameover) {
                                if(g_iWinningPlayer != i)
                                    iScoreboardSprite = PGFX_DEADFLYING;
                                else
                                    iScoreboardSprite = PGFX_JUMPING_R;
                            } else {
                                iScoreboardSprite = PGFX_STANDING_R;
                            }

                            //Search for player state to display
                            CPlayer * player = GetPlayerFromGlobalID(globalID);

                            if(player && !game_values.gamemode->gameover) {
                                short iScoreOffsetX = score[i]->x + scoreoffsets[k];
                                short iScoreOffsetY = score[i]->y + 2;

                                if(player->ownerPlayerID > -1)
                                    rm->spr_ownedtags.draw(iScoreOffsetX - 8, iScoreOffsetY - 8, player->ownerColorOffsetX, 0, 48, 48);

                                player->GetScoreboardSprite()[iScoreboardSprite]->draw(iScoreOffsetX, iScoreOffsetY, player->iSrcOffsetX, 0, 32, 32);

                                //Display jail if player is jailed
                                if(player->jailtimer > 0)
                                    rm->spr_jail.draw(iScoreOffsetX - 6, iScoreOffsetY - 6, (player->jailcolor + 1) * 44, 0, 44, 44);

                                //Display current powerup if player is using one
                                if(player->powerup > 0)
                                    rm->spr_storedpowerupsmall.draw(iScoreOffsetX, iScoreOffsetY + 16, g_iPowerupToIcon[player->powerup - 1], 0, 16, 16);

                                //Display tanooki powerup if player has it
                                if(player->tanooki)
                                    rm->spr_storedpowerupsmall.draw(iScoreOffsetX + 16, iScoreOffsetY + 16, 320, 0, 16, 16);
                            } else {
                                rm->spr_player[globalID][iScoreboardSprite]->draw(score[i]->x + scoreoffsets[k], score[i]->y + 2, 0, 0, 32, 32);
                            }

                            //give crown to player(s) with most kills
                            if(g_iWinningPlayer == i)
                                rm->spr_crown.draw(score[i]->x + scoreoffsets[k] + 12, score[i]->y - 4);
                        }

                        short storedpowerupid = game_values.gamepowerups[globalID];

                        //Draw stored powerup
                        if(storedpowerupid != -1) {
                            if(!game_values.swapplayers) {
                                rm->spr_storedpowerupsmall.draw(score[i]->x + scorepowerupoffsets[game_values.teamcounts[i] - 1][k], score[i]->y + 25, storedpowerupid * 16, 0, 16, 16);
                            }
                        }
                    }

                    //Draw hearts for health mode
                    if(game_values.gamemode->gamemode == game_mode_health) {
                        short iLife = score[i]->subscore[0];
                        short iMax = score[i]->subscore[1];
                        short iHeartX = score[i]->x + scorepowerupoffsets[game_values.teamcounts[i] - 1][0] - 32;

                        for(short iHeart = 0; iHeart < iLife; iHeart++) {
                            if(iHeart == iMax - 1 && iHeart % 2 == 0)
                                rm->spr_scorehearts.draw(iHeartX + iHeart * 8, score[i]->y + 43, 32, 0, 8, 16);
                            else
                                rm->spr_scorehearts.draw(iHeartX + iHeart * 8, score[i]->y + 43, iHeart % 2 ? 8 : 0, 0, 8, 16);
                        }

                        for(short iHeart = iLife; iHeart < iMax; iHeart++) {
                            if(iHeart == iMax - 1 && iHeart % 2 == 0)
                                rm->spr_scorehearts.draw(iHeartX + iHeart * 8, score[i]->y + 43, 40, 0, 8, 16);
                            else
                                rm->spr_scorehearts.draw(iHeartX + iHeart * 8, score[i]->y + 43, iHeart % 2 ? 24 : 16, 0, 8, 16);
                        }
                    } else if(game_values.gamemode->gamemode == game_mode_collection) { //Draw cards for collection mode
                        //Flash collected cards if 3 have been collected
                        if(score[i]->subscore[0] < 3 || score[i]->subscore[2] % 20 < 10) {
                            short iNumCards = score[i]->subscore[0];
                            short iCardValues = score[i]->subscore[1];
                            short iCardX = score[i]->x + scorepowerupoffsets[game_values.teamcounts[i] - 1][0] - 20;

                            for(short iCard = 0; iCard < iNumCards; iCard++) {
                                rm->spr_scorecards.draw(iCardX + iCard * 20, score[i]->y + 43, (iCardValues & 3) << 4, 0, 16, 16);
                                iCardValues >>= 2;
                            }
                        }
                    } else if(game_values.gamemode->gamemode == game_mode_boxes_minigame) { //Draw coins for boxes minigame
                        //Flash collected cards if 3 have been collected
                        short iNumCoins = score[i]->subscore[0];
                        short iCoinX = score[i]->x + scorepowerupoffsets[game_values.teamcounts[i] - 1][0] - 32;

                        short iCoin = 0;
                        for(; iCoin < iNumCoins; iCoin++) {
                            rm->spr_scorecoins.draw(iCoinX + iCoin * 16, score[i]->y + 43, 0, 0, 16, 16);
                        }

                        for(short iEmptyCoin = iCoin; iEmptyCoin < 5; iEmptyCoin++) {
                            rm->spr_scorecoins.draw(iCoinX + iEmptyCoin * 16, score[i]->y + 43, 16, 0, 16, 16);
                        }
                    }

                    short iScoreX = score[i]->x + iScoreTextOffset[i];
                    short iScoreY = score[i]->y + 4;

                    rm->spr_scoretext.draw(iScoreX, iScoreY, score[i]->iDigitLeft, (score[i]->iDigitLeft == 0 ? 16 : 0), 16, 16);
                    rm->spr_scoretext.draw(iScoreX + 18, iScoreY, score[i]->iDigitMiddle, (score[i]->iDigitLeft == 0 && score[i]->iDigitMiddle == 0 ? 16 : 0), 16, 16);
                    rm->spr_scoretext.draw(iScoreX + 36, iScoreY, score[i]->iDigitRight, 0, 16, 16);
                }
            }

            if(game_values.windaffectsplayers) {
                short iDisplayWindMeterY = game_values.scoreboardstyle == 1 ? 8 : 440;
                rm->spr_windmeter.draw(210, iDisplayWindMeterY, 0, 0, 220, 32);
                rm->spr_windmeter.draw((short)(game_values.gamewindx * 20.0f) + 320, iDisplayWindMeterY + 6, 220, 0, 12, 20);
            }

            //draw arrows for being above the top of the screen
            for(i = 0; i < list_players_cnt; i++)
                list_players[i]->drawarrows();

            //Draw countdown start timer
            if(iCountDownState > 0 && game_values.screenfade == 0) {
                SDL_Rect * rects = iCountDownNumbers[iCountDownRectGroup[28 - iCountDownState]][iCountDownRectSize[28 - iCountDownState]];
                rm->spr_countdown_numbers.draw(rects[1].x, rects[1].y, rects[0].x, rects[0].y, rects[0].w, rects[0].h);
            }

            if(game_values.screenfadespeed != 0) {
                g_map->update();
                game_values.screenfade += game_values.screenfadespeed;

                if(game_values.screenfade <= 0) {
                    game_values.screenfadespeed = 0;
                    game_values.screenfade = 0;

                    //display the mode and goal at the start of the game
                    //if(game_values.matchtype == MATCH_TYPE_QUICK_GAME)
                    if(game_values.startmodedisplay && game_values.singleplayermode == -1) {
                        char szMode[128];
                        if(game_values.gamemode->goal < 0)
                            sprintf(szMode, "%s  %s: X", game_values.gamemode->GetModeName(), game_values.gamemode->GetGoalName());
                        else
                            sprintf(szMode, "%s  %s: %d", game_values.gamemode->GetModeName(), game_values.gamemode->GetGoalName(), game_values.gamemode->goal);

                        short iMode = GetModeIconIndexFromMode(game_values.gamemode->gamemode);

                        eyecandy[2].add(new EC_Announcement(&rm->game_font_large, &rm->menu_mode_large, szMode, iMode, 130, 90));
                    }
                } else if(game_values.screenfade >= 255) {
                    game_values.screenfadespeed = 0;
                    game_values.screenfade = 255;
                }
            }

            if(game_values.screenfade > 0) {
                rm->menu_shade.setalpha((Uint8)game_values.screenfade);
                rm->menu_shade.draw(0, 0);
            }

            if(game_values.swapplayers) {
                for(i = 0; i < list_players_cnt; i++) {
                    list_players[i]->drawswap();

                    short storedpowerupid = game_values.gamepowerups[list_players[i]->getGlobalID()];

                    if(storedpowerupid != -1) {
                        short iPowerupX, iPowerupY;

                        if(game_values.swapstyle == 1) {
                            iPowerupX = game_values.swapplayersblink ? list_players[i]->iOldPowerupX : list_players[i]->iNewPowerupX;
                            iPowerupY = game_values.swapplayersblink ? list_players[i]->iOldPowerupY : list_players[i]->iNewPowerupY;
                        } else {
                            iPowerupX = (short)((float)(list_players[i]->iNewPowerupX - list_players[i]->iOldPowerupX) * game_values.swapplayersposition) + list_players[i]->iOldPowerupX;
                            iPowerupY = (short)((float)(list_players[i]->iNewPowerupY - list_players[i]->iOldPowerupY) * game_values.swapplayersposition) + list_players[i]->iOldPowerupY;
                        }

                        rm->spr_storedpowerupsmall.draw(iPowerupX, iPowerupY, storedpowerupid * 16, 0, 16, 16);
                    }
                }

                if(game_values.swapstyle == 0) {
                    if(!sfx_skid.isplaying())
                        ifsoundonplay(sfx_skid);
                }

                if(++game_values.swapplayersblinkcount > 10) {
                    game_values.swapplayersblinkcount = 0;
                    game_values.swapplayersblink = !game_values.swapplayersblink;
                }

                game_values.swapplayersposition += 0.02f;
                if(game_values.swapplayersposition >= 1.0f) {
                    game_values.swapplayersposition = 0.0f;
                    game_values.swapplayers = false;
                    game_values.screenfade = 0;

                    if(game_values.swapstyle == 0)
                        ifsoundonstop(sfx_skid);

                    ifsoundonplay(sfx_transform);

                    if(game_values.swapstyle == 1) {
                        for(i = 0; i < list_players_cnt; i++)
                            eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, list_players[i]->ix + (HALFPW) - 16, list_players[i]->iy + (HALFPH) - 16, 3, 8));
                    }
                }
            }

            //Draw black "behind" the game if we are shaking/moving the screen
            if(y_shake > 0) {
				SDL_Rect rect = {0, 0, smw->ScreenWidth, y_shake};
                SDL_FillRect(screen, &rect, 0x0);		//fill empty area with black
            } else if(y_shake < 0) {
				SDL_Rect rect = {0, smw->ScreenHeight + y_shake, smw->ScreenWidth, smw->ScreenHeight};
                SDL_FillRect(screen, &rect, 0x0);		//fill empty area with black
            }

            if(x_shake > 0) {
				SDL_Rect rect = {0, 0, x_shake, smw->ScreenHeight};
                SDL_FillRect(screen, &rect, 0x0);		//fill empty area with black
            } else if(x_shake < 0) {
				SDL_Rect rect = {smw->ScreenWidth + x_shake, 0, smw->ScreenWidth, smw->ScreenHeight};
                SDL_FillRect(screen, &rect, 0x0);		//fill empty area with black
            }
        } else {
            if(game_values.pausegame) {
                rm->spr_dialog.draw(224, 176);
				rm->menu_font_large.drawCentered(smw->ScreenHeight/2, 194, "Pause");

                //menu_font_large.drawCentered(320, 240, game_values.gamemode->GetModeName());
                short iMode = GetModeIconIndexFromMode(game_values.gamemode->gamemode);

                rm->menu_mode_large.draw(304, 224, iMode << 5, 0, 32, 32);

                char szGoal[256];
                strcpy(szGoal, game_values.gamemode->GetGoalName());
                strcat(szGoal, ": ");

                if(game_values.gamemode->goal == -1)
                    sprintf(szGoal + strlen(szGoal), "Unlimited");
                else
                    sprintf(szGoal + strlen(szGoal), "%d", game_values.gamemode->goal);

				rm->menu_font_large.drawCentered(smw->ScreenWidth/2, 264, szGoal);
            }

            if(game_values.exitinggame) {
                rm->spr_dialog.draw(224, 176);
                rm->menu_font_large.drawCentered(smw->ScreenWidth/2, 220 - (rm->menu_font_large.getHeight() >> 1), "Exit Game");

                rm->spr_dialogbutton.draw(235, 250, 0, (game_values.exityes ? 34 : 0), 80, 34);
                rm->spr_dialogbutton.draw(325, 250, 0, (game_values.exityes ? 0 : 34), 80, 34);

                rm->menu_font_large.draw(275 - (rm->menu_font_large.getWidth("Yes") >> 1),  269 - (rm->menu_font_large.getHeight() >> 1), "Yes");
                rm->menu_font_large.draw(365 - (rm->menu_font_large.getWidth("No") >> 1),  269 - (rm->menu_font_large.getHeight() >> 1), "No");
            }
        }

        //Make sure music and sound effects keep playing
        if(game_values.slowdownon != -1) {
            if(!sfx_slowdownmusic.isplaying())
                ifsoundonplay(sfx_slowdownmusic);
        } else {
            if(sfx_slowdownmusic.isplaying())
                ifsoundonstop(sfx_slowdownmusic);
        }

        if(game_values.playinvinciblesound) {
            if(!sfx_invinciblemusic.isplaying() && !sfx_timewarning.isplaying() && !backgroundmusic[0].isplaying())
                ifsoundonplay(sfx_invinciblemusic);
        } else {
            if(sfx_invinciblemusic.isplaying())
                ifsoundonstop(sfx_invinciblemusic);
        }

        //If no background music is playing, then play some
        if(!backgroundmusic[0].isplaying() && !sfx_invinciblemusic.isplaying() && !sfx_timewarning.isplaying() && !game_values.gamemode->gameover) {
            if(game_values.playnextmusic) {
                musiclist->SetNextMusic(g_map->musicCategoryID, maplist->currentShortmapname(), g_map->szBackgroundFile);
                backgroundmusic[0].load(musiclist->GetCurrentMusic());
            }

            backgroundmusic[0].play(game_values.playnextmusic, false);
        }

        ticks = SDL_GetTicks() - framestart;
        if(ticks == 0)
            ticks = 1;

        if(game_values.showfps) {
            float potentialFps = 1000.0f / (float)(game_values.framelimiter == 0 ? 1 : game_values.framelimiter);
            rm->menu_font_large.drawf(0, smw->ScreenHeight-rm->menu_font_large.getHeight(), "Actual:%.1f/%.1f, Flip:%.1f, Potential:%.1f", realfps, potentialFps, flipfps, 1000.0f / (float)ticks);
        }

#ifdef _DEBUG
        if(g_fAutoTest)
            rm->menu_font_small.drawRightJustified(635, 5, "Auto");
#endif

        //double buffering -> flip buffers
        SDL_Flip(screen);

        flipfps = 1000.0f / (float)ticks;


        //Sleep for time just under what we need
        short delay = (short)(game_values.framelimiter - SDL_GetTicks() + framestart - 2);

        if(delay > 0) {
            if(delay > game_values.framelimiter)
                delay = game_values.framelimiter;

            SDL_Delay(delay);
        }

        //Fine tune wait here
        while(SDL_GetTicks() - framestart < (unsigned short)game_values.framelimiter)
            SDL_Delay(0);   //keep framerate constant at 1000/game_values.framelimiter fps

        //Debug code to slow framerate down to 1 fps to see exact movement
#ifdef _DEBUG
        if(game_values.frameadvance) {
            delay = (short)(1000 - SDL_GetTicks() + framestart);

            if(delay > 0) {
                if(delay > 1000)
                    delay = 1000;

                SDL_Delay(delay);
            }

            while(SDL_GetTicks() - framestart < 1000)
                SDL_Delay(0);
        }
#endif

        ticks = SDL_GetTicks() - framestart;
        if(ticks == 0)
            ticks = game_values.framelimiter;

        realfps = 1000.0f / (float)ticks;

        /*
        static float avgFPS = 0.0f;
        static short outputtimer = 0;

        avgFPS += realfps;

        if(++outputtimer == 1)
        {
            FILE * out = fopen("fps.txt", "a+");

            fprintf(out, "%.2f\n", avgFPS / (float)outputtimer);

            fclose(out);

            avgFPS = 0.0f;
            outputtimer = 0;
        }*/
    }

    //we never get here

    return;
}

void CleanUp()
{
    short i;
    //delete object list
    for(i = 0; i < list_players_cnt; i++) {
        delete list_players[i];
    }
    list_players_cnt = 0;

    eyecandy[0].clean();
    eyecandy[1].clean();
    eyecandy[2].clean();
    spotlightManager.ClearSpotlights();

    noncolcontainer.clean();

    objectcontainer[0].clean();
    objectcontainer[1].clean();
    objectcontainer[2].clean();

    LoadMapObjects(true);
    g_map->clearWarpLocks();
    g_map->resetPlatforms();

    //Stop all game sounds
    sfx_stopallsounds();
    sfx_invinciblemusic.resetpause();
    sfx_slowdownmusic.resetpause();

    x_shake = 0;
    y_shake = 0;
}

void UpdateScoreBoard()
{
    if(game_values.matchtype == MATCH_TYPE_WORLD) {
        //If no one won, then nothing on the world map has changed
        if(game_values.gamemode->winningteam < 0)
            return;

        //If this was the last stage, signal that the world is over
        if(game_values.tourstops[game_values.tourstopcurrent]->fEndStage) {
            game_values.tournamentwinner = 1;
            backgroundmusic[4].play(true, true);
        }

        //Add up all the winnings so far and determine overall place in the standings
        for(short iScore = 0; iScore < score_cnt; iScore++)
            game_values.tournament_scores[iScore].wins = 0;

        //Assign the order that players will show up on the scoreboard (ordered by score)
        for(short iMyScore = 0; iMyScore < score_cnt; iMyScore++) {
            for(short iTheirScore = 0; iTheirScore < score_cnt; iTheirScore++) {
                if(game_values.tournament_scores[iMyScore].total > game_values.tournament_scores[iTheirScore].total) {
                    game_values.tournament_scores[iTheirScore].wins++;
                }
            }
        }

        //Give players the item bonuses that were won
        TourStop * tourStop = game_values.tourstops[game_values.tourstopcurrent];

        for(short iScore = 0; iScore < score_cnt; iScore++) {
            for(short iBonus = 0; iBonus < tourStop->iNumBonuses; iBonus++) {
                if(tourStop->wsbBonuses[iBonus].iWinnerPlace == score[iScore]->place) {
                    if(game_values.worldpowerupcount[iScore] < 32)
                        game_values.worldpowerups[iScore][game_values.worldpowerupcount[iScore]++] = tourStop->wsbBonuses[iBonus].iBonus;
                    else
                        game_values.worldpowerups[iScore][31] = tourStop->wsbBonuses[iBonus].iBonus;
                }
            }
        }

        //Add powerups to player's world item inventory that they held at the end of the game
        for(short iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++) {
            if(game_values.gamepowerups[iPlayer] != -1) {
                short iTeamId = LookupTeamID(iPlayer);
                if(game_values.worldpowerupcount[iTeamId] < 32)
                    game_values.worldpowerups[iTeamId][game_values.worldpowerupcount[iTeamId]++] = game_values.gamepowerups[iPlayer];
            }
        }
    } else if(game_values.matchtype == MATCH_TYPE_TOUR) {
        //If no one won (tied game), then there is no need to update the scores because nothing has changed
        if(game_values.gamemode->winningteam < 0)
            return;

        //For this game, set the player's place as the type of win
        for(short iScore = 0; iScore < score_cnt; iScore++) {
            game_values.tournament_scores[iScore].type[game_values.tourstopcurrent] = score[iScore]->place;
        }

        //Add up all the winnings so far and determine overall place in tour
        for(short iScore = 0; iScore < score_cnt; iScore++) {
            game_values.tournament_scores[iScore].total = 0;

            for(short iTourStop = 0; iTourStop <= game_values.tourstopcurrent; iTourStop++) {
                game_values.tournament_scores[iScore].total += (3 - game_values.tournament_scores[iScore].type[iTourStop]) * game_values.tourstops[iTourStop]->iPoints;
            }

            game_values.tournament_scores[iScore].wins = 0;
        }

        for(short iMyScore = 0; iMyScore < score_cnt; iMyScore++) {
            for(short iTheirScore = 0; iTheirScore < score_cnt; iTheirScore++) {
                if(game_values.tournament_scores[iMyScore].total > game_values.tournament_scores[iTheirScore].total) {
                    game_values.tournament_scores[iTheirScore].wins++;
                }
            }
        }

        if(++game_values.tourstopcurrent >= game_values.tourstoptotal) {
            //Calculate Tour Winner by counting up 1st, 2nd, 3rd, and 4th place wins

            short iWinningTeam = -2;  //Set winning team to -2 to signify a tie between teams
            short iWinningScore = 0;

            for(short iScore = 0; iScore < score_cnt; iScore++) {
                if(game_values.tournament_scores[iScore].total > iWinningScore) { //New winner found
                    iWinningTeam = iScore;
                    iWinningScore = game_values.tournament_scores[iScore].total;
                } else if(game_values.tournament_scores[iScore].total == iWinningScore) { //Winning position tied
                    iWinningTeam = -2;
                }
            }

            game_values.tournamentwinner = iWinningTeam;
            backgroundmusic[4].play(true, true);
        }
    } else if(game_values.matchtype == MATCH_TYPE_TOURNAMENT) {
        short maxScore = -1;  //Max score for game
        short maxTeam = -1;  //Team ID with the max score -> reset to -1 if two teams tied for win

        for(short i = 0; i < score_cnt; i++) {
            if(score[i]->score > maxScore) {
                maxScore = score[i]->score;
                maxTeam = i;
            } else if(score[i]->score == maxScore) {
                maxTeam = -1;
            }
        }

        if(maxTeam > -1) {
            game_values.tournament_scores[maxTeam].type[game_values.tournament_scores[maxTeam].wins] = currentgamemode;

            if(++game_values.tournament_scores[maxTeam].wins >= game_values.tournamentgames) {
                game_values.tournamentwinner = maxTeam;

                if(game_values.music)
                    backgroundmusic[4].play(true, true);
            }
        }
    }
}

void PlayNextMusicTrack()
{
    if(game_values.gamemode->gameover || game_values.playinvinciblesound || sfx_timewarning.isplaying())
        return;

    backgroundmusic[0].stop();
    musiclist->SetNextMusic(g_map->musicCategoryID, maplist->currentShortmapname(), g_map->szBackgroundFile);
    backgroundmusic[0].load(musiclist->GetCurrentMusic());
    backgroundmusic[0].play(game_values.playnextmusic, false);
}

bool coldec_player2player(CPlayer * o1, CPlayer * o2)
{
    //Special cases to deal with players overlapping the right and left sides of the screen
    if(o1->ix + PW < o2->ix) {
		return o1->ix + smw->ScreenWidth < o2->ix + PW && o1->ix + PW + smw->ScreenWidth >= o2->ix && o1->iy <= o2->iy + PH && o1->iy + PH >= o2->iy;
    } else if(o2->ix + PW < o1->ix) {
		return o1->ix < o2->ix + PW + smw->ScreenWidth && o1->ix + PW >= o2->ix + smw->ScreenWidth && o1->iy <= o2->iy + PH && o1->iy + PH >= o2->iy;
    } else { //Normal case where no overlap
        return o1->ix < o2->ix + PW && o1->ix + PW >= o2->ix && o1->iy <= o2->iy + PH && o1->iy + PH >= o2->iy;
    }
}

bool coldec_player2obj(CPlayer * o1, CObject * o2)
{
    //Special cases to deal with players overlapping the right and left sides of the screen
    if(o1->ix + PW < o2->ix) {
		return o1->ix + smw->ScreenWidth < o2->ix + o2->collisionWidth && o1->ix + PW + smw->ScreenWidth >= o2->ix && o1->iy < o2->iy + o2->collisionHeight && o1->iy + PH >= o2->iy;
    } else if(o2->ix + o2->collisionWidth < o1->ix) {
		return o1->ix < o2->ix + o2->collisionWidth + smw->ScreenWidth && o1->ix + PW >= o2->ix + smw->ScreenWidth && o1->iy < o2->iy + o2->collisionHeight && o1->iy + PH >= o2->iy;
    } else { //Normal case where no overlap
        return o1->ix < o2->ix + o2->collisionWidth && o1->ix + PW >= o2->ix && o1->iy < o2->iy + o2->collisionHeight && o2->iy <= o1->iy + PH;
    }
}

bool coldec_obj2obj(CObject * o1, CObject * o2)
{
    //Special cases to deal with players overlapping the right and left sides of the screen
    short o1r = o1->ix + o1->collisionWidth;
    short o1b = o1->iy + o1->collisionHeight;
    short o2r = o2->ix + o2->collisionWidth;
    short o2b = o2->iy + o2->collisionHeight;

    if(o1r < o2->ix) {
		return o1->ix + smw->ScreenWidth < o2r && o1r + smw->ScreenWidth >= o2->ix && o1->iy < o2b && o1b >= o2->iy;
    } else if(o2r < o1->ix) {
		return o1->ix < o2r + smw->ScreenWidth && o1r >= o2->ix + smw->ScreenWidth && o1->iy < o2b && o1b >= o2->iy;
    } else {
        return o1->ix < o2r && o1r >= o2->ix && o1->iy < o2b && o1b >= o2->iy;
    }
}

void SetGameModeSettingsFromMenu()
{
    //If this is a tour stop and the tour has settings in it, use those.  Otherwise use the menu settings.
    if(game_values.tourstops[game_values.tourstopcurrent]->fUseSettings &&
            (game_values.matchtype == MATCH_TYPE_TOUR || game_values.matchtype == MATCH_TYPE_WORLD))
        memcpy(&game_values.gamemodesettings, &game_values.tourstops[game_values.tourstopcurrent]->gmsSettings, sizeof(GameModeSettings));
    else
        memcpy(&game_values.gamemodesettings, &game_values.gamemodemenusettings, sizeof(GameModeSettings));
}

void LoadMapObjects(bool fPreview)
{
    LoadMapHazards(fPreview);

    //Clear all the previous switch settings
    for(short iSwitch = 0; iSwitch < 8; iSwitch++)
        g_map->switchBlocks[iSwitch].clear();

    //Add blocks (breakable, note, switch, throwable, etc)
    for(short x = 0; x < MAPWIDTH; x++) {
        for(short y = 0; y < MAPHEIGHT; y++) {
            short iType = g_map->objectdata[x][y].iType;
            if(iType == 0) {
                g_map->blockdata[x][y] = new B_BreakableBlock(&rm->spr_breakableblock, x << 5, y << 5, 4, 10);
                noncolcontainer.add(g_map->blockdata[x][y]);
            } else if(iType == 1) {
                g_map->blockdata[x][y] = new B_PowerupBlock(&rm->spr_powerupblock, x << 5, y << 5, 4, 10, g_map->objectdata[x][y].fHidden, g_map->objectdata[x][y].iSettings);
                noncolcontainer.add(g_map->blockdata[x][y]);
            } else if(iType == 2) {
                g_map->blockdata[x][y] = new B_DonutBlock(&rm->spr_donutblock, x << 5, y << 5);
                noncolcontainer.add(g_map->blockdata[x][y]);
            } else if(iType == 3) {
                g_map->blockdata[x][y] = new B_FlipBlock(&rm->spr_flipblock, x << 5, y << 5, g_map->objectdata[x][y].fHidden);
                noncolcontainer.add(g_map->blockdata[x][y]);
            } else if(iType == 4) {
                g_map->blockdata[x][y] = new B_BounceBlock(&rm->spr_bounceblock, x << 5, y << 5, g_map->objectdata[x][y].fHidden);
                noncolcontainer.add(g_map->blockdata[x][y]);
            } else if(iType == 5) {
                g_map->blockdata[x][y] = new B_NoteBlock(&rm->spr_noteblock, x << 5, y << 5, 4, 10, 1, g_map->objectdata[x][y].fHidden);
                noncolcontainer.add(g_map->blockdata[x][y]);
            } else if(iType == 6) {
                g_map->blockdata[x][y] = new B_ThrowBlock(&rm->spr_throwblock, x << 5, y << 5, 4, 10, 0);
                noncolcontainer.add(g_map->blockdata[x][y]);
            } else if(iType >= 7 && iType <= 10) {
                short iSwitchType = iType - 7;
                g_map->blockdata[x][y] = new B_OnOffSwitchBlock(&rm->spr_switchblocks, x << 5, y << 5, iSwitchType, g_map->iSwitches[iSwitchType]);
                noncolcontainer.add(g_map->blockdata[x][y]);
                g_map->switchBlocks[iSwitchType].push_back(g_map->blockdata[x][y]);
            } else if(iType >= 11 && iType <= 14) {
                short iSwitchType = iType - 11;

                //g_map->blockdata[x][y] = new B_SwitchBlock(&rm->spr_switchblocks, x << 5, y << 5, iSwitchType, g_map->iSwitches[iSwitchType]);
                g_map->blockdata[x][y] = new B_SwitchBlock(&rm->spr_switchblocks, x << 5, y << 5, iSwitchType, g_map->objectdata[x][y].iSettings[0]);
                noncolcontainer.add(g_map->blockdata[x][y]);
                g_map->switchBlocks[iSwitchType + 4].push_back(g_map->blockdata[x][y]);
            } else if(iType == 15) {
                g_map->blockdata[x][y] = new B_ViewBlock(&rm->spr_viewblock, x << 5, y << 5, g_map->objectdata[x][y].fHidden, g_map->objectdata[x][y].iSettings);
                noncolcontainer.add(g_map->blockdata[x][y]);
            } else if(iType == 16) {
                g_map->blockdata[x][y] = new B_ThrowBlock(&rm->spr_throwblock, x << 5, y << 5, 4, 10, 2);
                noncolcontainer.add(g_map->blockdata[x][y]);
            } else if(iType == 17 || iType == 18) {
                g_map->blockdata[x][y] = new B_NoteBlock(&rm->spr_noteblock, x << 5, y << 5, 4, 10, iType == 17 ? 2 : 0, g_map->objectdata[x][y].fHidden);
                noncolcontainer.add(g_map->blockdata[x][y]);
            } else if(iType == 19) {
                g_map->blockdata[x][y] = new B_ThrowBlock(&rm->spr_throwblock, x << 5, y << 5, 4, 10, 1);
                noncolcontainer.add(g_map->blockdata[x][y]);
            } else if(iType >= 20 && iType <= 29) {
                g_map->blockdata[x][y] = new B_WeaponBreakableBlock(&rm->spr_weaponbreakableblock, x << 5, y << 5, iType - 20);
                noncolcontainer.add(g_map->blockdata[x][y]);
            } else {
                g_map->blockdata[x][y] = NULL;
            }
        }
    }

    //Scan for throw box objects and add items to them if approprate
    //Add special coins to them for the boxes minigame
    short iCountWeight = 0;
    for(short iPowerup = 0; iPowerup < NUM_POWERUPS; iPowerup++)
        iCountWeight += game_values.powerupweights[iPowerup];

    short iThrowBoxCount = 0;
    bool * fBoxHasCoin = NULL;

    if(game_values.gamemode->gamemode == game_mode_boxes_minigame) {
        for(short iItem = 0; iItem < g_map->iNumMapItems; iItem++) {
            if(g_map->mapitems[iItem].itype == 5)
                iThrowBoxCount++;
        }

        fBoxHasCoin = new bool[iThrowBoxCount];
        for(short iItem = 0; iItem < iThrowBoxCount; iItem++)
            fBoxHasCoin[iItem] = false;

        //Randomly choose boxes to put 5 coins in
        for(short iItem = 0; iItem < 5 && iItem < iThrowBoxCount; iItem++) {
            short iBoxIndex = smw->rng->GetRandMax(iThrowBoxCount);

            while(fBoxHasCoin[iBoxIndex]) {
                if(++iBoxIndex >= iThrowBoxCount)
                    iBoxIndex = 0;
            }

            fBoxHasCoin[iBoxIndex] = true;
        }

        //If map has less than 5 boxes, then insert coins into map in random locations
        short iExtraCoinsNeeded = 5 - iThrowBoxCount;
        for(short iExtraCoin = 0; iExtraCoin < iExtraCoinsNeeded; iExtraCoin++) {
            objectcontainer[1].add(new MO_Coin(&rm->spr_coin, 0.0f, 0.0f, 0, 0, 2, -1, 2, 0, true));
        }
    }

    //Add map objects like springs, shoes and spikes
    short iAddThrowBoxIndex = 0;
    for(short i = 0; i < g_map->iNumMapItems; i++) {
        MapItem * mapItem = &g_map->mapitems[i];
        short iType = mapItem->itype;
        short ix = mapItem->ix << 5;
        short iy = mapItem->iy << 5;

        if(iType == 0)
            objectcontainer[1].add(new CO_Spring(&rm->spr_spring, ix, iy, false));
        else if(iType == 1)
            objectcontainer[1].add(new CO_Spike(&rm->spr_spike, ix, iy));
        else if(iType == 2)
            objectcontainer[1].add(new CO_KuriboShoe(&rm->spr_kuriboshoe, ix, iy, false));
        else if(iType == 3)
            objectcontainer[1].add(new CO_Spring(&rm->spr_spring, ix, iy, true));
        else if(iType == 4)
            objectcontainer[1].add(new CO_KuriboShoe(&rm->spr_kuriboshoe, ix, iy, true));
        else if(iType == 5) {
            short iItem = NO_POWERUP;
            if(!fPreview) {
                if(game_values.gamemode->gamemode == game_mode_boxes_minigame) {
                    if(fBoxHasCoin[iAddThrowBoxIndex])
                        iItem = MINIGAME_COIN;
                } else if(game_values.gamemode->gamemode == game_mode_health && smw->rng->GetRandMax(100) < game_values.gamemodesettings.health.percentextralife) {
                    iItem = HEALTH_POWERUP;
                } else if((game_values.gamemode->gamemode == game_mode_timelimit && smw->rng->GetRandMax(100) < game_values.gamemodesettings.time.percentextratime) ||
                          (game_values.gamemode->gamemode == game_mode_star && smw->rng->GetRandMax(100) < game_values.gamemodesettings.star.percentextratime)) {
                    iItem = TIME_POWERUP;
                } else if((game_values.gamemode->gamemode == game_mode_coins && smw->rng->GetRandMax(100) < game_values.gamemodesettings.coins.percentextracoin) ||
                          (game_values.gamemode->gamemode == game_mode_greed && smw->rng->GetRandMax(100) < game_values.gamemodesettings.greed.percentextracoin)) {
                    iItem = COIN_POWERUP;
                } else if(game_values.gamemode->gamemode == game_mode_jail && (smw->rng->GetRandMax(100)) < game_values.gamemodesettings.jail.percentkey) {
                    iItem = JAIL_KEY_POWERUP;
                } else if(iCountWeight > 0 && (smw->rng->GetRandMax(100)) < 40) {
                    int iRandPowerup = smw->rng->GetRandMax( iCountWeight )+ 1;
                    iItem = 0;

                    int iPowerupWeightCount = game_values.powerupweights[iItem];

                    while(iPowerupWeightCount < iRandPowerup)
                        iPowerupWeightCount += game_values.powerupweights[++iItem];
                }
            }

            objectcontainer[1].add(new CO_ThrowBox(&rm->spr_throwbox, ix, iy, iItem));
            iAddThrowBoxIndex++;
        }
    }

    if(fBoxHasCoin) {
        delete [] fBoxHasCoin;
        fBoxHasCoin = NULL;
    }

    //Set all the 1x1 gaps up so players can run across them
    g_map->UpdateAllTileGaps();
}

bool SwapPlayers(short iUsingPlayerID)
{
    //Count available players to switch with
    short iNumAvailablePlayers = 0;
    for(short iPlayer = 0; iPlayer < list_players_cnt; iPlayer++) {
        if(list_players[iPlayer]->isready()) {
            iNumAvailablePlayers++;
            list_players[iPlayer]->fOldSwapX = list_players[iPlayer]->ix;
            list_players[iPlayer]->fOldSwapY = list_players[iPlayer]->iy;

            list_players[iPlayer]->iNewPowerupX = list_players[iPlayer]->score->x + scorepowerupoffsets[game_values.teamcounts[list_players[iPlayer]->teamID] - 1][list_players[iPlayer]->subTeamID];
            list_players[iPlayer]->iNewPowerupY = list_players[iPlayer]->score->y + 25;
        }
    }

    if(iNumAvailablePlayers <= 1)
        return false;

    if(game_values.swapstyle != 2) {
        game_values.swapplayers = true;
        game_values.swapplayersposition = 0.0f;

        if(game_values.swapstyle == 1) {
            game_values.swapplayersblink = false;
            game_values.swapplayersblinkcount = 0;
        } else {
            game_values.screenfade = SCREEN_BACKGROUND_FADE	;
        }
    }

    short iIncrement = smw->rng->GetRandMax (iNumAvailablePlayers - 1);

    MysteryMushroomTempPlayer spots[4];
    for(short iPlayer = 0; iPlayer < list_players_cnt; iPlayer++) {
        if(!list_players[iPlayer]->isready())
            continue;

        short iNewSpot = iPlayer + iIncrement;

        do {
            if(++iNewSpot >= list_players_cnt)
                iNewSpot = 0;
        } while(spots[iNewSpot].fUsed || !list_players[iNewSpot]->isready());

        spots[iNewSpot].fUsed = true;
        spots[iPlayer].SetPlayer(list_players[iNewSpot], game_values.gamepowerups[list_players[iNewSpot]->getGlobalID()]);

        //Give credit for deaths to the player that used the mystery mushroom
        if(iUsingPlayerID == iNewSpot) {
            list_players[iPlayer]->iSuicideCreditPlayerID = list_players[iNewSpot]->globalID;
            list_players[iPlayer]->iSuicideCreditTimer = 62;
        }
    }

    for(short iPlayer = 0; iPlayer < list_players_cnt; iPlayer++) {
        if(!list_players[iPlayer]->isready())
            continue;

        spots[iPlayer].GetPlayer(list_players[iPlayer], &game_values.gamepowerups[list_players[iPlayer]->getGlobalID()]);

        if(game_values.swapstyle != 1)
            eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, (short)list_players[iPlayer]->fNewSwapX + (HALFPW) - 16, (short)list_players[iPlayer]->fNewSwapY + (HALFPH) - 16, 3, 8));

        if(game_values.swapstyle == 2) {
            list_players[iPlayer]->setXf(list_players[iPlayer]->fNewSwapX);
            list_players[iPlayer]->setYf(list_players[iPlayer]->fNewSwapY);

            if(list_players[iPlayer]->carriedItem)
                list_players[iPlayer]->carriedItem->MoveToOwner();
        }
    }

    //Clean the dead mystery mushroom
    objectcontainer[0].cleandeadobjects();

    return true;
}

bool IsExitAllowed()
{
    if(!game_values.noexit || list_players_cnt == 0)
        return true;

    for(int iPlayer = 0; iPlayer < list_players_cnt; iPlayer++) {
        if(game_values.playercontrol[list_players[iPlayer]->getGlobalID()] == 1)
            return false;
    }

    return true;
}

bool IsPauseAllowed()
{
    return !game_values.noexit;
}

void UpdateMusicWithOverrides()
{
    FILE * file = fopen(convertPath("music/Overrides.txt").c_str(), "r");

    if(!file)
        return;

    short iAddToCategory = 0;
    char szBuffer[256];
    while(fgets(szBuffer, 1024, file)) {
        //Ignore comment lines
        if(szBuffer[0] == '#' || szBuffer[0] == '\n' || szBuffer[0] == '\r' || szBuffer[0] == ' ' || szBuffer[0] == '\t')
            continue;

        //Chop off line ending
        int stringLength = strlen(szBuffer);
        for(short k = 0; k < stringLength; k++) {
            if(szBuffer[k] == '\r' || szBuffer[k] == '\n') {
                szBuffer[k] = '\0';
                break;
            }
        }

        //If we found a category header
        if(szBuffer[0] == '[') {
            if(!_stricmp(szBuffer, "[maps]"))
                iAddToCategory = 1;
            else if(!_stricmp(szBuffer, "[worlds]"))
                iAddToCategory = 2;

            continue;
        }

        //If we're not in a category, ignore this line
        if(iAddToCategory == 0)
            continue;

        char * pszName = strtok(szBuffer, ",\n");

        if(!pszName)
            continue;

        if(iAddToCategory == 1) {
            MapMusicOverride * override = new MapMusicOverride();

            override->mapname = pszName;

            char * pszMusic = strtok(NULL, ",\n");
            while(pszMusic) {
                std::string sPath = convertPath(pszMusic);

                if(File_Exists(sPath.c_str())) {
                    override->songs.push_back(sPath);
                }

                pszMusic = strtok(NULL, ",\n");
            }

            //Don't add overrides that have no songs
            if(override->songs.size() == 0) {
                delete override;
                continue;
            }

            mapmusicoverrides.push_back(override);
        } else if(iAddToCategory == 2) {
            WorldMusicOverride * override = new WorldMusicOverride();

            override->worldname = pszName;

            char * pszMusic = strtok(NULL, ",\n");
            if(pszMusic) {
                std::string sPath = convertPath(pszMusic);

                if(File_Exists(sPath.c_str())) {
                    override->song = sPath;
                    worldmusicoverrides.push_back(override);
                }
            }
        }
    }

    musiclist->UpdateEntriesWithOverrides();
    worldmusiclist->UpdateEntriesWithOverrides();

    fclose(file);
}


