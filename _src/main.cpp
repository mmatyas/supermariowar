/*----------------------------------------------------------+
| super mario war											|
|															|
| a mario war clone written using the tile based collision	|
| detection technique explained in jnrdev #1				|
|															|
| you can read the tutorial on http://jnrdev.72dpiarmy.net	|
|															|
|															|
| this sourcecode is released under the GPL.				|
|															|
|															|
| the code is a bit messy and contains a few things i'm not	|
| really happy with. (collision detection system, ...)      |
| but it works and the game is fun playing :)				|
|															|
|															|
| maybe put all the game relevant stuff in a CGame class	|
| when adding a menu.										|
|															|
|															|
| start:		24.01.2003									|
| last changes:	01.04.2005									|
|															|
|									 2004 © Florian Hufsky  |
|								  florian.hufsky@gmail.com	|
|                                     mtschaffer@gmail.com  |
|								  http://smw.72dpiarmy.com	|
+----------------------------------------------------------*/

#ifdef _XBOX
	#include <xtl.h>
#endif

#include "global.h"				//all the global stuff
#include <time.h>

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

short			x_shake = 0;
short			y_shake = 0;

//------ sprites (maybe this should be done in a resource manger) ------
gfxSprite       spr_tanooki, spr_statue;
gfxSprite		** spr_player[4];	//all player sprites (see global.h)
gfxSprite		** spr_chocobo[4];
gfxSprite		** spr_bobomb[4];
gfxSprite		spr_clouds[2];
gfxSprite		spr_ghosts[3];
gfxSprite		spr_background;
gfxSprite		spr_backmap;
gfxSprite		spr_frontmap;
gfxSprite		menu_backdrop;

gfxFont			menu_font_small;
gfxFont			menu_font_large;
gfxFont			game_font_small;
gfxFont			game_font_large;

gfxSprite		menu_shade;
gfxSprite		menu_smw;
gfxSprite		menu_version;
gfxSprite		menu_plain_field;
gfxSprite		menu_player_select;
gfxSprite		menu_map_filter;
gfxSprite		menu_dialog;
gfxSprite		menu_slider_bar;
gfxSprite		menu_stomp;
gfxSprite		menu_survival;
gfxSprite		menu_mode_small;
gfxSprite		menu_mode_large;
gfxSprite		spr_dialog;
gfxSprite		spr_dialogbutton;
gfxSprite		spr_tournament_background;
gfxSprite		spr_tournament_powerup_splash;
gfxSprite		spr_player_select_background;
gfxSprite		spr_player_select_ready;
gfxSprite		spr_ipfield;
gfxSprite		spr_selectfield;
gfxSprite		spr_selectfielddisabled;
gfxSprite		spr_map_filter_icons;
gfxSprite		spr_tour_markers;
gfxSprite		spr_menu_boxed_numbers;
gfxSprite		spr_thumbnail_platformarrows;
gfxSprite		spr_thumbnail_warps[2];

gfxSprite		spr_noteblock;
gfxSprite		spr_breakableblock;
gfxSprite		spr_powerupblock;
gfxSprite		spr_donutblock;
gfxSprite		spr_flipblock;
gfxSprite		spr_bounceblock;
gfxSprite		spr_throwblock;
gfxSprite		spr_switchblocks;

gfxSprite		spr_brokenyellowblock;
gfxSprite		spr_brokenflipblock;
gfxSprite		spr_brokenblueblock;

gfxSprite		spr_starpowerup;
gfxSprite		spr_1uppowerup;
gfxSprite		spr_2uppowerup;
gfxSprite		spr_3uppowerup;
gfxSprite		spr_5uppowerup;
gfxSprite		spr_firepowerup;
gfxSprite		spr_hammerpowerup;
gfxSprite		spr_poisonpowerup;
gfxSprite		spr_mysterymushroompowerup;
gfxSprite		spr_boomerangpowerup;
gfxSprite		spr_clockpowerup;
gfxSprite		spr_bobombpowerup;
gfxSprite		spr_powpowerup;
gfxSprite		spr_modpowerup;
gfxSprite		spr_bulletbillpowerup;
gfxSprite		spr_featherpowerup;

gfxSprite		spr_shade[3];
gfxSprite		spr_timershade;
gfxSprite		spr_spawneggs;
gfxSprite		spr_scoretext;
gfxSprite		spr_racetext;
gfxSprite		spr_crown;
gfxSprite		spr_warplock;
gfxSprite		spr_cape;
gfxSprite		spr_coinsparkle;
gfxSprite		spr_shinesparkle;
gfxSprite		spr_shellbounce;

gfxSprite		spr_coin;
gfxSprite		spr_egg;
gfxSprite		spr_star;
gfxSprite		spr_frenzycards;
gfxSprite		spr_flags;
gfxSprite		spr_yoshi;
gfxSprite		spr_thwomp;
gfxSprite		spr_podobo;
gfxSprite		spr_bowserfire;
gfxSprite		spr_areas;
gfxSprite		spr_kingofthehillarea;
gfxSprite		spr_jail;
gfxSprite		spr_racegoal;
gfxSprite		spr_chicken;

gfxSprite		spr_goomba;
gfxSprite		spr_goombadead;
gfxSprite		spr_goombadeadflying;
gfxSprite		spr_koopa;
gfxSprite		spr_cheepcheep;
gfxSprite		spr_cheepcheepdead;
gfxSprite		spr_bulletbill;
gfxSprite		spr_bulletbilldead;

gfxSprite		spr_fireball;
gfxSprite		spr_hammer;
gfxSprite		spr_boomerang;
gfxSprite		spr_shell;
gfxSprite		spr_shelldead;
gfxSprite		spr_blueblock;

gfxSprite		spr_fireballexplosion;
gfxSprite		spr_frictionsmoke;
gfxSprite		spr_bobombsmoke;
gfxSprite		spr_explosion;
gfxSprite		spr_burnup;
gfxSprite		spr_spawnsmoke[4];
gfxSprite		spr_spawndoor[4];
gfxSprite		spr_bonus;
gfxSprite		spr_extralife;
gfxSprite		spr_award;
gfxSprite		spr_awardsolid;
gfxSprite		spr_awardsouls;
gfxSprite		spr_awardsoulspawn;
gfxSprite		spr_awardkillsinrow[4];
gfxSprite		spr_flagbases;
gfxSprite		spr_ownedtags;

gfxSprite		spr_storedpowerupsmall;
gfxSprite		spr_storedpoweruplarge;
gfxSprite		spr_powerupselector;

gfxSprite		spr_scoreboard;
gfxSprite		spr_abovearrows;

//------ game relevant stuff ------
CPlayer			*list_players[4];
short			list_players_cnt = 0;

CScore			*score[4];

short			score_cnt;
short			scoreoffsets[3] = {2, 36, 70};
short			scorepowerupoffsets[3][3] = {{37, 0, 0},
											{71, 89, 0},
											{105, 123, 141}};
short			respawn[4] = {0, 0, 0, 0};

short			projectiles[4];

extern short controlkeys[2][2][4][NUM_KEYS];
extern short g_iVersion[];

CMap			g_map;

CEyecandyContainer eyecandyback;
CEyecandyContainer eyecandyfront;

CObjectContainer objectblocks;
CObjectContainer objectcollisionitems;
CObjectContainer objectsfront;
CObjectContainer objectsplayer;

bool g_fLoadMessages = true;

STextAward awards[PAWARD_LAST] = {
								STextAward("Double Kill", &game_font_small),
								STextAward("Triple Kill", &game_font_small),
								STextAward("Killing Spree",  &game_font_small),
								STextAward("Killing Spree x 2", &game_font_small),
								STextAward("Killing Spree x 3", &game_font_small),
								STextAward("Dominating", &game_font_large),
								STextAward("Dominating x 2", &game_font_large),
								STextAward("Dominating x 3", &game_font_large),
								STextAward("Unstoppable!", &game_font_large)
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
sfxSound sfx_storedpowerupsound;
sfxSound sfx_kicksound;
sfxSound sfx_racesound;
sfxSound sfx_bulletbillsound;
sfxSound sfx_boomerang;
sfxSound sfx_spit;
sfxSound sfx_starwarning;
sfxSound sfx_powerdown;
sfxSound sfx_switchpress;

sfxMusic backgroundmusic[5];

CGameMode	*gamemodes[GAMEMODE_LAST];
short		currentgamemode = 0;

short g_iWinningPlayer;

extern short g_iPowerupToIcon[4];

extern void SetupScoreBoard(bool fOrderMatters);
//extern bool LoadMenuSkin(short playerID, short skinID, short colorID);

FiltersList filterslist;  //Filters list must be initiallized before maps list because it is used in maplist constructor
MapList maplist;
SkinList skinlist;
AnnouncerList announcerlist;
MusicList musiclist;
GraphicsList menugraphicspacklist;
GraphicsList gamegraphicspacklist;
SoundsList soundpacklist;
TourList tourlist;

//Network stuff
int g_iNextNetworkID = 0;
int g_iNextMessageID = 0;
char szIPString[32] = "";

//NetServer netServer;
//NetClient netClient;

extern Uint8 GetScreenBackgroundFade();

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
	if(vel < -MAXVELY)
		return -MAXVELY;
	
	if(vel > MAXVELY)
		return MAXVELY;
	
	return vel;
}

//handles a collision between a powerup and an object
void collisionhandler_o2o(IO_MovingObject &o1, CObject &o2)
{
	o2.collide(&o1);
}

short CountAliveTeams(short * lastteam)
{
	short findlastteam = 0;

	bool teamalive[4] = {false, false, false, false};
	for(short k = 0; k < list_players_cnt; k++)
	{
		if(!list_players[k]->isdead())
			teamalive[list_players[k]->teamID] = true;
	}

	short numteams = 0;
	for(short k = 0; k < 4; k++)
	{
		if(teamalive[k])
		{
			findlastteam = k;
			numteams++;
		}
	}

	if(lastteam != NULL)
	{
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

	for(short i = 0; i < list_players_cnt; i++)
	{
		if(list_players[i]->state == player_dead)
		{
			fCheckForGameOver = true;

			if(respawn[list_players[i]->globalID] <= 0)
				list_players[i]->die(0, true);

			//Set this to zero so we don't display a spawn egg when player is removed from game
			respawn[list_players[i]->globalID] = 0;

			if(game_values.gamemode->tagged == list_players[i])
				game_values.gamemode->tagged = NULL;

			if(game_values.gamemode->chicken == list_players[i])
				game_values.gamemode->chicken = NULL;

			delete list_players[i];
			
			for(short j = i; j < list_players_cnt - 1; j++)
			{
				list_players[j] = list_players[j + 1];
				list_players[j]->localID = j;
			}

			list_players_cnt--;
			list_players[list_players_cnt] = NULL;
		}
	}

	if(fCheckForGameOver)
	{
		short lastteam = -1;
		if(!game_values.gamemode->gameover && CountAliveTeams(&lastteam) <= 1)
		{
			game_values.gamemode->gameover = true;
			game_values.gamemode->winningteam = lastteam;
			SetupScoreBoard(true);  //pass true because the order the players died in matters
		}
	}
}

//Must only be called after organizeteams() is called
short LookupTeamID(short id, short * teamID, short * subTeamID)
{
	for(short i = 0; i < score_cnt; i++)
	{
		for(short j = 0; j < game_values.teamcounts[i]; j++)
		{
			if(game_values.teamids[i][j] == id)
			{
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
	for(short i = 0; i < list_players_cnt; i++)
	{
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
	else
	{
		g_PlayingSoundChannels[channel]->clearchannel();
		g_PlayingSoundChannels[channel] = NULL;
	}
}

void DECLSPEC musicfinished()
{
	if(!game_values.music)
		return;

	if(game_values.gamestate == GS_GAME && !game_values.gamemode->gameover)
	{
		if(game_values.playnextmusic)
		{
			musiclist.SetNextMusic(g_map.musicCategoryID, maplist.currentShortmapname(), g_map.szBackgroundFile);
			backgroundmusic[0].load(musiclist.GetCurrentMusic());
		}

		backgroundmusic[0].play(game_values.playnextmusic, false);
	}
	else
	{
		if(fResumeMusic)
		{
			backgroundmusic[3].play(false, false);
		}
	}
}

void RunGame();
void CleanUp();
bool LoadAndSplashScreen();
void LoadMapObjects();
void UpdateScoreBoard();
void PlayNextMusicTrack();

//void runmenu(MenuID id);
//void createmenu();

Menu g_Menu;
gv game_values;
//MenuContext menu_context;

//Joystick-Init
SDL_Joystick **joysticks = NULL;
short joystickcount = 0;

#ifdef _DEBUG
bool g_fAutoTest = false;
bool g_fRecordTest = false;
#endif

// ------ MAIN ------
int main(int argc, char *argv[])
{
	printf("-------------------------------------------------------------------------------\n");
	printf(" %s %s\n", TITLESTRING, VERSIONNUMBER);
	printf("-------------------------------------------------------------------------------\n");
	printf("\n---------------- startup ----------------\n");

	gfx_init(640, 480, false);		//initialize the graphics (SDL)
	blitdest = screen;

	/*
	//Comment this in to performance test the preview map loading
	MI_MapField * miMapField = new MI_MapField(&spr_selectfield, 70, 165, "Map", 500, 120);

	for(int k = 0; k < 100; k++)
	{
		game_values.playerInput.outputControls[3].menu_right.fPressed = true;
		miMapField->SendInput(&game_values.playerInput);
		//printf("Map over-> %s\n", maplist.currentFilename());
	}

	exit(0);
	*/

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
	game_values.fullscreen			= true;
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
	game_values.tournamentgames		= 1;
	game_values.tournamentwinner	= -1;
	game_values.tour				= false;
	game_values.tourstopcurrent		= 0;
	game_values.tourstoptotal		= 0;
	game_values.slowdownon			= -1;
	game_values.slowdowncounter		= 0;
	game_values.friendlyfire		= false;
	game_values.screencrunch		= true;
	game_values.screenshaketimer	= 0;
	game_values.screenshakeplayer   = NULL;
	game_values.toplayer			= true;
	game_values.loadedannouncer		= -1;
	game_values.loadedmusic			= -1;
	game_values.scoreboardstyle     = 0;
	game_values.teamcolors          = true;
	game_values.cputurn				= -1;
	game_values.spawninvincibility  = 62;
	game_values.musicvolume			= 128;
	game_values.soundvolume			= 128;
	game_values.respawn				= 2;
	game_values.itemrespawntime		= 1860;  //default item respawn is 30 seconds (30 * 62 fps)
	game_values.outofboundstime		= 5;
	game_values.warplocks			= 186;  // 3 seconds
	game_values.cpudifficulty		= 2;
	game_values.fireballttl			= 310;  // 5 seconds
	game_values.shellttl			= 496;  // 8 seconds
	game_values.blueblockttl		= 310;  // 5 seconds
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
	game_values.storedpowerupdelay	= 4;
	game_values.bonuswheel			= 1;
	game_values.keeppowerup			= false;
	game_values.showwinningcrown	= false;
	game_values.playnextmusic		= false;
	game_values.pointspeed			= 20;
	game_values.swapstyle			= 1;	//Blink then swap

	game_values.pfFilters			= new bool[NUM_AUTO_FILTERS + filterslist.GetCount()];
	game_values.piFilterIcons		= new short[NUM_AUTO_FILTERS + filterslist.GetCount()];
	game_values.fNeedWriteFilters	= false;
	
	for(short iFilter = 0; iFilter < NUM_AUTO_FILTERS + filterslist.GetCount(); iFilter++)
	{
		game_values.pfFilters[iFilter] = false;
		game_values.piFilterIcons[iFilter] = 0;
	}

	//game_values.networktype		= 0;
	//game_values.networkhost		= false;
	//game_values.gamehost			= false;
	
	//Set the default powerup weights for bonus wheel and [?] boxes
	for(short iPowerup = 0; iPowerup < NUM_POWERUPS; iPowerup++)
		game_values.powerupweights[iPowerup] = g_iDefaultPowerupWeights[iPowerup];

	announcerlist.SetCurrent(0);
	musiclist.SetCurrent(0);
	menugraphicspacklist.SetCurrent(0);
	gamegraphicspacklist.SetCurrent(0);
	soundpacklist.SetCurrent(0);

	for(short iPlayer = 0; iPlayer < 4; iPlayer++)
	{
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
		for(short iInputType = 0; iInputType < 2; iInputType++)  //for keyboard/joystick
		{
			game_values.inputConfiguration[iPlayer][iInputType].iDevice = iInputType - 1;

			for(short iInputState = 0; iInputState < 2; iInputState++)  //for game/menu
			{
				for(short iKey = 0; iKey < NUM_KEYS; iKey++)
				{
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

	currentgamemode = 0;
	game_values.gamemode = gamemodes[currentgamemode];


	//Setup the default game mode settings

	//Jail
	game_values.gamemodesettings.jail.tagfree = true;		//players on same team can free player by touching
	game_values.gamemodesettings.jail.timetofree = 1240;   //20 seconds of jail

	//Coins
	game_values.gamemodesettings.coins.penalty = false;		//no penalty for getting stomped
	game_values.gamemodesettings.coins.quantity = 1;		//only 1 coin on screen

	//Stomp
	game_values.gamemodesettings.stomp.rate = 90; //Moderate
	game_values.gamemodesettings.stomp.enemyweight[0] = 1; // turn on goombas, koopa and cheep cheeps by default
	game_values.gamemodesettings.stomp.enemyweight[1] = 1;  
	game_values.gamemodesettings.stomp.enemyweight[2] = 2;

	//Capture The Flag
	game_values.gamemodesettings.flag.speed = 0;  //Bases don't move by default
	game_values.gamemodesettings.flag.touchreturn = false;  //Don't return by touching
	game_values.gamemodesettings.flag.pointmove = true;  //Move base after point
	game_values.gamemodesettings.flag.autoreturn = 1240;  //Return flag automatically after 20 seconds
	game_values.gamemodesettings.flag.homescore = false;  //Don't require flag to be home to score

	//Chicken
	game_values.gamemodesettings.chicken.usetarget = true;  //default to displaying a target around the chicken

	//Tag
	game_values.gamemodesettings.tag.tagontouch = true;  //default to transfer tag on touching other players

	//Star
	game_values.gamemodesettings.star.time = 30;			//default to 30 seconds
	game_values.gamemodesettings.star.shine = false;		//default to hot potato (ztar)

	//Domination
	game_values.gamemodesettings.domination.loseondeath = true;
	game_values.gamemodesettings.domination.stealondeath = false;
	game_values.gamemodesettings.domination.relocateondeath = false;
	game_values.gamemodesettings.domination.quantity = 13; //# Players + 1 = 13
	game_values.gamemodesettings.domination.relocationfrequency = 1240;  //Relocate after 20 seconds = 1240
	
	//King Of The Hill
	game_values.gamemodesettings.kingofthehill.areasize = 3;
	game_values.gamemodesettings.kingofthehill.relocationfrequency = 1240;

	//Race
	game_values.gamemodesettings.race.quantity = 4;
	game_values.gamemodesettings.race.speed = 4;
	game_values.gamemodesettings.race.penalty = 2;  //0 == none, 1 = 1 base, 2 = all bases lost on death
		
	//Frenzy
	game_values.gamemodesettings.frenzy.quantity = 6; //#players - 1
	game_values.gamemodesettings.frenzy.rate = 186; //3 seconds
	game_values.gamemodesettings.frenzy.storedshells = true; //Shells are stored by default
	game_values.gamemodesettings.frenzy.powerupweight[0] = 0;
	game_values.gamemodesettings.frenzy.powerupweight[1] = 1;  // turn on flowers and hammers by default
	game_values.gamemodesettings.frenzy.powerupweight[2] = 1;
	game_values.gamemodesettings.frenzy.powerupweight[3] = 0;
	game_values.gamemodesettings.frenzy.powerupweight[4] = 0;
	game_values.gamemodesettings.frenzy.powerupweight[5] = 0;
	game_values.gamemodesettings.frenzy.powerupweight[6] = 0;
	game_values.gamemodesettings.frenzy.powerupweight[7] = 0;
	game_values.gamemodesettings.frenzy.powerupweight[8] = 0;
	game_values.gamemodesettings.frenzy.powerupweight[9] = 0;
	game_values.gamemodesettings.frenzy.powerupweight[10] = 0;
	game_values.gamemodesettings.frenzy.powerupweight[11] = 0;

	//Survival
	game_values.gamemodesettings.survival.enemyweight[0] = 1;
	game_values.gamemodesettings.survival.enemyweight[1] = 0;
	game_values.gamemodesettings.survival.enemyweight[2] = 0;
	game_values.gamemodesettings.survival.density = 20;
	game_values.gamemodesettings.survival.speed = 4;
	game_values.gamemodesettings.survival.shield = true;
		

	//Read saved settings from disk
	FILE *fp;

#ifdef _XBOX
	fp = fopen("D:\\options.bin", "rb");
#else
#ifdef PREFIXPATH
	char * folder=getenv("HOME");
#ifdef __MACOSX__
	std::string optionsbin=std::string(folder)+
		std::string("/Library/Preferences/smw.options.bin");
#else
    std::string optionsbin=std::string(folder)+std::string("/.smw.options.bin");
#endif
    fp = fopen(optionsbin.c_str(), "rb");
#else
	fp = fopen("options.bin", "rb");
#endif
#endif

	if(fp != NULL)
	{
		short version[4];
		fread(version, sizeof(short), 4, fp);

		if(version[0] == g_iVersion[0] && version[1] == g_iVersion[1] && version[2] == g_iVersion[2] && version[3] == g_iVersion[3])
		{
			#ifdef _XBOX
				fread(&game_values.flickerfilter, sizeof(short), 1, fp);
				fread(&game_values.hardwarefilter, sizeof(short), 1, fp);
				fread(&game_values.softfilter, sizeof(short), 1, fp);
				fread(&game_values.aspectratio10x11, sizeof(bool), 1, fp);

				fread(&game_values.screenResizeX, sizeof(float), 1, fp);
				fread(&game_values.screenResizeY, sizeof(float), 1, fp);
				fread(&game_values.screenResizeW, sizeof(float), 1, fp);
				fread(&game_values.screenResizeH, sizeof(float), 1, fp);

				SDL_XBOX_SetScreenPosition(game_values.screenResizeX, game_values.screenResizeY);
				SDL_XBOX_SetScreenStretch(game_values.screenResizeW, game_values.screenResizeH);
			#endif

			unsigned char abyte[26];
			fread(abyte, sizeof(unsigned char), 26, fp);
			game_values.spawnstyle = (short) abyte[0];
			game_values.awardstyle = (short) abyte[1];
			game_values.friendlyfire = ((short)abyte[3] > 0 ? true : false);
			game_values.screencrunch = ((short)abyte[4] > 0 ? true : false);
			game_values.toplayer = ((short)abyte[5] > 0 ? true : false);
			game_values.scoreboardstyle = (short)abyte[6];
			game_values.teamcolors = ((short)abyte[7] > 0 ? true : false);
			game_values.sound = ((short)abyte[8] > 0 ? true : false);
			game_values.music = ((short)abyte[9] > 0 ? true : false);
			game_values.musicvolume = (short)abyte[10];
			game_values.soundvolume = (short)abyte[11];
			game_values.respawn = (short)abyte[12];
			game_values.outofboundstime = (short)abyte[14];
			game_values.cpudifficulty = (short)abyte[15];
			game_values.framelimiter = (short)abyte[18];
			game_values.bonuswheel = (short)abyte[19];
			game_values.keeppowerup = ((short)abyte[20] > 0 ? true : false);
			game_values.showwinningcrown = ((short)abyte[21] > 0 ? true : false);
			game_values.playnextmusic = ((short)abyte[22] > 0 ? true : false);
			game_values.pointspeed = (short)abyte[23];
			game_values.swapstyle = (short)abyte[24];
			
			fread(&game_values.spawninvincibility, sizeof(short), 1, fp);
			fread(&game_values.itemrespawntime, sizeof(short), 1, fp);
			fread(&game_values.fireballttl, sizeof(short), 1, fp);
			fread(&game_values.fireballlimit, sizeof(short), 1, fp);
			fread(&game_values.hammerdelay, sizeof(short), 1, fp);
			fread(&game_values.hammerttl, sizeof(short), 1, fp);
			fread(&game_values.hammerpower, sizeof(bool), 1, fp);
			fread(&game_values.hammerlimit, sizeof(short), 1, fp);
			fread(&game_values.boomerangstyle, sizeof(short), 1, fp);
			fread(&game_values.boomeranglife, sizeof(short), 1, fp);
			fread(&game_values.boomeranglimit, sizeof(short), 1, fp);
			fread(&game_values.featherjumps, sizeof(short), 1, fp);
			fread(&game_values.featherlimit, sizeof(short), 1, fp);
			fread(&game_values.shellttl, sizeof(short), 1, fp);
			fread(&game_values.blueblockttl, sizeof(short), 1, fp);
			fread(&game_values.storedpowerupdelay, sizeof(short), 1, fp);
			fread(&game_values.warplocks, sizeof(short), 1, fp);

			//TODO: Need to test what happens when you unplug some controllers from the xbox
			//and then start up (device index will probably point to a gamepad that isn't in the list)
			//and this will cause a crash
			fread(game_values.inputConfiguration, sizeof(CInputPlayerControl), 8, fp);

			for(short iPlayer = 0; iPlayer < 4; iPlayer++)
			{
				short iDevice;
				fread(&iDevice, sizeof(short), 1, fp);

#ifdef _XBOX
				game_values.playerInput.inputControls[iPlayer] = &game_values.inputConfiguration[iPlayer][1]; //Always use gamepads as input devices on xbox
#else
				if(iDevice >= joystickcount)
					iDevice = DEVICE_KEYBOARD;

				game_values.playerInput.inputControls[iPlayer] = &game_values.inputConfiguration[iPlayer][iDevice == DEVICE_KEYBOARD ? 0 : 1];
#endif
			}

			#ifndef _XBOX
				fread(&game_values.fullscreen, sizeof(bool), 1, fp);
			#endif
			
			for(short iGameMode = 0; iGameMode < GAMEMODE_LAST; iGameMode++)
				fread(&(gamemodes[iGameMode]->goal), sizeof(short), 1, fp);

			for(short iPowerup = 0; iPowerup < NUM_POWERUPS; iPowerup++)
				fread(&game_values.powerupweights[iPowerup], sizeof(short), 1, fp);

			fread(&game_values.gamemodesettings, sizeof(GameModeSettings), 1, fp);

			fread(&game_values.teamcounts, sizeof(short), 4, fp);
			fread(&game_values.teamids, sizeof(short), 12, fp);
			fread(&game_values.skinids, sizeof(short), 4, fp);
			fread(&game_values.randomskin, sizeof(bool), 4, fp);
			fread(&game_values.playercontrol, sizeof(short), 4, fp);

			//Load skin/team settings
			for(short iPlayer = 0; iPlayer < 4; iPlayer++)
			{
				if(game_values.skinids[iPlayer] >= skinlist.GetCount() || game_values.skinids[iPlayer] < 0)
					game_values.skinids[iPlayer] = 0;
			}

			announcerlist.SetCurrent((short) abyte[2]);
			musiclist.SetCurrent((short) abyte[13]);
			menugraphicspacklist.SetCurrent((short) abyte[16]);
			gamegraphicspacklist.SetCurrent((short) abyte[25]);
			soundpacklist.SetCurrent((short) abyte[17]);

			sfx_setmusicvolume(game_values.musicvolume);
			sfx_setsoundvolume(game_values.soundvolume);
		}
		else
		{
			printf("Old options.bin detected.  Skipped reading it.\n");
		}

		fclose(fp);
	}

#ifdef _XBOX
	gfx_setresolution(640, 480, false); //Sets flicker filter
	SDL_SetHardwareFilter(game_values.hardwarefilter);
	blitdest = screen;
#else
	if(game_values.fullscreen)
	{
		gfx_setresolution(640, 480, true);
		blitdest = screen;
	}
#endif

	//Load the gfx color palette
	gfx_loadpalette();

	//Call to setup input optimization
	game_values.playerInput.CheckIfMouseUsed();

	srand((unsigned int)time(NULL));

	bool fLoadOK = LoadAndSplashScreen();

	if(!fLoadOK)
	{
		printf("\n---------------- EXIT DURING LOADING ----------------\n\n");
        sfx_close();
		gfx_close();
		//net_close();
		return 0;
	}

	printf("\n---------------- ready, steady, go! ----------------\n");

	g_Menu.CreateMenu();
	g_Menu.RunMenu();

	while(game_values.gamestate != GS_QUIT)
	{
		switch(game_values.gamestate)
		{
			case GS_START_GAME:
			case GS_GAME:
				RunGame();
			break;

			case GS_MENU:
				g_Menu.RunMenu();
			break;

            case GS_QUIT: // added because of warning on not handling all of enum
            break;
		}
	}
	
	

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
	for(short k = 0; k < 4; k++)
	{
		for(short j = 0; j < PGFX_LAST; j++)
		{
			delete spr_player[k][j];
			delete spr_chocobo[k][j];
			delete spr_bobomb[k][j];
		}

		delete [] spr_player[k];
		delete [] spr_chocobo[k];
		delete [] spr_bobomb[k];
		
		delete score[k];
	}

	delete [] game_values.pfFilters;
	delete [] game_values.piFilterIcons;

//Return to dash on xbox
#ifdef _XBOX
	LD_LAUNCH_DASHBOARD LaunchData = { XLD_LAUNCH_DASHBOARD_MAIN_MENU };
	XLaunchNewImage( NULL, (LAUNCH_DATA*)&LaunchData );
#endif

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

bool coldec_player2player(CPlayer &o1, CPlayer &o2);
bool coldec_player2obj(CPlayer &o1, CObject &o2);
bool coldec_obj2obj(CObject &o1, CObject &o2);

void RunGame()
{
	unsigned int	framestart, ticks;
	SDL_Event		event;
	short			i, j;
	float			realfps = 0, flipfps = 0;

	//Reset the keys each time we switch from menu to game and back
	game_values.playerInput.ResetKeys();

#ifdef _DEBUG
	static int exitgametimer = 0;
	exitgametimer = 0;
#endif
	
	y_shake = 0;
	x_shake = 0;

	for(short iPlayer = 0; iPlayer < 4; iPlayer++)
	{
		projectiles[iPlayer] = 0;
		respawn[iPlayer] = 0;

		if(game_values.playercontrol[iPlayer] > 0)
		{
			short teamid, subteamid;
			LookupTeamID(iPlayer, &teamid, &subteamid);

			CPlayerAI * ai = NULL;
			if(game_values.playercontrol[iPlayer] == 2)
				ai = new CPlayerAI();

			list_players[list_players_cnt] = new CPlayer(iPlayer, list_players_cnt, teamid, subteamid, game_values.colorids[iPlayer], spr_player[iPlayer], score[teamid], &(respawn[iPlayer]), ai);
			list_players_cnt++;
		}
		else if(!game_values.keeppowerup)
		{
			//Reset off player's stored powerups
			game_values.storedpowerups[iPlayer] = -1;
		}

		game_values.gamepowerups[iPlayer] = game_values.storedpowerups[iPlayer];
		game_values.bulletbilltimer[iPlayer] = 0;
		game_values.bulletbillspawntimer[iPlayer] = 0;
	}

	game_values.gamemode->init();
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
	game_values.swapplayers = false;
	game_values.swapplayersposition = 0.0f;
	game_values.swapplayersblink = false;
	game_values.swapplayersblinkcount = 0;
	game_values.screenfade = 255;
	game_values.screenfadespeed = -8;

	short totalspace = 0;
	for(i = 0; i < score_cnt; i++)
	{
		totalspace += 56 + game_values.teamcounts[i] * 34;
	}
	totalspace += 20 * (score_cnt - 1);

	for(i = 0; i < score_cnt; i++)
	{
		if(game_values.scoreboardstyle == 0 || game_values.scoreboardstyle == 1)
		{
			score[i]->x = ((640 - totalspace) >> 1);

			for(short k = 0; k < i; k++)
				score[i]->x += 76 + game_values.teamcounts[k] * 34;

			score[i]->y = 5 + (game_values.scoreboardstyle == 1 ? 429 : 0);
		}
		else
		{
			score[i]->x = 5 + (574 - (34 * game_values.teamcounts[i])) * (i % 2);
			score[i]->y = 5 + 429 * (i > 1 ? 1 : 0);
		}

		score[i]->fromx = score[i]->x;
		score[i]->fromy = score[i]->y;
		score[i]->place = i;
		score[i]->order = -1;
	}

	if(g_map.eyecandyID == 1)
	{
		for(i = 0; i < 4; i++)
		{
			short c = (short)(rand() % 2);	//cloud type (0... small cloud, 1... big cloud)
			float velx;			//speed of cloud, small clouds are slower than big ones
			if(c == 0)
				velx = (short)(rand()%51-25)/10.0f;	//big clouds: -3 - +3 pixel/frame
			else
				velx = (short)(rand()%41-20)/10.0f;	//small clouds: -2 - +2 pixel/frame
			
			velx = velx < 0.5f && velx > -0.5f ? 1 : velx;	//no static clouds please

			//add cloud to eyecandy array
			eyecandyfront.add(new EC_Cloud(&spr_clouds[c], (float)(rand()%640), (float)(rand()%100), velx));
		}
	}
	else if(g_map.eyecandyID == 2)
	{
		for(i = 0; i < 12; i++)
		{
			short c = (short)(rand() % 3);	//ghost type
			float velx = (short)(rand()%51-25)/10.0f;	//big clouds: -3 - +3 pixel/frame
			
			velx = velx < 0.5f && velx > -0.5f ? (rand() % 1 ? 1.0f : -1.0f) : velx;	//no static clouds please

			//add cloud to eyecandy array
			eyecandyfront.add(new EC_Ghost(&spr_ghosts[c], (float)(rand()%640), (float)(rand()%100), velx, 8, 2));
		}
	}
	
	short iScoreTextOffset[4];
	for(short iTeam = 0; iTeam < score_cnt; iTeam++)
	{
		iScoreTextOffset[iTeam] = 34 * game_values.teamcounts[iTeam] + 1;
	}

	
	while (true)
	{
		framestart = SDL_GetTicks();

/*
#ifdef _XBOX
		if(joystickcount != SDL_NumJoysticks())
			reconnectjoysticks();
#endif
*/
#ifdef _DEBUG

		static short endgametimer = (short)(rand() % 200);
		if(g_fAutoTest && !game_values.swapplayers)
		{
			for(short k = 0; k < list_players_cnt; k++)
			{
				if(list_players[k]->isready())
				{
					//Detect player is in center of tile only
					short x = (list_players[k]->ix + HALFPW) / TILESIZE;
					
					if(list_players[k]->ix + HALFPW >= 640)
						x = (list_players[k]->ix + HALFPW - 640) / TILESIZE;

					short y = (list_players[k]->iy + HALFPH) / TILESIZE;

					TileType tile = tile_nonsolid;
					IO_Block * block = NULL;
					short blocktype = BLOCKSETSIZE;

					if(list_players[k]->iy + HALFPH >= 0 && list_players[k]->iy + HALFPH < 480)
					{
						tile = g_map.map(x, y);
						block = g_map.block(x, y);
						blocktype = g_map.blockat(x, y);
					}

					if( (tile != tile_nonsolid && tile != tile_solid_on_top) || 
						(block && blocktype != 3 && blocktype < 11))
					{
						game_values.pausegame = true;
						game_values.frameadvance = true;
						g_fAutoTest = false;
						break;
					}

					//Detect if any corner of player is in a tile
					short actualvalues[2][2];
					actualvalues[0][0] = list_players[k]->ix;

					if(actualvalues[0][0] < 0)
						actualvalues[0][0] += 640;

					actualvalues[0][1] = list_players[k]->ix + PW;

					if(actualvalues[0][1] >= 640)
						actualvalues[0][1] -= 640;

					actualvalues[1][0] = list_players[k]->iy;
					actualvalues[1][1] = list_players[k]->iy + PH;

					short corners[2][2];
					corners[0][0] = list_players[k]->ix / TILESIZE; 

					if(list_players[k]->ix < 0)
						corners[0][0] = (list_players[k]->ix + 640) / TILESIZE;

					corners[0][1] = (list_players[k]->ix + PW) / TILESIZE; 

					if(list_players[k]->ix + PW >= 640)
						corners[0][1] = (list_players[k]->ix + PW - 640) / TILESIZE;

					corners[1][0] = list_players[k]->iy / TILESIZE;
					corners[1][1] = (list_players[k]->iy + PH) / TILESIZE;

					for(short i = 0; i < 2; i++)
					{
						for(short j = 0; j < 2; j++)
						{
							TileType tile = tile_nonsolid;
							IO_Block * block = NULL;
							short blocktype = BLOCKSETSIZE;

							if(actualvalues[0][j] >= 0 && actualvalues[0][j] < 640 && actualvalues[1][i] > 0 && actualvalues[1][i] < 480)
							{
								tile = g_map.map(corners[0][j], corners[1][i]);
								block = g_map.block(corners[0][j], corners[1][i]);
								blocktype = g_map.blockat(corners[0][j], corners[1][i]);
							}

							if( (tile != tile_nonsolid && tile != tile_solid_on_top) || 
								(block && blocktype != 3 && blocktype < 11))
							{
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
			
			/*
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
				
			}
			*/
			
			//Automatically run menus
			if(game_values.showscoreboard)
			{
				if(--endgametimer < 0)
				{
					endgametimer = (short)(rand() % 200);

					CleanUp();
					game_values.gamestate = GS_MENU;

					if(game_values.tournament)
						UpdateScoreBoard();

					return;
				}
			}
			else
			{
				if(++exitgametimer >= 8000)
				{
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

		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
#ifndef _XBOX
				case SDL_QUIT:
				{
					CleanUp();
					game_values.gamestate = GS_QUIT;
					return;
				}
				break;
#endif
				case SDL_KEYDOWN:
				{
#ifndef _XBOX
					if(event.key.keysym.mod & (KMOD_LALT | KMOD_RALT))
					{
						if(event.key.keysym.sym == SDLK_F4)
						{
							CleanUp();
							game_values.gamestate = GS_QUIT;
							return;
						}
						else if(event.key.keysym.sym == SDLK_RETURN)
						{
							game_values.fullscreen = !game_values.fullscreen;
							gfx_setresolution(640, 480, game_values.fullscreen);
							blitdest = screen;
							
							//Continue with input -> don't feed this event to the input
							//otherwise it will pause the game when switching to full/windowed screen
							continue;
						}
					}
#endif
					if(event.key.keysym.sym == SDLK_F1)
					{
						game_values.showfps = !game_values.showfps;
					}
					else if(event.key.keysym.sym == SDLK_ESCAPE)
					{
						game_values.playerInput.outputControls[0].game_cancel.fPressed = true;
					}
					else if(event.key.keysym.sym == SDLK_TAB)
					{
						PlayNextMusicTrack();
					}
#ifdef _DEBUG
					else if(event.key.keysym.sym == SDLK_F2)
					{
						game_values.frameadvance = !game_values.frameadvance;
					}
					else if(event.key.keysym.sym == SDLK_F5)
					{
						game_values.autokill = !game_values.autokill;
					}
					else if(event.key.keysym.sym == SDLK_z)
					{
						for(short k = 0; k < list_players_cnt; k++)
						{
							list_players[k]->DeathAwards();

							if(!game_values.gamemode->playerkilledself(*(list_players[k])))
								list_players[k]->die(0, false);
						}
					}
					else if(event.key.keysym.sym == SDLK_1)
					{
						if(event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT))
							objectcollisionitems.add(new PU_BobombPowerup(&spr_bobombpowerup, list_players[0]->ix + 32, list_players[0]->iy - 1, 1, true, 32000, 30, 30, 1, 1));	
						else
							objectcollisionitems.add(new PU_StarPowerup(&spr_starpowerup, list_players[0]->ix + 32, list_players[0]->iy, 4, true, 2, 30, 30, 1, 1));
					}
					else if(event.key.keysym.sym == SDLK_2)
					{
						if(event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT))
							objectcollisionitems.add(new PU_PowPowerup(&spr_powpowerup, list_players[0]->ix + 32, list_players[0]->iy - 1, 8, true, 8, 30, 30, 1, 1));
						else
							objectcollisionitems.add(new PU_ExtraGuyPowerup(&spr_1uppowerup, list_players[0]->ix + 32, list_players[0]->iy, 1, true, 32000, 30, 30, 1, 1, 1));
					}
					else if(event.key.keysym.sym == SDLK_3)
					{
						if(event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT))
							objectcollisionitems.add(new PU_BulletBillPowerup(&spr_bulletbillpowerup, list_players[0]->ix + 32, list_players[0]->iy - 1, 1, true, 32000, 30, 30, 1, 1));
						else
							objectcollisionitems.add(new PU_ExtraGuyPowerup(&spr_2uppowerup, list_players[0]->ix + 32, list_players[0]->iy, 1, true, 32000, 30, 30, 1, 1, 2));
					}
					else if(event.key.keysym.sym == SDLK_4)
					{
						if(event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT))
							objectsplayer.add(new CO_Shell(0, list_players[0]->ix + 32, list_players[0]->iy, true, true, true, false));
						else
							objectcollisionitems.add(new PU_ExtraGuyPowerup(&spr_3uppowerup, list_players[0]->ix + 32, list_players[0]->iy, 1, true, 32000, 30, 30, 1, 1, 3));
					}
					else if(event.key.keysym.sym == SDLK_5)
					{
						if(event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT))
							objectsplayer.add(new CO_Shell(1, list_players[0]->ix + 32, list_players[0]->iy, false, true, true, false));
						else
							objectcollisionitems.add(new PU_ExtraGuyPowerup(&spr_5uppowerup, list_players[0]->ix + 32, list_players[0]->iy, 1, true, 32000, 30, 30, 1, 1, 5));
					}
					else if(event.key.keysym.sym == SDLK_6)
					{
						if(event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT))
							objectsplayer.add(new CO_Shell(2, list_players[0]->ix + 32, list_players[0]->iy, false, false, true, true));
						else
							objectcollisionitems.add(new PU_FirePowerup(&spr_firepowerup, list_players[0]->ix + 32, list_players[0]->iy, 1, true, 32000, 30, 30, 1, 1));
					}
					else if(event.key.keysym.sym == SDLK_7)
					{
						if(event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT))
							objectsplayer.add(new CO_Shell(3, list_players[0]->ix + 32, list_players[0]->iy, false, true, false, false));
						else
							objectcollisionitems.add(new PU_HammerPowerup(&spr_hammerpowerup, list_players[0]->ix + 32, list_players[0]->iy, 1, true, 32000, 30, 30, 1, 1));
					}
					else if(event.key.keysym.sym == SDLK_8)
					{
						if(event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT))
							objectcollisionitems.add(new PU_ModPowerup(&spr_modpowerup, list_players[0]->ix + 32, list_players[0]->iy, 8, true, 8, 30, 30, 1, 1));
						else
							objectcollisionitems.add(new PU_PoisonPowerup(&spr_poisonpowerup, list_players[0]->ix + 32, list_players[0]->iy, 1, true, 32000, 30, 30, 1, 1));
					}
					else if(event.key.keysym.sym == SDLK_9)
					{
						if(event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT))
							objectcollisionitems.add(new PU_FeatherPowerup(&spr_featherpowerup, list_players[0]->ix + 32, list_players[0]->iy - 1, 1, true, 32000, 30, 30, 1, 1));
						else
							objectcollisionitems.add(new PU_ClockPowerup(&spr_clockpowerup, list_players[0]->ix + 32, list_players[0]->iy - 1, 1, true, 32000, 30, 30, 1, 1));
					}
					else if(event.key.keysym.sym == SDLK_0)
					{
						if(event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT))
							objectcollisionitems.add(new PU_BoomerangPowerup(&spr_boomerangpowerup, list_players[0]->ix + 32, list_players[0]->iy - 1, 1, true, 32000, 30, 30, 1, 1));
						else
							objectcollisionitems.add(new PU_MysteryMushroomPowerup(&spr_mysterymushroompowerup, list_players[0]->ix + 32, list_players[0]->iy - 1, 1, true, 32000, 30, 30, 1, 1));
					}
					else if(event.key.keysym.sym == SDLK_INSERT)
					{
						g_fAutoTest = !g_fAutoTest;
					}
#endif
					break;
				}	

#ifdef _XBOX
				case SDL_JOYBUTTONDOWN:
				{
					if(event.jbutton.state == SDL_PRESSED && event.jbutton.button == 5)
					{
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

		if(game_values.screenfade == 0)
		{
			for(int iPlayer = 0; iPlayer < 4; iPlayer++)
			{
				COutputControl * playerKeys = &game_values.playerInput.outputControls[iPlayer];

				//If the start key is pressed (pause key)
				if(playerKeys->game_start.fPressed)
				{
					if(!game_values.showscoreboard && !game_values.exitinggame)
					{
						game_values.pausegame = !game_values.pausegame;

						if(game_values.pausegame)
						{
							menu_shade.setalpha(GetScreenBackgroundFade());
							menu_shade.draw(0, 0);
						}

						ifsoundonpause(sfx_invinciblemusic);
						ifsoundonpause(sfx_slowdownmusic);
						ifsoundonplay(sfx_pause);
					}
				}

				//Only player 1 is allowed to exit a game
				//if(iPlayer != 0)
				//	continue;

				//If the cancel button is pressed
				if(playerKeys->game_cancel.fPressed || (playerKeys->game_start.fPressed && game_values.showscoreboard))
				{
					if(game_values.showscoreboard)
					{
						CleanUp();
						game_values.gamestate = GS_MENU;

						if(game_values.tournament)
							UpdateScoreBoard();

						return;
					}
					else
					{
						if(!game_values.pausegame && !game_values.exitinggame)
						{
							menu_shade.setalpha(GetScreenBackgroundFade());
							menu_shade.draw(0, 0);
							game_values.exitinggame = true;
							ifsoundonpause(sfx_invinciblemusic);
							ifsoundonpause(sfx_slowdownmusic);

							//Reset the keys each time we switch from menu to game and back
							game_values.playerInput.ResetKeys();
						}
					}
				}

				//Deal with input to game exit dialog box
				if(game_values.exitinggame)
				{
					if(playerKeys->menu_left.fPressed)
						game_values.exityes = true;
					else if(playerKeys->menu_right.fPressed)
						game_values.exityes = false;

					if(playerKeys->menu_select.fPressed)
					{
						if(game_values.exityes)
						{
							CleanUp();
							game_values.exitinggame = false;
							game_values.exityes = false;
							game_values.gamestate = GS_MENU;
							return;
						}
						else
						{
							game_values.exitinggame = false;
							ifsoundonpause(sfx_invinciblemusic);
							ifsoundonpause(sfx_slowdownmusic);

							//Reset the keys each time we switch from menu to game and back
							game_values.playerInput.ResetKeys();
						}
					}
				}
			}
		}

		if(!game_values.pausegame && !game_values.exitinggame)
		{
			if(!game_values.swapplayers && game_values.screenfade == 0)
			{
				//Shake screen
				if(game_values.screenshaketimer > 0)
				{
					game_values.screenshaketimer--;

					static bool shakeleft = false;
					if(shakeleft)
					{
						x_shake -= 2;
						if(x_shake <= -2)
						{
							shakeleft = false;
						}
					}
					else
					{
						x_shake += 2;
						if(x_shake >= 2)
						{
							shakeleft = true;
						}
					}

					//Kill things touching the ground
					if(game_values.screenshakeplayer)
					{
						short k;
						for(k = 0; k < list_players_cnt; k++)
						{
							if(k == game_values.screenshakeplayer->localID)
								continue;

							if(!game_values.friendlyfire && game_values.screenshakeplayer->teamID == list_players[k]->teamID)
								continue;
							
							if(!list_players[k]->invincible && !list_players[k]->spawninvincible && list_players[k]->isready())
							{
								if(game_values.screenshakekillinair == list_players[k]->inair)
								{
									PlayerKilledPlayer(*(game_values.screenshakeplayer), *list_players[k], death_style_jump, kill_style_pow);
									game_values.screenshakekillscount++;
									
									if(game_values.screenshakeplayer->inair)
										game_values.screenshakeplayer->killsinrowinair--;  //Don't want to give both shake and in air award
								}
							}
						}

						for(k = 0; k < objectcollisionitems.list_end; k++)
						{
							if(objectcollisionitems.list[k]->getObjectType() == object_moving)
							{
								IO_MovingObject * object = (IO_MovingObject *)objectcollisionitems.list[k];
								
								if((object->getMovingObjectType() == movingobject_goomba || object->getMovingObjectType() == movingobject_koopa)
									&& game_values.screenshakekillinair == object->inair)
								{
									if(!game_values.gamemode->gameover)
										game_values.screenshakeplayer->score->AdjustScore(1);

									ifsoundonplay(sfx_kicksound);
									((MO_Goomba*)object)->Die();

									game_values.screenshakekillscount++;
									
									if(game_values.screenshakeplayer->inair)
										game_values.screenshakeplayer->killsinrowinair--;  //Don't want to give both shake and in air award
								}
							}
						}

						for(k = 0; k < objectsplayer.list_end; k++)
						{
							if(objectsplayer.list[k]->getObjectType() == object_moving)
							{
								IO_MovingObject * object = (IO_MovingObject *)objectsplayer.list[k];
								
								if(game_values.screenshakekillinair == object->inair)
								{
									if(object->getMovingObjectType() == movingobject_shell)
									{
										CO_Shell * shell = (CO_Shell*)object;
										if(!shell->owner || shell->owner->inair == game_values.screenshakekillinair)
											shell->Die();
									}
									else if(object->getMovingObjectType() == movingobject_throwblock)
									{
										CO_ThrowBlock * throwblock = (CO_ThrowBlock*)object;
										if(!throwblock->owner || throwblock->owner->inair == game_values.screenshakekillinair)
											throwblock->Die();
									}
								}
							}
						}
					}

					if(game_values.screenshakekillscount > 1 && game_values.awardstyle != award_style_none)
					{
						game_values.screenshakekillscount = 0;
						game_values.screenshakeplayer->AddKillsInRowInAirAward();
					}
				}
				else
				{
					//Make sure we zero out the shake value after it is done
					x_shake = 0;
				}

				for(short iPlayer = 0; iPlayer < 4; iPlayer++)
				{
					if(game_values.bulletbilltimer[iPlayer] > 0)
					{
						game_values.bulletbilltimer[iPlayer]--;
						
						if(--game_values.bulletbillspawntimer[iPlayer] <= 0)
						{
							game_values.bulletbillspawntimer[iPlayer] = (short)(rand() % 20 + 25);
							float speed = ((float)(rand() % 21 + 20)) / 10.0f;
							objectsfront.add(new OMO_BulletBill(&spr_bulletbill, (short)(rand() % 448), (rand() % 2 ? speed : -speed), iPlayer));
							ifsoundonplay(sfx_bulletbillsound);
						}
					}
				}

				//------------- update objects -----------------------

#ifdef _DEBUG
				if(game_values.autokill)
				{
					for(short k = 0; k < list_players_cnt; k++)
					{
						list_players[k]->DeathAwards();

						if(!game_values.gamemode->playerkilledself(*(list_players[k])))
							list_players[k]->die(0, false);
					}
				}
#endif

				if(++game_values.cputurn > 3)
					game_values.cputurn = 0;

				//Player to player collisions
				for(i = 0; i < list_players_cnt; i++)
				{
					if(list_players[i]->state > player_dead)
					{
						for(j = i + 1; j < list_players_cnt; j++)
						{
							if(list_players[j]->state > player_dead)
							{
								if(coldec_player2player(*(list_players[i]), *(list_players[j])))
								{
									collisionhandler_p2p(*(list_players[i]), *(list_players[j]));

									//if player was killed by another player, continue with next player for collision detection
									if(list_players[i]->state <= player_dead)
										break;
								}
							}
						}
					}
				}
				
				g_map.updatePlatforms();

				game_values.playskidsound = false;
				game_values.playinvinciblesound = false;

				for(i = 0; i < list_players_cnt; i++)
					list_players[i]->move();	//move all objects before doing object-object collision detection in
												//->think(), so we test against the new position after object-map collision detection

				if(!game_values.swapplayers)
				{
					if(game_values.playskidsound)
					{
						if(!sfx_skid.isplaying())
							ifsoundonplay(sfx_skid);
					}
					else
					{
						if(sfx_skid.isplaying())
							ifsoundonstop(sfx_skid);
					}

					objectblocks.update();
					objectcollisionitems.update();
					objectsfront.update();
					objectsplayer.update();

					for(i = 0; i < list_players_cnt; i++)
					{
						if(!list_players[i]->isready())
							continue;

						//Collide with collision detected objects
						for(j = 0; j < objectcollisionitems.list_end; j++)
						{
							if(!objectcollisionitems.list[j]->GetDead() && objectcollisionitems.list[j]->GetState() >= 1)
							{
								if(coldec_player2obj(*(list_players[i]), *(objectcollisionitems.list[j])))
								{
									if(collisionhandler_p2o(*(list_players[i]), *(objectcollisionitems.list[j])))
										break;
								}
							}
						}

						if(!list_players[i]->isready())
							continue;

						if(game_values.swapplayers)
							break;

						//Collide with player objects
						for(j = 0; j < objectsplayer.list_end; j++)
						{
							if(!objectsplayer.list[j]->GetDead())
							{
								if(coldec_player2obj(*(list_players[i]), *(objectsplayer.list[j])))
								{
									if(collisionhandler_p2o(*(list_players[i]), *(objectsplayer.list[j])))
										break;
								}
							}
						}

						if(!list_players[i]->isready())
							continue;

						//Collide with front objects
						for(j = 0; j < objectsfront.list_end; j++)
						{
							if(!objectsfront.list[j]->GetDead())
							{
								if(coldec_player2obj(*(list_players[i]), *(objectsfront.list[j])))
								{
									if(collisionhandler_p2o(*(list_players[i]), *(objectsfront.list[j])))
										break;
								}
							}
						}
					}

					if(!game_values.swapplayers)
					{
						//moving object to moving object collisions
						for(i = 0; i < objectcollisionitems.list_end; i++)
						{
							if(!objectcollisionitems.list[i]->GetDead() && objectcollisionitems.list[i]->getObjectType() == object_moving && objectcollisionitems.list[i]->GetState() == 1)
							{
								IO_MovingObject * object = (IO_MovingObject*)objectcollisionitems.list[i];

								//collision items
								if(object->getMovingObjectType() == movingobject_fireball)
								{
									for(j = 0; j < objectcollisionitems.list_end; j++)
									{
										if(i == j)
											continue;

										if(!objectcollisionitems.list[j]->GetDead() && objectcollisionitems.list[j]->GetState() == 1)
										{
											if(coldec_obj2obj(*(objectcollisionitems.list[i]), *(objectcollisionitems.list[j])))
											{
												collisionhandler_o2o(*object, *(objectcollisionitems.list[j]));
											}
										}
									}

									//fireball to shell/throwblock
									for(j = 0; j < objectsplayer.list_end; j++)
									{
										if(objectsplayer.list[j]->getObjectType() == object_moving)
										{
											MovingObjectType type = ((IO_MovingObject*)objectsplayer.list[j])->getMovingObjectType();

											if((type == movingobject_shell || type == movingobject_throwblock) && !objectsplayer.list[j]->GetDead())
											{
												if(coldec_obj2obj(*(objectcollisionitems.list[i]), *(objectsplayer.list[j])))
												{
													collisionhandler_o2o(*object, *(objectsplayer.list[j]));
												}
											}
										}
									}
								}

								if(object->getMovingObjectType() == movingobject_goomba || object->getMovingObjectType() == movingobject_koopa || object->getMovingObjectType() == movingobject_fireball)
								{
									for(j = 0; j < objectsfront.list_end; j++)
									{
										if(!objectsfront.list[j]->GetDead())
										{
											if(coldec_obj2obj(*(objectcollisionitems.list[i]), *(objectsfront.list[j])))
											{
												collisionhandler_o2o(*object, *(objectsfront.list[j]));
											}
										}
									}
								}
							}
						}

						//front objects to front and collision items (bullet bills to explosions and hammers to bullet bills and goombas)
						for(i = 0; i < objectsfront.list_end; i++)
						{
							if(!objectsfront.list[i]->GetDead() && objectsfront.list[i]->getObjectType() == object_moving)
							{
								IO_MovingObject * object = (IO_MovingObject*)objectsfront.list[i];
								MovingObjectType type = object->getMovingObjectType();
								
								if(type == movingobject_bulletbill || type == movingobject_hammer || type == movingobject_cheepcheep || type == movingobject_boomerang)
								{
									//bullet bills to explosions && hammers to bullet bills
									for(j = 0; j < objectsfront.list_end; j++)
									{
										if(i == j)
											continue;

										bool fCollideWith = false;
										if(objectsfront.list[j]->getObjectType() == object_moving)
										{
											MovingObjectType type = ((IO_MovingObject*)objectsfront.list[j])->getMovingObjectType();
										
											if(type == movingobject_bulletbill || type == movingobject_cheepcheep)
												fCollideWith = true;
										}

										if((fCollideWith || objectsfront.list[j]->getObjectType() == object_explosion) && !objectsfront.list[j]->GetDead())
										{
											if(coldec_obj2obj(*(objectsfront.list[i]), *(objectsfront.list[j])))
											{
												collisionhandler_o2o(*object, *(objectsfront.list[j]));

												//If the object was killed by this collision, break out
												if(objectsfront.list[i]->GetDead())
													break;
											}
										}
									}

									//hammers to goombas
									for(j = 0; j < objectcollisionitems.list_end; j++)
									{
										bool collidewithobject = false;
										
										if(objectcollisionitems.list[j]->getObjectType() == object_moving)
										{
											IO_MovingObject * movingobject = (IO_MovingObject*)objectcollisionitems.list[j];

											if(movingobject->getMovingObjectType() == movingobject_goomba || movingobject->getMovingObjectType() == movingobject_koopa)
												collidewithobject = true;
										}

										if(collidewithobject && !objectcollisionitems.list[j]->GetDead())
										{
											if(coldec_obj2obj(*(objectsfront.list[i]), *(objectcollisionitems.list[j])))
											{
												collisionhandler_o2o(*object, *(objectcollisionitems.list[j]));
											}
										}
									}

									if(type == movingobject_hammer || type == movingobject_boomerang)
									{
										//hammer to shell/throwblock
										for(j = 0; j < objectsplayer.list_end; j++)
										{
											if(objectsplayer.list[j]->getObjectType() == object_moving)
											{
												MovingObjectType type = ((IO_MovingObject*)objectsplayer.list[j])->getMovingObjectType();

												if((type == movingobject_shell || type == movingobject_throwblock) && !objectsplayer.list[j]->GetDead())
												{
													if(coldec_obj2obj(*(objectsfront.list[i]), *(objectsplayer.list[j])))
													{
														collisionhandler_o2o(*object, *(objectsplayer.list[j]));
													}
												}
											}
										}
									}
								}
							}
						}

						//player objects to collision items (shells to goombas and shell to shell)
						//Also flags to flag bases
						for(i = 0; i < objectsplayer.list_end; i++)
						{
							if(!objectsplayer.list[i]->GetDead())
							{
								if(objectsplayer.list[i]->getObjectType() == object_moving)
								{
									IO_MovingObject * object = (IO_MovingObject*)objectsplayer.list[i];

									if(object->getMovingObjectType() == movingobject_shell || object->getMovingObjectType() == movingobject_throwblock)
									{
										//shell/throwblock to goomba and shell/throwblock to fireball
										for(j = 0; j < objectcollisionitems.list_end; j++)
										{
											if(objectcollisionitems.list[j]->getObjectType() == object_moving)
											{
												IO_MovingObject * object2 = (IO_MovingObject*)objectcollisionitems.list[j];
											
												if(object2->getMovingObjectType() == movingobject_goomba || object2->getMovingObjectType() == movingobject_koopa)
												{
													if(!objectcollisionitems.list[j]->GetDead())
													{
														if(coldec_obj2obj(*(objectsplayer.list[i]), *(objectcollisionitems.list[j])))
														{
															collisionhandler_o2o(*object, *(objectcollisionitems.list[j]));
														}
													}
												}
											}
										}

										//shell to shell or block to block
										for(j = 0; j < objectsplayer.list_end; j++)
										{
											if(i == j)
												continue;

											if(objectsplayer.list[j]->getObjectType() == object_moving)
											{
												MovingObjectType type = ((IO_MovingObject*)objectsplayer.list[j])->getMovingObjectType();

												if((type == movingobject_shell || type == movingobject_throwblock) && !objectsplayer.list[j]->GetDead())
												{
													if(coldec_obj2obj(*(objectsplayer.list[i]), *(objectsplayer.list[j])))
													{
														collisionhandler_o2o(*object, *(objectsplayer.list[j]));
													}
												}
											}
										}

										//shells/throwblocks to explosions
										for(j = 0; j < objectsfront.list_end; j++)
										{
											bool fCollideWith = false;
											if(objectsfront.list[j]->getObjectType() == object_moving)
											{
												MovingObjectType type = ((IO_MovingObject*)objectsfront.list[j])->getMovingObjectType();
											
												if(type == movingobject_bulletbill || type == movingobject_cheepcheep)
													fCollideWith = true;
											}

											if((fCollideWith || objectsfront.list[j]->getObjectType() == object_explosion) && !objectsfront.list[j]->GetDead())
											{
												if(coldec_obj2obj(*(objectsplayer.list[i]), *(objectsfront.list[j])))
												{
													collisionhandler_o2o(*object, *(objectsfront.list[j]));
												}
											}
										}
									}
								}
								else if(objectsplayer.list[i]->getObjectType() == object_flag)
								{
									CO_Flag * flag = (CO_Flag*)objectsplayer.list[i];

									//flag to flag base collisions
									for(j = 0; j < objectcollisionitems.list_end; j++)
									{
										if(objectcollisionitems.list[j]->getObjectType() == object_flagbase)
										{
											if(!objectcollisionitems.list[j]->GetDead())
											{
												if(coldec_obj2obj(*(objectsplayer.list[i]), *(objectcollisionitems.list[j])))
												{
													collisionhandler_o2o(*flag, *(objectcollisionitems.list[j]));
												}
											}
										}
									}
								}
								else if(objectsplayer.list[i]->getObjectType() == object_egg)
								{
									CO_Egg * egg = (CO_Egg*)objectsplayer.list[i];

									//thrown egg to yoshi collision
									for(j = 0; j < objectsplayer.list_end; j++)
									{
										if(i == j)
											continue;

										if(objectsplayer.list[j]->getObjectType() == object_yoshi)
										{
											if(!objectsplayer.list[j]->GetDead())
											{
												if(coldec_obj2obj(*(objectsplayer.list[i]), *(objectsplayer.list[j])))
												{
													collisionhandler_o2o(*egg, *(objectsplayer.list[j]));
												}
											}
										}
									}
								}
							}
						}

						eyecandyfront.cleandeadobjects();
						eyecandyback.cleandeadobjects();
						objectsfront.cleandeadobjects();
						objectsplayer.cleandeadobjects();
						objectcollisionitems.cleandeadobjects();
						objectblocks.cleandeadobjects();
						CleanDeadPlayers();

						eyecandyfront.update();
						eyecandyback.update();
						game_values.gamemode->think();

						if(game_values.slowdownon != -1)
						{
							if(!sfx_slowdownmusic.isplaying())
								ifsoundonplay(sfx_slowdownmusic);

							if(++game_values.slowdowncounter > 580)
							{
								game_values.slowdownon = -1;
								game_values.slowdowncounter = 0;
							}
						}
						else
						{
							if(sfx_slowdownmusic.isplaying())
								ifsoundonstop(sfx_slowdownmusic);
						}

						g_map.updateWarpLocks();

						if(y_shake > 0)
						{
							y_shake -= CRUNCHVELOCITY;

							if(y_shake < 0)
								y_shake = 0;
						}
					
						if(game_values.showscoreboard)
						{
							if(game_values.scorepercentmove < 1.0f)
							{
								game_values.scorepercentmove += 0.01f;
								
								if(game_values.scorepercentmove >= 1.0f)
									game_values.scorepercentmove = 1.0f;
							}
							else
							{
								game_values.scorepercentmove = 1.0f;
							}

							for(i = 0; i < score_cnt; i++)
							{
								score[i]->x = (short)((float)(score[i]->destx - score[i]->fromx) * game_values.scorepercentmove) + score[i]->fromx;
								score[i]->y = (short)((float)(score[i]->desty - score[i]->fromy) * game_values.scorepercentmove) + score[i]->fromy;
							}
						}
					}
				}
			}

			if(game_values.playinvinciblesound)
			{
				if(!sfx_invinciblemusic.isplaying() && !sfx_timewarning.isplaying() && !backgroundmusic[0].isplaying())
					ifsoundonplay(sfx_invinciblemusic);
			}
			else
			{
				if(sfx_invinciblemusic.isplaying())
					ifsoundonstop(sfx_invinciblemusic);
			}

			//If no background music is playing, then play some
			if(!backgroundmusic[0].isplaying() && !sfx_invinciblemusic.isplaying() && !sfx_timewarning.isplaying() && !game_values.gamemode->gameover)
			{
				if(game_values.playnextmusic)
				{
					musiclist.SetNextMusic(g_map.musicCategoryID, maplist.currentShortmapname(), g_map.szBackgroundFile);
					backgroundmusic[0].load(musiclist.GetCurrentMusic());
				}

				backgroundmusic[0].play(game_values.playnextmusic, false);
			}

			//--------------- draw everything ----------------------

			if(y_shake > 0)
			{
				SDL_Rect rect = {0, 0, 640, y_shake};
				SDL_FillRect(screen, &rect, 0x0);		//fill empty area with black
			}

			spr_backmap.draw(0, 0);

			//draw back eyecandy behind players
			objectblocks.draw();
			eyecandyback.draw();

			objectcollisionitems.draw();

			if(!game_values.swapplayers)
			{
				for(i = 0; i < list_players_cnt; i++)
					list_players[i]->draw();
			}

			objectsplayer.draw();

			g_map.drawPlatforms();

#ifdef _XBOX
			g_map.drawfrontlayer();
#else
			if(game_values.toplayer)
				g_map.drawfrontlayer();
#endif
			g_map.drawWarpLocks();

			objectsfront.draw();
			eyecandyfront.draw();
			game_values.gamemode->draw();
		
			g_iWinningPlayer = -1;
			short mostkills = 0;
			
			bool fReverseScoring = game_values.gamemode->GetReverseScoring();
			if(fReverseScoring)
				mostkills = 32000;
			
			for(i = 0; i < score_cnt; i++)
			{
				if((score[i]->score > mostkills && !fReverseScoring) || (score[i]->score < mostkills && fReverseScoring))
				{
					mostkills = score[i]->score;
					g_iWinningPlayer = i;
				}
				else if(score[i]->score == mostkills)
				{
					g_iWinningPlayer = -1;
				}
			}
			
			//big end game scoreboard (sorted)
			if(game_values.showscoreboard)
			{
				char gameovertext[128] = "";
				if(game_values.gamemode->winningteam > -1)
				{
					if(game_values.teamcounts[game_values.gamemode->winningteam] == 1)
						sprintf(gameovertext, "Player %d Wins!", game_values.teamids[game_values.gamemode->winningteam][0] + 1);
					else
						sprintf(gameovertext, "Team %d Wins!", game_values.gamemode->winningteam + 1);
				}
				else
				{
					sprintf(gameovertext, "Tie Game");
				}
					
				game_font_large.drawCentered(320, 100, gameovertext);
			}

			//in game scoreboards
			for(i = 0; i < score_cnt; i++)
			{
				spr_shade[game_values.teamcounts[i] - 1].draw(score[i]->x, score[i]->y);

				for(short k = 0; k < game_values.teamcounts[i]; k++)
				{
					short globalID = game_values.teamids[i][k];

					//If player is respawning, draw the egg
					if(respawn[globalID] > 0 && !game_values.gamemode->gameover)
					{
						spr_spawneggs.draw(score[i]->x + scoreoffsets[k], score[i]->y + 2, ((respawn[globalID] - 1) >> 1) * 32, game_values.colorids[globalID] * 32, 32, 32);
					}
					else  //otherwise draw the player's skin in the scoreboard
					{
						short iScoreboardSprite;
						if(game_values.gamemode->gameover)
						{
							if(g_iWinningPlayer != i)
								iScoreboardSprite = PGFX_DEADFLYING;
							else
								iScoreboardSprite = PGFX_JUMPING_R;
						}
						else
						{
							iScoreboardSprite = PGFX_STANDING_R;
						}
						
						//Search for player state to display
						CPlayer * player = GetPlayerFromGlobalID(globalID);
						
						if(player && !game_values.gamemode->gameover)
						{
							short iScoreOffsetX = score[i]->x + scoreoffsets[k];
							short iScoreOffsetY = score[i]->y + 2;

							if(player->ownerPlayerID > -1)
								spr_ownedtags.draw(iScoreOffsetX - 8, iScoreOffsetY - 8, player->ownerColorOffsetX, 0, 48, 48);
	
							player->GetScoreboardSprite()[iScoreboardSprite]->draw(iScoreOffsetX, iScoreOffsetY, player->iSrcOffsetX, 0, 32, 32);

							if(player->jailed > 0)
								spr_jail.draw(iScoreOffsetX - 6, iScoreOffsetY - 6);

							if(player->powerup > 0)
								spr_storedpowerupsmall.draw(iScoreOffsetX, iScoreOffsetY + 16, g_iPowerupToIcon[player->powerup - 1], 0, 16, 16);
						}
						else
						{
							spr_player[globalID][iScoreboardSprite]->draw(score[i]->x + scoreoffsets[k], score[i]->y + 2, 0, 0, 32, 32);
						}
					
						//give crown to player(s) with most kills
						if(g_iWinningPlayer == i)
							spr_crown.draw(score[i]->x + scoreoffsets[k] + 12, score[i]->y - 4);
					}

					short storedpowerupid = game_values.gamepowerups[globalID];

					if(storedpowerupid != -1)
					{
						if(!game_values.swapplayers)
						{
							spr_storedpowerupsmall.draw(score[i]->x + scorepowerupoffsets[game_values.teamcounts[i] - 1][k], score[i]->y + 25, storedpowerupid * 16, 0, 16, 16);
						}
					}
				}
				
				short iScoreX = score[i]->x + iScoreTextOffset[i];
				short iScoreY = score[i]->y + 4;

				spr_scoretext.draw(iScoreX, iScoreY, score[i]->iDigitLeft, (score[i]->iDigitLeft == 0 ? 16 : 0), 16, 16);
				spr_scoretext.draw(iScoreX + 18, iScoreY, score[i]->iDigitMiddle, (score[i]->iDigitLeft == 0 && score[i]->iDigitMiddle == 0 ? 16 : 0), 16, 16);
				spr_scoretext.draw(iScoreX + 36, iScoreY, score[i]->iDigitRight, 0, 16, 16);
			}


			//draw arrows for being above the top of the screen
			for(i = 0; i < list_players_cnt; i++)
				list_players[i]->drawarrows();


			if(game_values.screenfadespeed != 0)
			{
				game_values.screenfade += game_values.screenfadespeed;

				if(game_values.screenfade <= 0)
				{
					game_values.screenfadespeed = 0;
					game_values.screenfade = 0;
				}
				else if(game_values.screenfade >= 255)
				{
					game_values.screenfadespeed = 0;
					game_values.screenfade = 255;
				}
			}

			if(game_values.screenfade > 0)
			{
				menu_shade.setalpha((Uint8)game_values.screenfade);
				menu_shade.draw(0, 0);
			}

			if(game_values.swapplayers)
			{
				for(i = 0; i < list_players_cnt; i++)
				{
					list_players[i]->drawswap();

					short storedpowerupid = game_values.gamepowerups[list_players[i]->getGlobalID()];

					if(storedpowerupid != -1)
					{
						short iPowerupX, iPowerupY;

						if(game_values.swapstyle == 1)
						{
							iPowerupX = game_values.swapplayersblink ? list_players[i]->iOldPowerupX : list_players[i]->iNewPowerupX;
							iPowerupY = game_values.swapplayersblink ? list_players[i]->iOldPowerupY : list_players[i]->iNewPowerupY;
						}
						else
						{
							iPowerupX = (short)((float)(list_players[i]->iNewPowerupX - list_players[i]->iOldPowerupX) * game_values.swapplayersposition) + list_players[i]->iOldPowerupX;
							iPowerupY = (short)((float)(list_players[i]->iNewPowerupY - list_players[i]->iOldPowerupY) * game_values.swapplayersposition) + list_players[i]->iOldPowerupY;
						}

						spr_storedpowerupsmall.draw(iPowerupX, iPowerupY, storedpowerupid * 16, 0, 16, 16);
					}
				}

				if(game_values.swapstyle == 0)
				{
					if(!sfx_skid.isplaying())
						ifsoundonplay(sfx_skid);
				}

				if(++game_values.swapplayersblinkcount > 10)
				{
					game_values.swapplayersblinkcount = 0;
					game_values.swapplayersblink = !game_values.swapplayersblink;
				}

				game_values.swapplayersposition += 0.02f;
				if(game_values.swapplayersposition >= 1.0f)
				{
					game_values.swapplayersposition = 0.0f;
					game_values.swapplayers = false;
					game_values.screenfade = 0;
					
					if(game_values.swapstyle == 0)
						ifsoundonstop(sfx_skid);

					ifsoundonplay(sfx_transform);

					if(game_values.swapstyle == 1)
					{
						for(i = 0; i < list_players_cnt; i++)
							eyecandyfront.add(new EC_SingleAnimation(&spr_fireballexplosion, list_players[i]->ix + (HALFPW) - 16, list_players[i]->iy + (HALFPH) - 16, 3, 8));
					}
				}
			}
		}
		else
		{
			if(game_values.pausegame)
			{
				spr_dialog.draw(224, 176);
				menu_font_large.drawCentered(320, 240 - (menu_font_large.getHeight() >> 1), "Pause");
			}

			if(game_values.exitinggame)
			{
				spr_dialog.draw(224, 176);
				menu_font_large.drawCentered(320, 220 - (menu_font_large.getHeight() >> 1), "Exit Game");

				spr_dialogbutton.draw(235, 250, 0, (game_values.exityes ? 34 : 0), 80, 34);
				spr_dialogbutton.draw(325, 250, 0, (game_values.exityes ? 0 : 34), 80, 34);
			
				menu_font_large.draw(275 - (menu_font_large.getWidth("Yes") >> 1),  269 - (menu_font_large.getHeight() >> 1), "Yes");
				menu_font_large.draw(365 - (menu_font_large.getWidth("No") >> 1),  269 - (menu_font_large.getHeight() >> 1), "No");
			}
		}

		ticks = SDL_GetTicks() - framestart;
		if(ticks == 0)
			ticks = 1;

		if(game_values.showfps)
		{
			float potentialFps = 1000.0f / (float)(game_values.framelimiter == 0 ? 1 : game_values.framelimiter);
			menu_font_large.drawf(0, 480-menu_font_large.getHeight(), "Actual:%.1f/%.1f, Flip:%.1f, Potential:%.1f", realfps, potentialFps, flipfps, 1000.0f / (float)ticks);
		}

#ifdef _DEBUG
	if(g_fAutoTest)
		menu_font_small.drawRightJustified(635, 5, "Auto");
#endif
 
		//double buffering -> flip buffers
		SDL_Flip(screen);

		flipfps = 1000.0f / (float)ticks;

		
		//Sleep for time just under what we need
		short delay = (short)(game_values.framelimiter - SDL_GetTicks() + framestart - 2);

		if(delay > 0)
		{
			if(delay > game_values.framelimiter)
				delay = game_values.framelimiter;
			
			SDL_Delay(delay);
		}

		//Fine tune wait here
		while(SDL_GetTicks() - framestart < (unsigned short)game_values.framelimiter)
			SDL_Delay(0);   //keep framerate constant at 1000/game_values.framelimiter fps

		//Debug code to slow framerate down to 1 fps to see exact movement
#ifdef _DEBUG
		if(game_values.frameadvance)
		{
			delay = (short)(1000 - SDL_GetTicks() + framestart);

			if(delay > 0)
			{
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

		if(++outputtimer == 60)
		{
			FILE * out = fopen("fps.txt", "a+");

			fprintf(out, "%.2f\n", avgFPS / (float)outputtimer);

			fclose(out);

			avgFPS = 0.0f;
			outputtimer = 0;
		}
		*/
	}

	
	//we never get here

	return;
}

void CleanUp()
{
	short i;
	//delete object list
	for(i = 0; i < list_players_cnt; i++)
	{
		delete list_players[i];
	}
	list_players_cnt = 0;

	eyecandyfront.clean();
	eyecandyback.clean();
	objectsfront.clean();
	objectsplayer.clean();
	objectcollisionitems.clean();
	objectblocks.clean();
	LoadMapObjects();
	g_map.clearWarpLocks();
	g_map.resetPlatforms();

	//Stop all game sounds
	sfx_stopallsounds();
	sfx_invinciblemusic.resetpause();
	sfx_slowdownmusic.resetpause();

	x_shake = 0;
	y_shake = 0;
}

void UpdateScoreBoard()
{
	if(game_values.tour && game_values.gamemode->winningteam > -1)
	{
		//For this game, set the player's place as the type of win
		for(short iScore = 0; iScore < score_cnt; iScore++)
		{
			game_values.tournament_scores[iScore].type[game_values.tourstopcurrent] = score[iScore]->place;
		}

		//Add up all the winnings so far and determine overall place in tour
		for(short iScore = 0; iScore < score_cnt; iScore++)
		{
			game_values.tournament_scores[iScore].total = 0;

			for(short iTourStop = 0; iTourStop <= game_values.tourstopcurrent; iTourStop++)
			{
				game_values.tournament_scores[iScore].total += (3 - game_values.tournament_scores[iScore].type[iTourStop]) * game_values.tourstops[iTourStop]->iPoints;
			}

			game_values.tournament_scores[iScore].wins = 0;
		}

		for(short iMyScore = 0; iMyScore < score_cnt; iMyScore++)
		{
			for(short iTheirScore = 0; iTheirScore < score_cnt; iTheirScore++)
			{
				if(game_values.tournament_scores[iMyScore].total > game_values.tournament_scores[iTheirScore].total)
				{
					game_values.tournament_scores[iTheirScore].wins++;
				}
			}
		}

		if(++game_values.tourstopcurrent >= game_values.tourstoptotal)
		{
			//Calculate Tour Winner by counting up 1st, 2nd, 3rd, and 4th place wins

			short iWinningTeam = -2;  //Set winning team to -2 to signify a tie between teams
			short iWinningScore = 0;

			for(short iScore = 0; iScore < score_cnt; iScore++)
			{
				if(game_values.tournament_scores[iScore].total > iWinningScore)  //New winner found
				{
					iWinningTeam = iScore;
					iWinningScore = game_values.tournament_scores[iScore].total;
				}
				else if(game_values.tournament_scores[iScore].total == iWinningScore)  //Winning position tied
				{
					iWinningTeam = -2;
				}
			}

			game_values.tournamentwinner = iWinningTeam;
			backgroundmusic[4].play(true, true);
		}
	}
	else if(game_values.tournament)
	{
		short maxScore = -1;  //Max score for game
		short maxTeam = -1;  //Team ID with the max score -> reset to -1 if two teams tied for win

		for(short i = 0; i < score_cnt; i++)
		{
			if(score[i]->score > maxScore)
			{
				maxScore = score[i]->score;
				maxTeam = i;
			}
			else if(score[i]->score == maxScore)
			{
				maxTeam = -1;
			}
		}

		if(maxTeam > -1)
		{
			game_values.tournament_scores[maxTeam].type[game_values.tournament_scores[maxTeam].wins] = currentgamemode;

			if(++game_values.tournament_scores[maxTeam].wins >= game_values.tournamentgames)
			{
				game_values.tournamentwinner = maxTeam;
				
				if(game_values.music)
				{
					backgroundmusic[4].play(true, true);
				}
			}
		}
	}
}

void PlayNextMusicTrack()
{
	if(game_values.gamemode->gameover || game_values.playinvinciblesound || sfx_timewarning.isplaying())
		return;

	backgroundmusic[0].stop();
	musiclist.SetNextMusic(g_map.musicCategoryID, maplist.currentShortmapname(), g_map.szBackgroundFile);
	backgroundmusic[0].load(musiclist.GetCurrentMusic());
	backgroundmusic[0].play(game_values.playnextmusic, false);
}

bool coldec_player2player(CPlayer &o1, CPlayer &o2)
{
	//Special cases to deal with players overlapping the right and left sides of the screen

	if(o1.ix + PW < o2.ix)
	{
		if (o1.ix + 640 >= o2.ix + PW || o1.ix + PW + 640 < o2.ix || o1.iy > o2.iy + PH || o1.iy + PH < o2.iy) 
			return false;
		else 
			return true;
	}
	else if(o2.ix + PW < o1.ix)
	{
		if (o1.ix >= o2.ix + PW + 640 || o1.ix + PW < o2.ix + 640 || o1.iy > o2.iy + PH || o1.iy + PH < o2.iy) 
			return false;
		else 
			return true;
	}
	else  //Normal case where no overlap
	{
		if (o1.ix >= o2.ix + PW || o1.ix + PW < o2.ix || o1.iy > o2.iy + PH || o1.iy + PH < o2.iy) 
			return false;
		else 
			return true;
	}
}

bool coldec_player2obj(CPlayer &o1, CObject &o2)
{
	//Special cases to deal with players overlapping the right and left sides of the screen
	if(o1.ix + PW < o2.ix)
	{
		if (o1.ix + 640 >= o2.ix + o2.collisionWidth || o1.ix + PW + 640 < o2.ix || o1.iy >= o2.iy + o2.collisionHeight || o1.iy + PH < o2.iy) 
			return false;
		else 
			return true;
	}
	else if(o2.ix + o2.collisionWidth < o1.ix)
	{
		if (o1.ix >= o2.ix + o2.collisionWidth + 640 || o1.ix + PW < o2.ix + 640 || o1.iy >= o2.iy + o2.collisionHeight || o1.iy + PH < o2.iy) 
			return false;
		else 
			return true;
	}
	else //Normal case where no overlap
	{
		if (o1.ix >= o2.ix + o2.collisionWidth || o2.ix > o1.ix + PW || o1.iy >= o2.iy + o2.collisionHeight || o2.iy > o1.iy + PH) 
			return false;
		else 
			return true;
	}
}

bool coldec_obj2obj(CObject &o1, CObject &o2)
{
	//Special cases to deal with players overlapping the right and left sides of the screen
	short o1r = o1.ix + o1.collisionWidth;
	short o1b = o1.iy + o1.collisionHeight;
	short o2r = o2.ix + o2.collisionWidth;
	short o2b = o2.iy + o2.collisionHeight;

	if(o1r < o2.ix)
	{
		if (o1.ix + 640 >= o2r || o1r + 640 < o2.ix || o1.iy >= o2b || o1b < o2.iy) 
			return false;
		else 
			return true;
	}
	else if(o2r < o1.ix)
	{
		if (o1.ix >= o2r + 640 || o1r < o2.ix + 640 || o1.iy >= o2b || o1b < o2.iy)
			return false;
		else 
			return true;
	}
	else
	{
		if (o1.ix >= o2r || o2.ix > o1r || o1.iy >= o2b || o2.iy > o1b) 
			return false;
		else 
			return true;
	}
}

void LoadMapObjects()
{
	objectblocks.clean();

	for(short iSwitch = 0; iSwitch < 8; iSwitch++)
		g_map.switchBlocks[iSwitch].clear();
	
	for(short x = 0; x < MAPWIDTH; x++)
	{
		for(short y = 0; y < MAPHEIGHT; y++)
		{
			if(g_map.objectdata[x][y] == 0)
			{
				g_map.blockdata[x][y] = new B_BreakableBlock(&spr_breakableblock, x * TILESIZE, y * TILESIZE, 4, 10);
				objectblocks.add(g_map.blockdata[x][y]);
			}
			else if(g_map.objectdata[x][y] == 1)
			{
				g_map.blockdata[x][y] = new B_PowerupBlock(&spr_powerupblock, x * TILESIZE, y * TILESIZE, 4, 10);
				objectblocks.add(g_map.blockdata[x][y]);
			}
			else if(g_map.objectdata[x][y] == 2)
			{
				g_map.blockdata[x][y] = new B_DonutBlock(&spr_donutblock, x * TILESIZE, y * TILESIZE);
				objectblocks.add(g_map.blockdata[x][y]);
			}
			else if(g_map.objectdata[x][y] == 3)
			{
				g_map.blockdata[x][y] = new B_FlipBlock(&spr_flipblock, x * TILESIZE, y * TILESIZE);
				objectblocks.add(g_map.blockdata[x][y]);
			}
			else if(g_map.objectdata[x][y] == 4)
			{
				g_map.blockdata[x][y] = new B_BounceBlock(&spr_bounceblock, x * TILESIZE, y * TILESIZE);
				objectblocks.add(g_map.blockdata[x][y]);
			}
			else if(g_map.objectdata[x][y] == 5)
			{
				g_map.blockdata[x][y] = new B_NoteBlock(&spr_noteblock, x * TILESIZE, y * TILESIZE, 4, 10);
				objectblocks.add(g_map.blockdata[x][y]);
			}
			else if(g_map.objectdata[x][y] == 6)
			{
				g_map.blockdata[x][y] = new B_ThrowBlock(&spr_throwblock, x * TILESIZE, y * TILESIZE, 4, 10);
				objectblocks.add(g_map.blockdata[x][y]);
			}
			else if(g_map.objectdata[x][y] >= 7 && g_map.objectdata[x][y] <= 10)
			{
				short iSwitchType = g_map.objectdata[x][y] - 7;
				g_map.blockdata[x][y] = new B_OnOffSwitchBlock(&spr_switchblocks, x * TILESIZE, y * TILESIZE, iSwitchType, g_map.iSwitches[iSwitchType]);
				objectblocks.add(g_map.blockdata[x][y]);
				g_map.switchBlocks[iSwitchType].push_back(g_map.blockdata[x][y]);
			}
			else if(g_map.objectdata[x][y] >= 11 && g_map.objectdata[x][y] <= 14)
			{
				short iSwitchType = g_map.objectdata[x][y] - 11;

				g_map.blockdata[x][y] = new B_SwitchBlock(&spr_switchblocks, x * TILESIZE, y * TILESIZE, iSwitchType, g_map.iSwitches[iSwitchType]);
				objectblocks.add(g_map.blockdata[x][y]);
				g_map.switchBlocks[iSwitchType + 4].push_back(g_map.blockdata[x][y]);
			}
			else
			{
				g_map.blockdata[x][y] = NULL;
			}
		}
	}
}

bool SwapPlayers(short iUsingPlayerID)
{
	//Count available players to switch with
	short iNumAvailablePlayers = 0;
	for(short iPlayer = 0; iPlayer < list_players_cnt; iPlayer++)
	{
		if(list_players[iPlayer]->isready())
		{
			iNumAvailablePlayers++;
			list_players[iPlayer]->fOldSwapX = list_players[iPlayer]->ix;
			list_players[iPlayer]->fOldSwapY = list_players[iPlayer]->iy;

			list_players[iPlayer]->iNewPowerupX = list_players[iPlayer]->score->x + scorepowerupoffsets[game_values.teamcounts[list_players[iPlayer]->teamID] - 1][list_players[iPlayer]->subTeamID];
			list_players[iPlayer]->iNewPowerupY = list_players[iPlayer]->score->y + 25;
		}
	}

	if(iNumAvailablePlayers <= 1)
		return false;

	if(game_values.swapstyle != 2)
	{
		game_values.swapplayers = true;
		game_values.swapplayersposition = 0.0f;
		
		if(game_values.swapstyle == 1)
		{
			game_values.swapplayersblink = false;
			game_values.swapplayersblinkcount = 0;
		}
		else
		{
			game_values.screenfade = GetScreenBackgroundFade();
		}
	}

	short iIncrement = rand() % (iNumAvailablePlayers - 1);

	MysteryMushroomTempPlayer spots[4];
	for(short iPlayer = 0; iPlayer < list_players_cnt; iPlayer++)
	{
		if(!list_players[iPlayer]->isready())
			continue;

		short iNewSpot = iPlayer + iIncrement;
		
		do
		{
			if(++iNewSpot >= list_players_cnt)
				iNewSpot = 0;
		}
		while(spots[iNewSpot].fUsed || !list_players[iNewSpot]->isready());

		spots[iNewSpot].fUsed = true;
		spots[iPlayer].SetPlayer(list_players[iNewSpot], game_values.gamepowerups[list_players[iNewSpot]->getGlobalID()]);

		//Give credit for deaths to the player that used the mystery mushroom
		if(iUsingPlayerID == iNewSpot)
		{
			list_players[iPlayer]->pSuicideCreditPlayer = list_players[iNewSpot];
			list_players[iPlayer]->iSuicideCreditTimer = 62;
		}
	}

	for(short iPlayer = 0; iPlayer < list_players_cnt; iPlayer++)
	{
		if(!list_players[iPlayer]->isready())
			continue;

		spots[iPlayer].GetPlayer(list_players[iPlayer], &game_values.gamepowerups[list_players[iPlayer]->getGlobalID()]);
		
		if(game_values.swapstyle != 1)
			eyecandyfront.add(new EC_SingleAnimation(&spr_fireballexplosion, (short)list_players[iPlayer]->fNewSwapX + (HALFPW) - 16, (short)list_players[iPlayer]->fNewSwapY + (HALFPH) - 16, 3, 8));

		if(game_values.swapstyle == 2)
		{
			list_players[iPlayer]->xf(list_players[iPlayer]->fNewSwapX);
			list_players[iPlayer]->yf(list_players[iPlayer]->fNewSwapY);

			if(list_players[iPlayer]->carriedItem)
				list_players[iPlayer]->carriedItem->MoveToOwner();
		}
	}

	//Clean the dead mystery mushroom
	objectcollisionitems.cleandeadobjects();	

	return true;
}
