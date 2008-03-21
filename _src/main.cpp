/*----------------------------------------------------------+
| super mario war											|
|															|
| a mario war clone written using the tile based collision	|
| detection technique explained in jnrdev #1				|
|															|
| you can read the tutorial on http://jnrdev.72dpiarmy.net	|
|															|
|															|
| this sourcecode is released under the GPLv2.				|
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
| last changes:	03.26.2007									|
|															|
|								© 2003-2008 Florian Hufsky  |
|								  florian.hufsky@gmail.com	|
|                                     mtschaffer@gmail.com  |
|								  http://smw.72dpiarmy.com	|
+----------------------------------------------------------*/

//TODO
//BUG!! Still reports of disappearing map tiles - caused when rRects is used out of bounds causing w and h to be set to 0 - happened with platform with tile using row 960
//      I think this was due to using old maps with newer versions of the 1.8 alpha - keep an eye on this, but it might be a non-issue

/*
//BUG!! Throwing flags into flag bases and probably eggs to yoshi doens't work anymore because there is no collision detection between those items
//      I was never a big fan of this feature, we'll see if anyone complains

//BUG!! When you kill 2 players/bots rapidly one after another with the star and you have the announcer on, the invincibilty music stops.

//[ ] Frenzy mode doesn't have update to items - missing wand, podobo, bomb, pwings, leaf, tanooki
//[ ] Need better end stage marker gfx
//[ ] Need match type titles on gfx
//[ ] Need to test memory/slowness on xbox build
//[ ] Need to update some maps with better platform paths (Reznor map) and some hazards here and there and maybe some map items
//[ ] World AI needs ability to use stored items -> harder problem than I have time for
//[ ] Add SMB3's first world as a test world to ship with

//[ ] Option to make the shield a "soft" shield and allow the player to not be able to kill (stomp) when shielded - lots of requests for this
//[ ] Announcement that a player/team was removed from game - needs more specing but this could be helpful
//[ ] Countdown before match begins, 3..., 2..., 1... with announcer hooks
//[ ] Better eyecandy block for 10+ kills like stars - need gfx
//[ ] On/Off switch [?] and note blocks and possibly other types of on/off blocks
//[ ] Enemies should die in lava
//[ ] Quick Match option that selects everything random for you
//[ ] Allow spawn areas for domination blocks
//[ ] Water bubbles and rain eyecandy, Splashing feet when running on rain levels - need gfx for all of these
//[ ] Resume level editor to zzleveleditor map every time so it looks like you've resumed your last work (or try and load the last map you were on)
//[ ] "Random" and "MultiStars" modes for star mode
//[ ] Podoboo pops out of a [?] if all powerups are turned off
//[ ] ? card for frenzy card mode to do random powerup
//[ ] Thunderbolt style spawn
//[ ] Reverse gravity blue podoboos
//[ ] Breaking skull blocks (too close to donut block, perhaps)

* Phanto Mode (there is a key and whoever has the key is chased by a Phanto)
- Hammers or something arc-pathed in Survival
* Bomb option in Star mode
* Multiple selectable item switch presets
- Mariokart-type podium at the end of a tournament/tour/whatever

*/

/*
Checkin:
*/

#ifdef _XBOX
	#include <xtl.h>
#endif

#include "global.h"				//all the global stuff
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

//------ sprites (maybe this should be done in a resource manger) ------
gfxSprite		** spr_player[4];	//all player sprites (see global.h)
gfxSprite		** spr_chocobo[4];
gfxSprite		** spr_bobomb[4];
gfxSprite		spr_clouds;
gfxSprite		spr_ghosts;
gfxSprite		spr_fish;
gfxSprite		spr_leaves;
gfxSprite		spr_snow;
gfxSprite		spr_background;
gfxSprite		spr_backmap[2];
gfxSprite		spr_frontmap[2];
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
gfxSprite		menu_match_select;
gfxSprite		menu_dialog;
gfxSprite		menu_slider_bar;

gfxSprite		menu_stomp;
gfxSprite		menu_survival;
gfxSprite		menu_egg;

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
gfxSprite		spr_thumbnail_mapitems[2];
gfxSprite		spr_platformstarttile;
gfxSprite		spr_platformendtile;
gfxSprite		spr_platformpath;

gfxSprite		spr_worldbackground[3];
gfxSprite		spr_worldforeground[3];
gfxSprite		spr_worldforegroundspecial[3];
gfxSprite		spr_worldpaths[3];
gfxSprite		spr_worldvehicle[3];

gfxSprite		spr_worlditems;
gfxSprite		spr_worlditempopup;
gfxSprite		spr_worlditemssmall;
gfxSprite		spr_worlditemsplace;
gfxSprite		spr_worldbonushouse;

gfxSprite		spr_noteblock;
gfxSprite		spr_breakableblock;
gfxSprite		spr_powerupblock;
gfxSprite		spr_donutblock;
gfxSprite		spr_flipblock;
gfxSprite		spr_bounceblock;
gfxSprite		spr_throwblock;
gfxSprite		spr_switchblocks;
gfxSprite		spr_viewblock;
gfxSprite		spr_weaponbreakableblock;

gfxSprite		spr_brokenyellowblock;
gfxSprite		spr_brokenflipblock;
gfxSprite		spr_brokenblueblock;

gfxSprite		spr_brokeniceblock;
gfxSprite		spr_iceblock;

gfxSprite		spr_tileanimation[3];
gfxSprite		spr_blocks[3];
gfxSprite		spr_unknowntile[3];

gfxSprite		spr_starpowerup;
gfxSprite		spr_1uppowerup;
gfxSprite		spr_2uppowerup;
gfxSprite		spr_3uppowerup;
gfxSprite		spr_5uppowerup;
gfxSprite		spr_firepowerup;
gfxSprite		spr_hammerpowerup;
gfxSprite		spr_icewandpowerup;
gfxSprite		spr_podobopowerup;
gfxSprite		spr_poisonpowerup;
gfxSprite		spr_mysterymushroompowerup;
gfxSprite		spr_boomerangpowerup;
gfxSprite		spr_clockpowerup;
gfxSprite		spr_bobombpowerup;
gfxSprite		spr_powpowerup;
gfxSprite		spr_modpowerup;
gfxSprite		spr_bulletbillpowerup;
gfxSprite		spr_featherpowerup;
gfxSprite		spr_leafpowerup;
gfxSprite		spr_bombpowerup;
gfxSprite		spr_pwingspowerup;
gfxSprite       spr_tanooki, spr_statue;
gfxSprite		spr_extraheartpowerup;
gfxSprite		spr_extratimepowerup;
gfxSprite		spr_jailkeypowerup;

gfxSprite		spr_shade[3];
gfxSprite		spr_scorehearts;
gfxSprite		spr_scorecards;

gfxSprite		spr_timershade;
gfxSprite		spr_spawneggs;
gfxSprite		spr_scoretext;
gfxSprite		spr_racetext;
gfxSprite		spr_crown;
gfxSprite		spr_warplock;
gfxSprite		spr_cape;
gfxSprite		spr_tail;
gfxSprite		spr_wings;
gfxSprite		spr_coinsparkle;
gfxSprite		spr_shinesparkle;
gfxSprite		spr_shellbounce;
gfxSprite		spr_superstomp;

gfxSprite		spr_coin;
gfxSprite		spr_egg;
gfxSprite		spr_eggnumbers;
gfxSprite		spr_star;
gfxSprite		spr_frenzycards;
gfxSprite		spr_collectcards;
gfxSprite		spr_flags;
gfxSprite		spr_yoshi;
gfxSprite		spr_thwomp;
gfxSprite		spr_podobo;
gfxSprite		spr_bowserfire;
gfxSprite		spr_areas;
gfxSprite		spr_kingofthehillarea;
gfxSprite		spr_jail;
gfxSprite		spr_racegoal;
gfxSprite		spr_pipegamebonus;
gfxSprite		spr_chicken;

gfxSprite		spr_bonuschest;
gfxSprite		spr_teleportstar;

gfxSprite		spr_goomba;
gfxSprite		spr_goombadead;
gfxSprite		spr_goombadeadflying;
gfxSprite		spr_koopa;
gfxSprite		spr_buzzybeetle;
gfxSprite		spr_spiny;
gfxSprite		spr_paragoomba;
gfxSprite		spr_parakoopa;
gfxSprite		spr_redparakoopa;
//gfxSprite		spr_sledgebrothers;
//gfxSprite		spr_sledgebrothersdead;
gfxSprite		spr_redkoopa;
gfxSprite		spr_cheepcheep;
gfxSprite		spr_cheepcheepdead;
gfxSprite		spr_bulletbill;
gfxSprite		spr_bulletbilldead;

gfxSprite		spr_fireball;
//gfxSprite		spr_superfireball;
gfxSprite		spr_hammer;
gfxSprite		spr_iceblast;
gfxSprite		spr_boomerang;
gfxSprite		spr_shell;
gfxSprite		spr_shelldead;
gfxSprite		spr_blueblock;
gfxSprite		spr_spring;
gfxSprite		spr_spike;
gfxSprite		spr_bomb;
gfxSprite		spr_kuriboshoe;

gfxSprite		spr_hazard_fireball[3];
gfxSprite		spr_hazard_rotodisc[3];
gfxSprite		spr_hazard_bulletbill[3];
gfxSprite		spr_hazard_bulletbilldead;
gfxSprite		spr_hazard_flame[3];
gfxSprite		spr_hazard_pirhanaplant[3];

gfxSprite		spr_fireballexplosion;
gfxSprite		spr_frictionsmoke;
gfxSprite		spr_bobombsmoke;
gfxSprite		spr_explosion;
gfxSprite		spr_burnup;
gfxSprite		spr_fireworks;
gfxSprite		spr_poof;

gfxSprite		spr_spawnsmoke;
gfxSprite		spr_spawndoor;
gfxSprite		spr_bonus;
gfxSprite		spr_extralife;
gfxSprite		spr_award;
gfxSprite		spr_awardsolid;
gfxSprite		spr_awardsouls;
gfxSprite		spr_awardsoulspawn;
gfxSprite		spr_awardkillsinrow;
gfxSprite		spr_flagbases;
gfxSprite		spr_ownedtags;
gfxSprite		spr_phanto, spr_phantokey;

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
short			respawnanimationtimer[4] = {0, 0, 0, 0};
short			respawnanimationframe[4] = {0, 0, 0, 0};

short			projectiles[4];

extern short controlkeys[2][2][4][NUM_KEYS];
extern short g_iVersion[];

//Locations for swirl spawn effects
short g_iSwirlSpawnLocations[4][2][25];

CMap			g_map;
CTilesetManager g_tilesetmanager;

CEyecandyContainer eyecandyback;
CEyecandyContainer eyecandyfront;

CObjectContainer noncolcontainer;
CObjectContainer objectcontainer[3];

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
sfxSound sfx_gameover;
sfxSound sfx_stun;
sfxSound sfx_inventory;
sfxSound sfx_worldmove;
sfxSound sfx_treasurechest;
sfxSound sfx_flamecannon;
sfxSound sfx_wand;

sfxMusic backgroundmusic[6];

CGameMode			*gamemodes[GAMEMODE_LAST];
CGM_Bonus			*bonushousemode = NULL;
CGM_Pipe_MiniGame	*pipegamemode = NULL;

short		currentgamemode = 0;

short g_iWinningPlayer;

extern short g_iPowerupToIcon[6];

extern void SetupScoreBoard(bool fOrderMatters);
extern void ShowScoreBoard();

FiltersList filterslist;  //Filters list must be initiallized before maps list because it is used in maplist constructor
MapList maplist;
SkinList skinlist;
AnnouncerList announcerlist;
MusicList musiclist;
WorldMusicList worldmusiclist;
GraphicsList menugraphicspacklist;
GraphicsList gamegraphicspacklist;
SoundsList soundpacklist;
TourList tourlist;
WorldList worldlist;
WorldMap g_worldmap;

//Network stuff
int g_iNextNetworkID = 0;
int g_iNextMessageID = 0;
char szIPString[32] = "";

//NetServer netServer;
//NetClient netClient;

extern Uint8 GetScreenBackgroundFade();

extern short g_iCollisionMap[MOVINGOBJECT_LAST][MOVINGOBJECT_LAST];

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
void collisionhandler_o2o(IO_MovingObject * o1, IO_MovingObject * o2)
{
	o2->collide(o1);
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
		if(!game_values.gamemode->gameover && CountAliveTeams(&lastteam) <= 1 && game_values.gamemode->gamemode != game_mode_bonus)
		{
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
			backgroundmusic[0].load(musiclist.GetCurrentMusic()); //In Game Music
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
void SetGameModeSettingsFromMenu();
void LoadMapObjects(bool fPreview);
void LoadMapHazards(bool fPreview);
void UpdateScoreBoard();
void PlayNextMusicTrack();
//void EnterBossMode(short type);
bool IsExitAllowed();
bool IsPauseAllowed();

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
	game_values.suicidetime			= 310;	// 5 seconds
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
	game_values.playnextmusic		= false;
	game_values.pointspeed			= 20;
	game_values.swapstyle			= 1;	//Blink then swap
	game_values.worldpointsbonus	= -1; //no world multiplier until player uses item to boost it
	game_values.singleplayermode	= -1;
	game_values.worldskipscoreboard = false;
	game_values.overridepowerupsettings = 0;
	game_values.minigameunlocked	= false;

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
	worldmusiclist.SetCurrent(0);
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
	gamemodes[17] = new CGM_Greed();
	gamemodes[18] = new CGM_Health();
	gamemodes[19] = new CGM_Collection();
	gamemodes[20] = new CGM_Chase();

	currentgamemode = 0;
	game_values.gamemode = gamemodes[currentgamemode];

	//Special modes
	bonushousemode = new CGM_Bonus();
	pipegamemode = new CGM_Pipe_MiniGame();

	//Setup the default game mode settings
	//Classic
	game_values.gamemodemenusettings.classic.style = 0;		//Respawn on death

	//Frag
	game_values.gamemodemenusettings.frag.style = 0;		//Respawn on death
	game_values.gamemodemenusettings.frag.scoring = 0;		//All kills will score

	//Time Limit
	game_values.gamemodemenusettings.time.style = 0;		//Respawn on death
	game_values.gamemodemenusettings.time.scoring = 0;		//All kills will score
	game_values.gamemodemenusettings.time.percentextratime = 10;	//10% chance of a heart spawning

	//Jail
	game_values.gamemodemenusettings.jail.style = 1;			//defaults to color jail play
	game_values.gamemodemenusettings.jail.tagfree = true;		//players on same team can free player by touching
	game_values.gamemodemenusettings.jail.timetofree = 1240;   //20 seconds of jail
	game_values.gamemodemenusettings.jail.percentkey = 30;		//30% chance of a key spawning

	//Coins
	game_values.gamemodemenusettings.coins.penalty = false;		//no penalty for getting stomped
	game_values.gamemodemenusettings.coins.quantity = 1;		//only 1 coin on screen

	//Stomp
	game_values.gamemodemenusettings.stomp.rate = 90; //Moderate
	game_values.gamemodemenusettings.stomp.enemyweight[0] = 4; // turn on goombas, koopa and cheep cheeps by default
	game_values.gamemodemenusettings.stomp.enemyweight[1] = 4;  
	game_values.gamemodemenusettings.stomp.enemyweight[2] = 6;
	game_values.gamemodemenusettings.stomp.enemyweight[3] = 2;
	game_values.gamemodemenusettings.stomp.enemyweight[4] = 2;
	game_values.gamemodemenusettings.stomp.enemyweight[5] = 4;
	game_values.gamemodemenusettings.stomp.enemyweight[6] = 1;
	game_values.gamemodemenusettings.stomp.enemyweight[7] = 1;
	game_values.gamemodemenusettings.stomp.enemyweight[8] = 1;

	//Eggs
	game_values.gamemodemenusettings.egg.eggs[0] = 0;
	game_values.gamemodemenusettings.egg.eggs[1] = 1;
	game_values.gamemodemenusettings.egg.eggs[2] = 0;
	game_values.gamemodemenusettings.egg.eggs[3] = 0;
	game_values.gamemodemenusettings.egg.yoshis[0] = 0;
	game_values.gamemodemenusettings.egg.yoshis[1] = 1;
	game_values.gamemodemenusettings.egg.yoshis[2] = 0;
	game_values.gamemodemenusettings.egg.yoshis[3] = 0;
	game_values.gamemodemenusettings.egg.explode = 0;  //Exploding eggs is turned off by default

	//Capture The Flag
	game_values.gamemodemenusettings.flag.speed = 0;  //Bases don't move by default
	game_values.gamemodemenusettings.flag.touchreturn = false;  //Don't return by touching
	game_values.gamemodemenusettings.flag.pointmove = true;  //Move base after point
	game_values.gamemodemenusettings.flag.autoreturn = 1240;  //Return flag automatically after 20 seconds
	game_values.gamemodemenusettings.flag.homescore = false;  //Don't require flag to be home to score
	game_values.gamemodemenusettings.flag.centerflag = false; //Do normal CTF, not center flag style

	//Chicken
	game_values.gamemodemenusettings.chicken.usetarget = true;  //default to displaying a target around the chicken
	game_values.gamemodemenusettings.chicken.glide = false;		//don't give the chicken the ability to glide

	//Tag
	game_values.gamemodemenusettings.tag.tagontouch = true;  //default to transfer tag on touching other players

	//Star
	game_values.gamemodemenusettings.star.time = 30;				//default to 30 seconds
	game_values.gamemodemenusettings.star.shine = 0;				//default to hot potato (ztar)
	game_values.gamemodemenusettings.star.percentextratime = 10;	//10 percent chance of an extra time poweurp spawning

	//Domination
	game_values.gamemodemenusettings.domination.loseondeath = true;
	game_values.gamemodemenusettings.domination.stealondeath = false;
	game_values.gamemodemenusettings.domination.relocateondeath = false;
	game_values.gamemodemenusettings.domination.quantity = 13; //# Players + 1 = 13
	game_values.gamemodemenusettings.domination.relocationfrequency = 1240;  //Relocate after 20 seconds = 1240
	
	//King Of The Hill
	game_values.gamemodemenusettings.kingofthehill.areasize = 3;
	game_values.gamemodemenusettings.kingofthehill.relocationfrequency = 1240;
	game_values.gamemodemenusettings.kingofthehill.maxmultiplier = 1;	//No multiplier

	//Race
	game_values.gamemodemenusettings.race.quantity = 4;
	game_values.gamemodemenusettings.race.speed = 4;
	game_values.gamemodemenusettings.race.penalty = 2;  //0 == none, 1 = 1 base, 2 = all bases lost on death
		
	//Frenzy
	game_values.gamemodemenusettings.frenzy.quantity = 6; //#players - 1
	game_values.gamemodemenusettings.frenzy.rate = 186; //3 seconds
	game_values.gamemodemenusettings.frenzy.storedshells = true; //Shells are stored by default
	game_values.gamemodemenusettings.frenzy.powerupweight[0] = 0;
	game_values.gamemodemenusettings.frenzy.powerupweight[1] = 1;  // turn on flowers and hammers by default
	game_values.gamemodemenusettings.frenzy.powerupweight[2] = 1;
	game_values.gamemodemenusettings.frenzy.powerupweight[3] = 0;
	game_values.gamemodemenusettings.frenzy.powerupweight[4] = 0;
	game_values.gamemodemenusettings.frenzy.powerupweight[5] = 0;
	game_values.gamemodemenusettings.frenzy.powerupweight[6] = 0;
	game_values.gamemodemenusettings.frenzy.powerupweight[7] = 0;
	game_values.gamemodemenusettings.frenzy.powerupweight[8] = 0;
	game_values.gamemodemenusettings.frenzy.powerupweight[9] = 0;
	game_values.gamemodemenusettings.frenzy.powerupweight[10] = 0;
	game_values.gamemodemenusettings.frenzy.powerupweight[11] = 0;

	//Survival
	game_values.gamemodemenusettings.survival.enemyweight[0] = 1;
	game_values.gamemodemenusettings.survival.enemyweight[1] = 0;
	game_values.gamemodemenusettings.survival.enemyweight[2] = 0;
	game_values.gamemodemenusettings.survival.density = 20;
	game_values.gamemodemenusettings.survival.speed = 4;
	game_values.gamemodemenusettings.survival.shield = true;
	
	//Greed
	game_values.gamemodemenusettings.greed.coinlife = 124;			//Coins disappear after 2 seconds
	
	//Health
	game_values.gamemodemenusettings.health.startlife = 6;			//Start with 3 whole hearts (each increment is a half heart)
	game_values.gamemodemenusettings.health.maxlife = 10;			//Maximum of 5 hearts
	game_values.gamemodemenusettings.health.percentextralife = 20;	//20% chance of a heart spawning

	//Card Collection
	game_values.gamemodemenusettings.collection.quantity = 6;		//#players - 1
	game_values.gamemodemenusettings.collection.rate = 186;			//3 seconds to spawn
	game_values.gamemodemenusettings.collection.banktime = 310;		//5 seconds to bank
	
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

			unsigned char abyte[30];
			fread(abyte, sizeof(unsigned char), 30, fp);
			game_values.spawnstyle = (short) abyte[0];
			game_values.awardstyle = (short) abyte[1];
			game_values.teamcollision = (short)abyte[3];
			game_values.screencrunch = ((short)abyte[4] > 0 ? true : false);
			game_values.toplayer = ((short)abyte[5] > 0 ? true : false);
			game_values.scoreboardstyle = (short)abyte[6];
			game_values.teamcolors = ((short)abyte[7] > 0 ? true : false);
			game_values.sound = ((short)abyte[8] > 0 ? true : false);
			game_values.music = ((short)abyte[9] > 0 ? true : false);
			game_values.musicvolume = (short)abyte[10];
			game_values.soundvolume = (short)abyte[11];
			game_values.respawn = (short)abyte[12];
			game_values.outofboundstime = (short)abyte[15];
			game_values.cpudifficulty = (short)abyte[16];
			game_values.framelimiter = (short)abyte[19];
			game_values.bonuswheel = (short)abyte[20];
			game_values.keeppowerup = ((short)abyte[21] > 0 ? true : false);
			game_values.showwinningcrown = ((short)abyte[22] > 0 ? true : false);
			game_values.playnextmusic = ((short)abyte[23] > 0 ? true : false);
			game_values.pointspeed = (short)abyte[24];
			game_values.swapstyle = (short)abyte[25];
			game_values.overridepowerupsettings = (short)abyte[28];
			game_values.minigameunlocked = ((short)abyte[29] > 0 ? true : false);
			
			fread(&game_values.shieldtime, sizeof(short), 1, fp);
			fread(&game_values.shieldstyle, sizeof(short), 1, fp);
			fread(&game_values.itemrespawntime, sizeof(short), 1, fp);
			fread(&game_values.hiddenblockrespawn, sizeof(short), 1, fp);
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
			fread(&game_values.leaflimit, sizeof(short), 1, fp);
			fread(&game_values.pwingslimit, sizeof(short), 1, fp);
			fread(&game_values.tanookilimit, sizeof(short), 1, fp);
			fread(&game_values.bombslimit, sizeof(short), 1, fp);
			fread(&game_values.wandfreezetime, sizeof(short), 1, fp);
			fread(&game_values.wandlimit, sizeof(short), 1, fp);
			fread(&game_values.shellttl, sizeof(short), 1, fp);
			fread(&game_values.blueblockttl, sizeof(short), 1, fp);
			fread(&game_values.redblockttl, sizeof(short), 1, fp);
			fread(&game_values.grayblockttl, sizeof(short), 1, fp);
			fread(&game_values.storedpowerupdelay, sizeof(short), 1, fp);
			fread(&game_values.warplockstyle, sizeof(short), 1, fp);
			fread(&game_values.warplocktime, sizeof(short), 1, fp);
			fread(&game_values.suicidetime, sizeof(short), 1, fp);

			//TODO: Need to test what happens when you unplug some controllers from the xbox
			//and then start up (device index will probably point to a gamepad that isn't in the list)
			//and this will cause a crash
			fread(game_values.inputConfiguration, sizeof(CInputPlayerControl), 8, fp);

			//setup player input controls for game
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

			fread(&game_values.gamemodemenusettings, sizeof(GameModeSettings), 1, fp);

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
			worldmusiclist.SetCurrent((short) abyte[14]);
			menugraphicspacklist.SetCurrent((short) abyte[17]);
			gamegraphicspacklist.SetCurrent((short) abyte[26]);
			soundpacklist.SetCurrent((short) abyte[18]);

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

	//Calculate the swirl spawn effect locations
	float spawnradius = 100.0f;
	float spawnangle = 0.0f;
	
	for(short i = 0; i < 25; i++)
	{
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

bool coldec_player2player(CPlayer * o1, CPlayer * o2);
bool coldec_player2obj(CPlayer * o1, CObject * o2);
bool coldec_obj2obj(CObject * o1, CObject * o2);

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

	//Create players for this game
	for(short iPlayer = 0; iPlayer < 4; iPlayer++)
	{
		projectiles[iPlayer] = 0;
		respawn[iPlayer] = 0;

		if(game_values.singleplayermode == -1 || game_values.singleplayermode == iPlayer)
		{
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
				//Reset off player's stored powerups if they are not playing
				game_values.storedpowerups[iPlayer] = -1;
			}
		}

		if(game_values.gamemode->HasStoredPowerups())
			game_values.gamepowerups[iPlayer] = game_values.storedpowerups[iPlayer];
		else
			game_values.gamepowerups[iPlayer] = -1;

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

	//Initialize game mode
	game_values.gamemode->init();

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

	//Clouds
	if(g_map.eyecandyID & 1)
	{
		for(i = 0; i < 4; i++)
		{
			float velx;			//speed of cloud, small clouds are slower than big ones
			short srcy, w, h;

			if(rand() % 2)
			{
				velx = (short)(rand() % 51 - 25) / 10.0f;	//big clouds: -3 - +3 pixel/frame
				srcy = 0;
				w = 60;
				h = 28;
			}
			else
			{
				velx = (short)(rand() % 41 - 20) / 10.0f;	//small clouds: -2 - +2 pixel/frame
				srcy = 28;
				w = 28;
				h = 12;
			}
			
			velx = velx < 0.5f && velx > -0.5f ? 1 : velx;	//no static clouds please

			//add cloud to eyecandy array
			eyecandyfront.add(new EC_Cloud(&spr_clouds, (float)(rand()%640), (float)(rand()%100), velx, 0, srcy, w, h));
		}
	}
	
	//Ghosts
	if(g_map.eyecandyID & 2)
	{
		for(i = 0; i < 12; i++)
		{
			short iGhostSrcY = (short)(rand() % 3) << 5;	//ghost type
			float velx = (short)(rand() % 51 - 25) / 10.0f;	//big clouds: -3 - +3 pixel/frame
			
			velx = velx < 0.5f && velx > -0.5f ? (rand() % 1 ? 1.0f : -1.0f) : velx;	//no static clouds please

			//add cloud to eyecandy array
			eyecandyfront.add(new EC_Ghost(&spr_ghosts, (float)(rand() % 640), (float)(rand() % 100), velx, 8, 2, velx < 0.0f ? 64 : 0, iGhostSrcY, 32, 32));
		}
	}

	//Leaves
	if(g_map.eyecandyID & 4)
	{
		for(i = 0; i < 20; i++)
			eyecandyfront.add(new EC_Leaf(&spr_leaves, (float)(rand() % 640), (float)(rand() % 480)));
	}

	//Snow
	if(g_map.eyecandyID & 8)
	{
		for(i = 0; i < 20; i++)
			eyecandyfront.add(new EC_Snow(&spr_snow, (float)(rand() % 640), (float)(rand() % 480)));
	}

	//Fish
	short iFishWeights[] = {20, 20, 15, 10, 10, 5, 10, 10};
	short iFishSettings[][4] = { {0, 0, 64, 44}, {0, 44, 64, 44}, {0, 44, 48, 44}, {32, 32, 16, 12}, {32, 44, 16, 12}, {32, 16, 16, 28}, {32, 0, 32, 28}, {32, 44, 32, 28}}; 
	if(g_map.eyecandyID & 16)
	{
		for(i = 0; i < 10; i++)
		{
			float velx = (short)(rand() % 41 - 20) / 10.0f;
			velx = velx < 0.5f && velx > -0.5f ? 1.0f : velx; //Keep fish from moving too slowly

			short srcx, srcy, w, h;

			short iRandomFish = rand() % 100;
			
			short iFishWeightCount = 0;
			for(short iFish = 0; iFish < 8; iFish++)
			{
				iFishWeightCount += iFishWeights[iFish];

				if(iRandomFish < iFishWeightCount)
				{
					srcx = iFishSettings[iFish][0];
					srcy = iFishSettings[iFish][1];
					w = iFishSettings[iFish][2];
					h = iFishSettings[iFish][3];
					break;
				}
			}

			//add cloud to eyecandy array
			short iPossibleY = (480 - h) / 10;
			float dDestY = (float)(rand() % iPossibleY + iPossibleY * i);
			eyecandyfront.add(new EC_Cloud(&spr_fish, (float)(rand()%640), dDestY, velx, srcx + (velx > 0.0f ? 64 : 0), srcy, w, h));
		}
	}
	
	short iScoreTextOffset[4];
	for(short iTeam = 0; iTeam < score_cnt; iTeam++)
	{
		iScoreTextOffset[iTeam] = 34 * game_values.teamcounts[iTeam] + 1;
	}


	short iWindTimer = 0;
	float dNextWind = (float)((rand() % 41) - 20) / 4.0f;
	game_values.gamewindx = (float)((rand() % 41) - 20) / 4.0f;

	while (true)
	{
		framestart = SDL_GetTicks();

		if(iWindTimer == 0)
		{
			//Then trigger next wind event
			if(game_values.gamewindx < dNextWind)
			{
				game_values.gamewindx += 0.02f;

				if(game_values.gamewindx >= dNextWind)
					iWindTimer = (rand() % 60) + 30;
			}
			if(game_values.gamewindx > dNextWind)
			{
				game_values.gamewindx -= 0.02f;

				if(game_values.gamewindx <= dNextWind)
					iWindTimer = (rand() % 60) + 30;
			}
		}
		else
		{
			if(--iWindTimer <= 0)
			{
				dNextWind = (float)((rand() % 41) - 20) / 4.0f;
			}
		}

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

					int tile = tile_flag_nonsolid;
					IO_Block * block = NULL;
					short blocktype = -1;

					if(list_players[k]->iy + HALFPH >= 0 && list_players[k]->iy + HALFPH < 480)
					{
						tile = g_map.map(x, y);
						block = g_map.block(x, y);
						blocktype = g_map.blockat(x, y)->iType;
					}

					if((tile & tile_flag_solid) || 
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
							int tile = tile_flag_nonsolid;
							IO_Block * block = NULL;
							short blocktype = -1;

							if(actualvalues[0][j] >= 0 && actualvalues[0][j] < 640 && actualvalues[1][i] > 0 && actualvalues[1][i] < 480)
							{
								tile = g_map.map(corners[0][j], corners[1][i]);
								block = g_map.block(corners[0][j], corners[1][i]);
								blocktype = g_map.blockat(corners[0][j], corners[1][i])->iType;
							}

							if( (tile & tile_flag_solid) || 
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

					if(game_values.matchtype != MATCH_TYPE_SINGLE_GAME && game_values.matchtype != MATCH_TYPE_MINIGAME)
						UpdateScoreBoard();

					CleanUp();
					game_values.gamestate = GS_MENU;

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

							if(game_values.gamemode->playerkilledself(*(list_players[k]), kill_style_environment) == player_kill_normal)
								list_players[k]->die(0, false);
						}
					}
					else if(event.key.keysym.sym == SDLK_x)
					{
						short iplayer = rand() % list_players_cnt;
						list_players[iplayer]->makefrozen(300);
					}
					else if(event.key.keysym.sym == SDLK_c)
					{
						short iplayer = rand() % list_players_cnt;
						list_players[iplayer]->shield = rand() % 3 + 1;
						list_players[iplayer]->shieldtimer = 620;
					}
					else if(event.key.keysym.sym == SDLK_1)
					{
						if(event.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL))
							objectcontainer[0].add(new PU_IceWandPowerup(&spr_icewandpowerup, list_players[0]->ix + 32, list_players[0]->iy, 1, 0, 30, 30, 1, 1));
						else if(event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT))
							objectcontainer[0].add(new PU_BobombPowerup(&spr_bobombpowerup, list_players[0]->ix + 32, list_players[0]->iy - 1, 1, true, 0, 30, 30, 1, 1));	
						else
							objectcontainer[0].add(new PU_StarPowerup(&spr_starpowerup, list_players[0]->ix + 32, list_players[0]->iy, 4, true, 2, 30, 30, 1, 1));
					}
					else if(event.key.keysym.sym == SDLK_2)
					{
						if(event.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL))
							objectcontainer[0].add(new PU_BombPowerup(&spr_bombpowerup, list_players[0]->ix + 32, list_players[0]->iy, 1, 0, 30, 30, 1, 1));
						else if(event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT))
							objectcontainer[0].add(new PU_PowPowerup(&spr_powpowerup, list_players[0]->ix + 32, list_players[0]->iy - 1, 8, true, 8, 30, 30, 1, 1));
						else
							objectcontainer[0].add(new PU_ExtraGuyPowerup(&spr_1uppowerup, list_players[0]->ix + 32, list_players[0]->iy, 1, true, 0, 30, 30, 1, 1, 1));
					}
					else if(event.key.keysym.sym == SDLK_3)
					{
						if(event.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL))
							objectcontainer[0].add(new PU_PodoboPowerup(&spr_podobopowerup, list_players[0]->ix + 32, list_players[0]->iy, 1, 0, 30, 30, 1, 1));
						else if(event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT))
							objectcontainer[0].add(new PU_BulletBillPowerup(&spr_bulletbillpowerup, list_players[0]->ix + 32, list_players[0]->iy - 1, 1, true, 0, 30, 30, 1, 1));
						else
							objectcontainer[0].add(new PU_ExtraGuyPowerup(&spr_2uppowerup, list_players[0]->ix + 32, list_players[0]->iy, 1, true, 0, 30, 30, 1, 1, 2));
					}
					else if(event.key.keysym.sym == SDLK_4)
					{
						if(event.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL))
							objectcontainer[0].add(new PU_Tanooki(list_players[0]->ix + 32, list_players[0]->iy));
						else if(event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT))
							objectcontainer[1].add(new CO_Shell(0, list_players[0]->ix + 32, list_players[0]->iy, true, true, true, false));
						else
							objectcontainer[0].add(new PU_ExtraGuyPowerup(&spr_3uppowerup, list_players[0]->ix + 32, list_players[0]->iy, 1, true, 0, 30, 30, 1, 1, 3));
					}
					else if(event.key.keysym.sym == SDLK_5)
					{
						if(event.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL))
							objectcontainer[0].add(new PU_PWingsPowerup(&spr_pwingspowerup, list_players[0]->ix + 32, list_players[0]->iy));
						else if(event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT))
							objectcontainer[1].add(new CO_Shell(1, list_players[0]->ix + 32, list_players[0]->iy, false, true, true, false));
						else
							objectcontainer[0].add(new PU_ExtraGuyPowerup(&spr_5uppowerup, list_players[0]->ix + 32, list_players[0]->iy, 1, true, 0, 30, 30, 1, 1, 5));
					}
					else if(event.key.keysym.sym == SDLK_6)
					{
						if(event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT))
							objectcontainer[1].add(new CO_Shell(2, list_players[0]->ix + 32, list_players[0]->iy, false, false, true, true));
						else
							objectcontainer[0].add(new PU_FirePowerup(&spr_firepowerup, list_players[0]->ix + 32, list_players[0]->iy, 1, true, 0, 30, 30, 1, 1));
					}
					else if(event.key.keysym.sym == SDLK_7)
					{
						if(event.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL))
							objectcontainer[1].add(new CO_Spring(&spr_spring, list_players[0]->ix + 32, list_players[0]->iy));
						else if(event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT))
							objectcontainer[1].add(new CO_Shell(3, list_players[0]->ix + 32, list_players[0]->iy, false, true, false, false));
						else
							objectcontainer[0].add(new PU_HammerPowerup(&spr_hammerpowerup, list_players[0]->ix + 32, list_players[0]->iy, 1, true, 0, 30, 30, 1, 1));
					}
					else if(event.key.keysym.sym == SDLK_8)
					{
						if(event.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL))
							objectcontainer[1].add(new CO_Spike(&spr_spike, list_players[0]->ix + 32, list_players[0]->iy));
						else if(event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT))
							objectcontainer[0].add(new PU_ModPowerup(&spr_modpowerup, list_players[0]->ix + 32, list_players[0]->iy, 8, true, 8, 30, 30, 1, 1));
						else
							objectcontainer[0].add(new PU_PoisonPowerup(&spr_poisonpowerup, list_players[0]->ix + 32, list_players[0]->iy, 1, true, 0, 30, 30, 1, 1));
					}
					else if(event.key.keysym.sym == SDLK_9)
					{
						if(event.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL))
							objectcontainer[1].add(new CO_KuriboShoe(&spr_kuriboshoe, list_players[0]->ix + 32, list_players[0]->iy));
						else if(event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT))
							objectcontainer[0].add(new PU_FeatherPowerup(&spr_featherpowerup, list_players[0]->ix + 32, list_players[0]->iy - 1, 1, 0, 30, 30, 1, 1));
						else
							objectcontainer[0].add(new PU_ClockPowerup(&spr_clockpowerup, list_players[0]->ix + 32, list_players[0]->iy - 1, 1, true, 0, 30, 30, 1, 1));
					}
					else if(event.key.keysym.sym == SDLK_0)
					{
						if(event.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL))
							objectcontainer[0].add(new PU_LeafPowerup(&spr_leafpowerup, list_players[0]->ix + 32, list_players[0]->iy - 1, 1, 0, 30, 30, 1, 1));
						else if(event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT))
							objectcontainer[0].add(new PU_BoomerangPowerup(&spr_boomerangpowerup, list_players[0]->ix + 32, list_players[0]->iy - 1, 1, true, 0, 30, 30, 1, 1));
						else
							objectcontainer[0].add(new PU_MysteryMushroomPowerup(&spr_mysterymushroompowerup, list_players[0]->ix + 32, list_players[0]->iy - 1, 1, true, 0, 30, 30, 1, 1));
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
			//If the cancel button is pressed
			if(game_values.forceexittimer > 0)
			{
				if(--game_values.forceexittimer <= 0)
				{
					game_values.gamestate = GS_END_GAME;
					game_values.screenfade = 8;
					game_values.screenfadespeed = 8;
				}
			}

			for(int iPlayer = 0; iPlayer < 4; iPlayer++)
			{
				COutputControl * playerKeys = &game_values.playerInput.outputControls[iPlayer];

				//If the start key is pressed (pause key)
				if(playerKeys->game_start.fPressed && IsPauseAllowed())
				{
					if(!game_values.showscoreboard && !game_values.exitinggame)
					{
						game_values.pausegame = !game_values.pausegame;

						if(game_values.pausegame)
						{
							menu_shade.setalpha(GetScreenBackgroundFade());
							menu_shade.draw(0, 0);
						}

						//ifsoundonpause(sfx_invinciblemusic);
						//ifsoundonpause(sfx_slowdownmusic);
						ifsoundonplay(sfx_pause);
					}
				}

				//Only player 1 is allowed to exit a game
				//if(iPlayer != 0)
				//	continue;

				if((playerKeys->game_cancel.fPressed || (playerKeys->game_start.fPressed && game_values.gamemode->gameover)) && IsExitAllowed())
				{
					if(game_values.gamemode->gameover)
					{
						if(game_values.matchtype == MATCH_TYPE_TOUR || game_values.matchtype == MATCH_TYPE_TOURNAMENT)
							UpdateScoreBoard();

						CleanUp();
						game_values.gamestate = GS_MENU;

						return;
					}
					else
					{
						if(!game_values.pausegame && !game_values.exitinggame)
						{
							menu_shade.setalpha(GetScreenBackgroundFade());
							menu_shade.draw(0, 0);
							game_values.exitinggame = true;
							//ifsoundonpause(sfx_invinciblemusic);
							//ifsoundonpause(sfx_slowdownmusic);

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
							//ifsoundonpause(sfx_invinciblemusic);
							//ifsoundonpause(sfx_slowdownmusic);

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

					//Kill players touching the ground (or in air for MOd blocks)
					for(short k = 0; k < list_players_cnt; k++)
					{
						CPlayer * player = list_players[k];
						if(player->globalID == game_values.screenshakeplayerid)
							continue;

						if(game_values.teamcollision != 2 && game_values.screenshaketeamid == player->teamID)
							continue;
						
						if(!player->invincible && player->shield == 0 && !player->fKuriboShoe && player->isready())
						{
							if(game_values.screenshakekillinair == player->inair)
							{
								PlayerKilledPlayer(game_values.screenshakeplayerid, player, death_style_jump, kill_style_pow, false);

								CPlayer * killer = GetPlayerFromGlobalID(game_values.screenshakeplayerid);

								if(killer)
								{
									game_values.screenshakekillscount++;
									
									if(killer->inair)
										killer->killsinrowinair--;  //Don't want to give both shake and in air award
								}
							}
						}
					}
					

					//Kill goombas and koopas
					for(short k = 0; k < objectcontainer[0].list_end; k++)
					{
						CObject * object = objectcontainer[0].list[k];
						if(object->getObjectType() == object_moving)
						{
							IO_MovingObject * movingobject = (IO_MovingObject *)object;
							MovingObjectType type = movingobject->getMovingObjectType();
							
							if((type == movingobject_goomba || type == movingobject_koopa || type == movingobject_buzzybeetle || type == movingobject_spiny)
								&& game_values.screenshakekillinair == movingobject->inair)
							{
								CPlayer * killer = GetPlayerFromGlobalID(game_values.screenshakeplayerid);

								if(killer)
								{
									if(!game_values.gamemode->gameover)
										killer->score->AdjustScore(1);

									ifsoundonplay(sfx_kicksound);
									((MO_WalkingEnemy*)movingobject)->DieAndDropShell();

									game_values.screenshakekillscount++;
									
									if(killer->inair)
										killer->killsinrowinair--;  //Don't want to give both shake and in air award
								}
							}
						}
					}

					//Destroy throw blocks and flip shells over
					for(short k = 0; k < objectcontainer[1].list_end; k++)
					{
						CObject * object = objectcontainer[1].list[k];
						if(object->getObjectType() == object_moving)
						{
							IO_MovingObject * movingobject = (IO_MovingObject *)object;
							
							if(game_values.screenshakekillinair == movingobject->inair)
							{
								if(movingobject->getMovingObjectType() == movingobject_shell)
								{
									CO_Shell * shell = (CO_Shell*)movingobject;
									if(!shell->owner || shell->owner->inair == game_values.screenshakekillinair)
										shell->Flip();
								}
								else if(movingobject->getMovingObjectType() == movingobject_throwblock)
								{
									CO_ThrowBlock * throwblock = (CO_ThrowBlock*)movingobject;
									if(!throwblock->owner || throwblock->owner->inair == game_values.screenshakekillinair)
										throwblock->Die();
								}
							}
						}
					}

					//Add kills in row for kills from pow and mod
					if(game_values.screenshakekillscount > 1 && game_values.awardstyle != award_style_none)
					{
						game_values.screenshakekillscount = 0;

						CPlayer * killer = GetPlayerFromGlobalID(game_values.screenshakeplayerid);

						if(killer)
							killer->AddKillsInRowInAirAward();
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
							objectcontainer[2].add(new MO_BulletBill(&spr_bulletbill, &spr_bulletbilldead, 0, (short)(rand() % 448), (rand() % 2 ? speed : -speed), iPlayer, false));
							ifsoundonplay(sfx_bulletbillsound);
						}
					}
				}

				if(game_values.matchtype == MATCH_TYPE_WORLD && game_values.gamemode->gameover && game_values.forceexittimer <= 0)
				{
					if(--game_values.noexittimer <= 0)
						game_values.noexit = false;
				}

				//------------- update objects -----------------------

#ifdef _DEBUG
				if(game_values.autokill)
				{
					for(short k = 0; k < list_players_cnt; k++)
					{
						list_players[k]->DeathAwards();

						if(!game_values.gamemode->playerkilledself(*(list_players[k]), kill_style_environment))
							list_players[k]->die(0, false);
					}
				}
#endif

				//Advance the cpu's turn (AI only calculates player's actions 1 out of 4 frames)
				if(++game_values.cputurn > 3)
					game_values.cputurn = 0;

				//Player to player collisions
				for(i = 0; i < list_players_cnt; i++)
				{
					CPlayer * player1 = list_players[i];
					if(player1->state > player_dead)
					{
						for(j = i + 1; j < list_players_cnt; j++)
						{
							CPlayer * player2 = list_players[j];
							if(player2->state > player_dead)
							{
								if(coldec_player2player(player1, player2))
								{
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
				g_map.updatePlatforms();

				game_values.playskidsound = false;
				game_values.playinvinciblesound = false;
				game_values.playflyingsound = false;

				for(i = 0; i < list_players_cnt; i++)
					list_players[i]->move();	//move all objects before doing object-object collision detection in
												//->think(), so we test against the new position after object-map collision detection

				//Play sound for skidding players
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

				//Play sound for players using PWings
				if(game_values.playflyingsound)
				{
					if(!sfx_flyingsound.isplaying())
						ifsoundonplay(sfx_flyingsound);
				}
				else
				{
					if(sfx_flyingsound.isplaying())
						ifsoundonstop(sfx_flyingsound);
				}

				noncolcontainer.update();
				objectcontainer[0].update();
				objectcontainer[1].update();
				objectcontainer[2].update();

				//Collide player with objects
				for(short iPlayer = 0; iPlayer < list_players_cnt; iPlayer++)
				{
					CPlayer * player = list_players[iPlayer];
					if(player->state != player_ready)
						continue;

					//Collide with objects
					for(short iLayer = 0; iLayer < 3; iLayer++)
					{
						for(short iObject = 0; iObject < objectcontainer[iLayer].list_end; iObject++)
						{
							CObject * object = objectcontainer[iLayer].list[iObject];

							if(!object->GetDead())
							{
								if(coldec_player2obj(player, object))
								{
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

				for(short iLayer1 = 0; iLayer1 < 3; iLayer1++)
				{
					short iContainerEnd1 = objectcontainer[iLayer1].list_end;
					for(short iObject1 = 0; iObject1 < iContainerEnd1; iObject1++)
					{
						CObject * object1 = objectcontainer[iLayer1].list[iObject1];

						if(object1->getObjectType() != object_moving)
							continue;

						IO_MovingObject * movingobject1 = (IO_MovingObject*)object1;

						for(short iLayer2 = iLayer1; iLayer2 < 3; iLayer2++)
						{
							short iContainerEnd2 = objectcontainer[iLayer2].list_end;
							for(short iObject2 = (iLayer1 == iLayer2 ? iObject1 + 1 : 0); iObject2 < iContainerEnd2; iObject2++)
							{
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
								if(g_iCollisionMap[iType1][iType2])
								{
									if(coldec_obj2obj(movingobject1, movingobject2))
									{
										collisionhandler_o2o(movingobject1, movingobject2);
									}
								}
								else if(g_iCollisionMap[iType2][iType1])
								{
									if(coldec_obj2obj(movingobject2, movingobject1))
									{
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

				eyecandyfront.cleandeadobjects();
				eyecandyback.cleandeadobjects();
				objectcontainer[2].cleandeadobjects();
				objectcontainer[1].cleandeadobjects();
				objectcontainer[0].cleandeadobjects();
				noncolcontainer.cleandeadobjects();
				CleanDeadPlayers();

				eyecandyfront.update();
				eyecandyback.update();
				game_values.gamemode->think();

				if(game_values.slowdownon != -1 && ++game_values.slowdowncounter > 580)
				{
					game_values.slowdownon = -1;
					game_values.slowdowncounter = 0;
				}

				g_map.update();

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

			//Go to this label if a player collects a swap mushroom and we need to break out of two loops
			SWAPBREAK:

			if(game_values.swapplayers)
			{
				for(i = 0; i < list_players_cnt; i++)
				{
					list_players[i]->updateswap();
				}
			}

			if(game_values.screenfade == 255)
			{
				if(game_values.gamestate == GS_START_GAME)
				{
					CleanUp();
					SetGameModeSettingsFromMenu();
					game_values.gamestate = GS_GAME;
					
					/*
					game_values.gamemode = bossgamemode;  //boss type has already been set at this point

					if(bossgamemode->GetBossType() == 0)
						g_map.loadMap(convertPath("maps/special/dungeon.map"), read_type_full);
					else if(bossgamemode->GetBossType() == 1)
						g_map.loadMap(convertPath("maps/special/hills.map"), read_type_full);
					else if(bossgamemode->GetBossType() == 2)
						g_map.loadMap(convertPath("maps/special/volcano.map"), read_type_full);

					char filename[128];
					sprintf(filename, "gfx/packs/backgrounds/%s", g_map.szBackgroundFile);
					std::string path = convertPath(filename, gamegraphicspacklist.current_name());

					//if the background file doesn't exist, use the classic background
					if(!File_Exists(path))
						path = convertPath("gfx/packs/backgrounds/Land_Classic.png", gamegraphicspacklist.current_name());

					gfx_loadimagenocolorkey(spr_background, path, false);

					g_map.predrawbackground(spr_background, spr_backmap);
					g_map.predrawforeground(spr_frontmap);
					g_map.SetupAnimatedTiles();
					LoadMapObjects(false);
					*/

					if(game_values.music)
					{
						musiclist.SetRandomMusic(g_map.musicCategoryID, "", "");
						backgroundmusic[0].load(musiclist.GetCurrentMusic());
						backgroundmusic[0].play(game_values.playnextmusic, false);
					}

					return;
				}
				else if(game_values.gamestate == GS_END_GAME)
				{
					CleanUp();
					game_values.gamestate = GS_MENU;
					game_values.screenfadespeed = -8;

					return;
				}
			}

			//--------------- draw everything ----------------------

			if(y_shake > 0)
			{
				SDL_Rect rect = {0, 0, 640, y_shake};
				SDL_FillRect(screen, &rect, 0x0);		//fill empty area with black
			}

			spr_backmap[g_iCurrentDrawIndex].draw(0, 0);

			//draw back eyecandy behind players
			noncolcontainer.draw();
			eyecandyback.draw();

			game_values.gamemode->draw_background();

			objectcontainer[0].draw();

			if(!game_values.swapplayers)
			{
				for(i = 0; i < list_players_cnt; i++)
					list_players[i]->draw();
			}

			objectcontainer[1].draw();

			g_map.drawPlatforms();

#ifdef _XBOX
			g_map.drawfrontlayer();
#else
			if(game_values.toplayer)
				g_map.drawfrontlayer();
#endif
			g_map.drawWarpLocks();

			objectcontainer[2].draw();
			eyecandyfront.draw();
			game_values.gamemode->draw_foreground();
		
			g_iWinningPlayer = -1;
			short mostkills = 0;
			
			//Draw scoreboards for all games (except special cases where we have a single player walking the map)
			if(game_values.singleplayermode == -1)
			{
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
						
					game_font_large.drawCentered(320, 90, gameovertext);

				}

				//in game scoreboards
				for(i = 0; i < score_cnt; i++)
				{
					if(game_values.gamemode->gamemode == game_mode_health || game_values.gamemode->gamemode == game_mode_collection)
						spr_shade[game_values.teamcounts[i] - 1].draw(score[i]->x, score[i]->y);
					else
						spr_shade[game_values.teamcounts[i] - 1].draw(score[i]->x, score[i]->y, 0, 0, 256, 41);

					for(short k = 0; k < game_values.teamcounts[i]; k++)
					{
						short globalID = game_values.teamids[i][k];

						//If player is respawning, draw an animated egg counter
						if(respawn[globalID] > 0 && !game_values.gamemode->gameover)
						{
							if(++respawnanimationtimer[globalID] > 8)
							{
								respawnanimationtimer[globalID] = 0;
								respawnanimationframe[globalID] += 32;

								if(respawnanimationframe[globalID] > 32)
									respawnanimationframe[globalID] = 0;
							}

							short scorex = score[i]->x + scoreoffsets[k];
							short scorey = score[i]->y + 2;
							spr_egg.draw(scorex, scorey, respawnanimationframe[globalID], game_values.colorids[globalID] << 5, 32, 32);
							spr_eggnumbers.draw(scorex, scorey, ((respawn[globalID] - 1) >> 1) << 5, game_values.colorids[globalID] << 5, 32, 32);
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

								if(player->jailtimer > 0)
									spr_jail.draw(iScoreOffsetX - 6, iScoreOffsetY - 6, (player->jailcolor + 1) * 44, 0, 44, 44);

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

						//Draw stored powerup
						if(storedpowerupid != -1)
						{
							if(!game_values.swapplayers)
							{
								spr_storedpowerupsmall.draw(score[i]->x + scorepowerupoffsets[game_values.teamcounts[i] - 1][k], score[i]->y + 25, storedpowerupid * 16, 0, 16, 16);
							}
						}
					}

					//Draw hearts for health mode
					if(game_values.gamemode->gamemode == game_mode_health)
					{
						short iLife = score[i]->subscore[0];
						short iMax = score[i]->subscore[1];
						short iHeartX = score[i]->x + scorepowerupoffsets[game_values.teamcounts[i] - 1][0] - 32;

						for(short iHeart = 0; iHeart < iLife; iHeart++)
						{
							if(iHeart == iMax - 1 && iHeart % 2 == 0)
								spr_scorehearts.draw(iHeartX + iHeart * 8, score[i]->y + 43, 32, 0, 8, 16);
							else
								spr_scorehearts.draw(iHeartX + iHeart * 8, score[i]->y + 43, iHeart % 2 ? 8 : 0, 0, 8, 16);
						}

						for(short iHeart = iLife; iHeart < iMax; iHeart++)
						{
							if(iHeart == iMax - 1 && iHeart % 2 == 0)
								spr_scorehearts.draw(iHeartX + iHeart * 8, score[i]->y + 43, 40, 0, 8, 16);
							else
								spr_scorehearts.draw(iHeartX + iHeart * 8, score[i]->y + 43, iHeart % 2 ? 24 : 16, 0, 8, 16);
						}
					}
					else if(game_values.gamemode->gamemode == game_mode_collection) //Draw cards for collection mode
					{
						//Flash collected cards if 3 have been collected
						if(score[i]->subscore[0] < 3 || score[i]->subscore[2] % 20 < 10)
						{
							short iNumCards = score[i]->subscore[0];
							short iCardValues = score[i]->subscore[1];
							short iCardX = score[i]->x + scorepowerupoffsets[game_values.teamcounts[i] - 1][0] - 20;

							for(short iCard = 0; iCard < iNumCards; iCard++)
							{
								spr_scorecards.draw(iCardX + iCard * 20, score[i]->y + 43, (iCardValues & 3) << 4, 0, 16, 16);
								iCardValues >>= 2;
							}
						}
					}
					
					short iScoreX = score[i]->x + iScoreTextOffset[i];
					short iScoreY = score[i]->y + 4;

					spr_scoretext.draw(iScoreX, iScoreY, score[i]->iDigitLeft, (score[i]->iDigitLeft == 0 ? 16 : 0), 16, 16);
					spr_scoretext.draw(iScoreX + 18, iScoreY, score[i]->iDigitMiddle, (score[i]->iDigitLeft == 0 && score[i]->iDigitMiddle == 0 ? 16 : 0), 16, 16);
					spr_scoretext.draw(iScoreX + 36, iScoreY, score[i]->iDigitRight, 0, 16, 16);
				}
			}

			//draw arrows for being above the top of the screen
			for(i = 0; i < list_players_cnt; i++)
				list_players[i]->drawarrows();


			if(game_values.screenfadespeed != 0)
			{
				g_map.update();
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
				menu_font_large.drawCentered(320, 200, "Pause");

				menu_font_large.drawCentered(320, 240, game_values.gamemode->GetModeName());

				char szGoal[256];
				strcpy(szGoal, game_values.gamemode->GetGoalName());
				strcat(szGoal, ": ");

				if(game_values.gamemode->goal == -1)
					sprintf(szGoal + strlen(szGoal), "Unlimited");
				else
					sprintf(szGoal + strlen(szGoal), "%d", game_values.gamemode->goal);
				
				menu_font_large.drawCentered(320, 264, szGoal);
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

		//Make sure music and sound effects keep playing
		if(game_values.slowdownon != -1)
		{
			if(!sfx_slowdownmusic.isplaying())
				ifsoundonplay(sfx_slowdownmusic);
		}
		else
		{
			if(sfx_slowdownmusic.isplaying())
				ifsoundonstop(sfx_slowdownmusic);
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
	for(i = 0; i < list_players_cnt; i++)
	{
		delete list_players[i];
	}
	list_players_cnt = 0;

	eyecandyfront.clean();
	eyecandyback.clean();
	
	//noncolcontainer.clean();

	//objectcontainer[0].clean();
	//objectcontainer[1].clean();
	//objectcontainer[2].clean();
		
	LoadMapObjects(true);
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
	if(game_values.matchtype == MATCH_TYPE_WORLD)
	{
		//If no one won, then nothing on the world map has changed
		if(game_values.gamemode->winningteam < 0)
			return;

		if(game_values.tourstops[game_values.tourstopcurrent]->fEndStage)
		{
			game_values.tournamentwinner = 1;
			backgroundmusic[4].play(true, true);
		}

		//Add up all the winnings so far and determine overall place in the standings
		for(short iScore = 0; iScore < score_cnt; iScore++)
			game_values.tournament_scores[iScore].wins = 0;

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

		//Give players the item bonuses that were won
		TourStop * tourStop = game_values.tourstops[game_values.tourstopcurrent];

		for(short iScore = 0; iScore < score_cnt; iScore++)
		{
			for(short iBonus = 0; iBonus < tourStop->iNumBonuses; iBonus++)
			{
				if(tourStop->wsbBonuses[iBonus].iWinnerPlace == score[iScore]->place)
				{
					if(game_values.worldpowerupcount[iScore] < 32)
						game_values.worldpowerups[iScore][game_values.worldpowerupcount[iScore]++] = tourStop->wsbBonuses[iBonus].iBonus;
					else
						game_values.worldpowerups[iScore][31] = tourStop->wsbBonuses[iBonus].iBonus;	
				}
			}
		}
	}
	else if(game_values.matchtype == MATCH_TYPE_TOUR)
	{
		//If no one won (tied game), then there is no need to update the scores because nothing has changed
		if(game_values.gamemode->winningteam < 0)
			return;

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
	else if(game_values.matchtype == MATCH_TYPE_TOURNAMENT)
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
	musiclist.SetNextMusic(g_map.musicCategoryID, maplist.currentShortmapname(), g_map.szBackgroundFile);
	backgroundmusic[0].load(musiclist.GetCurrentMusic());
	backgroundmusic[0].play(game_values.playnextmusic, false);
}

bool coldec_player2player(CPlayer * o1, CPlayer * o2)
{
	//Special cases to deal with players overlapping the right and left sides of the screen

	if(o1->ix + PW < o2->ix)
	{
		if (o1->ix + 640 >= o2->ix + PW || o1->ix + PW + 640 < o2->ix || o1->iy > o2->iy + PH || o1->iy + PH < o2->iy) 
			return false;
		else 
			return true;
	}
	else if(o2->ix + PW < o1->ix)
	{
		if (o1->ix >= o2->ix + PW + 640 || o1->ix + PW < o2->ix + 640 || o1->iy > o2->iy + PH || o1->iy + PH < o2->iy) 
			return false;
		else 
			return true;
	}
	else  //Normal case where no overlap
	{
		if (o1->ix >= o2->ix + PW || o1->ix + PW < o2->ix || o1->iy > o2->iy + PH || o1->iy + PH < o2->iy) 
			return false;
		else 
			return true;
	}
}

bool coldec_player2obj(CPlayer * o1, CObject * o2)
{
	//Special cases to deal with players overlapping the right and left sides of the screen
	if(o1->ix + PW < o2->ix)
	{
		if (o1->ix + 640 >= o2->ix + o2->collisionWidth || o1->ix + PW + 640 < o2->ix || o1->iy >= o2->iy + o2->collisionHeight || o1->iy + PH < o2->iy) 
			return false;
		else 
			return true;
	}
	else if(o2->ix + o2->collisionWidth < o1->ix)
	{
		if (o1->ix >= o2->ix + o2->collisionWidth + 640 || o1->ix + PW < o2->ix + 640 || o1->iy >= o2->iy + o2->collisionHeight || o1->iy + PH < o2->iy) 
			return false;
		else 
			return true;
	}
	else //Normal case where no overlap
	{
		if (o1->ix >= o2->ix + o2->collisionWidth || o2->ix > o1->ix + PW || o1->iy >= o2->iy + o2->collisionHeight || o2->iy > o1->iy + PH) 
			return false;
		else 
			return true;
	}
}

bool coldec_obj2obj(CObject * o1, CObject * o2)
{
	//Special cases to deal with players overlapping the right and left sides of the screen
	short o1r = o1->ix + o1->collisionWidth;
	short o1b = o1->iy + o1->collisionHeight;
	short o2r = o2->ix + o2->collisionWidth;
	short o2b = o2->iy + o2->collisionHeight;

	if(o1r < o2->ix)
	{
		if (o1->ix + 640 >= o2r || o1r + 640 < o2->ix || o1->iy >= o2b || o1b < o2->iy) 
			return false;
		else 
			return true;
	}
	else if(o2r < o1->ix)
	{
		if (o1->ix >= o2r + 640 || o1r < o2->ix + 640 || o1->iy >= o2b || o1b < o2->iy)
			return false;
		else 
			return true;
	}
	else
	{
		if (o1->ix >= o2r || o2->ix > o1r || o1->iy >= o2b || o2->iy > o1b) 
			return false;
		else 
			return true;
	}
}

void SetGameModeSettingsFromMenu()
{
	//If this is a tour stop and the tour has settings in it, use those.  Otherwise use the menu settings.
	if((game_values.matchtype == MATCH_TYPE_TOUR && game_values.tourstops[game_values.tourstopcurrent]->fUseSettings) || 
		game_values.matchtype == MATCH_TYPE_WORLD)
		memcpy(&game_values.gamemodesettings, &game_values.tourstops[game_values.tourstopcurrent]->gmsSettings, sizeof(GameModeSettings));
	else
		memcpy(&game_values.gamemodesettings, &game_values.gamemodemenusettings, sizeof(GameModeSettings));
}

void LoadMapObjects(bool fPreview)
{
	LoadMapHazards(fPreview);

	//Clear all the previous switch settings
	for(short iSwitch = 0; iSwitch < 8; iSwitch++)
		g_map.switchBlocks[iSwitch].clear();
	
	//Add blocks (breakable, note, switch, throwable, etc)
	for(short x = 0; x < MAPWIDTH; x++)
	{
		for(short y = 0; y < MAPHEIGHT; y++)
		{
			short iType = g_map.objectdata[x][y].iType;
			if(iType == 0)
			{
				g_map.blockdata[x][y] = new B_BreakableBlock(&spr_breakableblock, x * TILESIZE, y * TILESIZE, 4, 10);
				noncolcontainer.add(g_map.blockdata[x][y]);
			}
			else if(iType == 1)
			{
				g_map.blockdata[x][y] = new B_PowerupBlock(&spr_powerupblock, x * TILESIZE, y * TILESIZE, 4, 10, g_map.objectdata[x][y].fHidden, g_map.objectdata[x][y].iSettings);
				noncolcontainer.add(g_map.blockdata[x][y]);
			}
			else if(iType == 2)
			{
				g_map.blockdata[x][y] = new B_DonutBlock(&spr_donutblock, x * TILESIZE, y * TILESIZE);
				noncolcontainer.add(g_map.blockdata[x][y]);
			}
			else if(iType == 3)
			{
				g_map.blockdata[x][y] = new B_FlipBlock(&spr_flipblock, x * TILESIZE, y * TILESIZE, g_map.objectdata[x][y].fHidden);
				noncolcontainer.add(g_map.blockdata[x][y]);
			}
			else if(iType == 4)
			{
				g_map.blockdata[x][y] = new B_BounceBlock(&spr_bounceblock, x * TILESIZE, y * TILESIZE, g_map.objectdata[x][y].fHidden);
				noncolcontainer.add(g_map.blockdata[x][y]);
			}
			else if(iType == 5)
			{
				g_map.blockdata[x][y] = new B_NoteBlock(&spr_noteblock, x * TILESIZE, y * TILESIZE, 4, 10, 1, g_map.objectdata[x][y].fHidden);
				noncolcontainer.add(g_map.blockdata[x][y]);
			}
			else if(iType == 6)
			{
				g_map.blockdata[x][y] = new B_ThrowBlock(&spr_throwblock, x * TILESIZE, y * TILESIZE, 4, 10, 0);
				noncolcontainer.add(g_map.blockdata[x][y]);
			}
			else if(iType >= 7 && iType <= 10)
			{
				short iSwitchType = iType - 7;
				g_map.blockdata[x][y] = new B_OnOffSwitchBlock(&spr_switchblocks, x * TILESIZE, y * TILESIZE, iSwitchType, g_map.iSwitches[iSwitchType]);
				noncolcontainer.add(g_map.blockdata[x][y]);
				g_map.switchBlocks[iSwitchType].push_back(g_map.blockdata[x][y]);
			}
			else if(iType >= 11 && iType <= 14)
			{
				short iSwitchType = iType - 11;

				//g_map.blockdata[x][y] = new B_SwitchBlock(&spr_switchblocks, x * TILESIZE, y * TILESIZE, iSwitchType, g_map.iSwitches[iSwitchType]);
				g_map.blockdata[x][y] = new B_SwitchBlock(&spr_switchblocks, x * TILESIZE, y * TILESIZE, iSwitchType, g_map.objectdata[x][y].iSettings[0]);
				noncolcontainer.add(g_map.blockdata[x][y]);
				g_map.switchBlocks[iSwitchType + 4].push_back(g_map.blockdata[x][y]);
			}
			else if(iType == 15)
			{
				g_map.blockdata[x][y] = new B_ViewBlock(&spr_viewblock, x * TILESIZE, y * TILESIZE, g_map.objectdata[x][y].fHidden, g_map.objectdata[x][y].iSettings);
				noncolcontainer.add(g_map.blockdata[x][y]);
			}
			else if(iType == 16)
			{
				g_map.blockdata[x][y] = new B_ThrowBlock(&spr_throwblock, x * TILESIZE, y * TILESIZE, 4, 10, 2);
				noncolcontainer.add(g_map.blockdata[x][y]);
			}
			else if(iType == 17 || iType == 18)
			{
				g_map.blockdata[x][y] = new B_NoteBlock(&spr_noteblock, x * TILESIZE, y * TILESIZE, 4, 10, iType == 17 ? 2 : 0, g_map.objectdata[x][y].fHidden);
				noncolcontainer.add(g_map.blockdata[x][y]);
			}
			else if(iType == 19)
			{
				g_map.blockdata[x][y] = new B_ThrowBlock(&spr_throwblock, x * TILESIZE, y * TILESIZE, 4, 10, 1);
				noncolcontainer.add(g_map.blockdata[x][y]);
			}
			else if(iType >= 20 && iType <= 29)
			{
				g_map.blockdata[x][y] = new B_WeaponBreakableBlock(&spr_weaponbreakableblock, x * TILESIZE, y * TILESIZE, iType - 20);
				noncolcontainer.add(g_map.blockdata[x][y]);
			}
			else
			{
				g_map.blockdata[x][y] = NULL;
			}
		}
	}

	//Add map objects like springs, shoes and spikes
	for(short i = 0; i < g_map.iNumMapItems; i++)
	{
		MapItem * mapItem = &g_map.mapitems[i];
		short iType = mapItem->itype;
		short ix = mapItem->ix * TILESIZE;
		short iy = mapItem->iy * TILESIZE;

		if(iType == 0)
			objectcontainer[1].add(new CO_Spring(&spr_spring, ix, iy));
		else if(iType == 1)
			objectcontainer[1].add(new CO_Spike(&spr_spike, ix, iy));
		else if(iType == 2)
			objectcontainer[1].add(new CO_KuriboShoe(&spr_kuriboshoe, ix, iy));
	}

	//Set all the 1x1 gaps up so players can run across them
	g_map.UpdateAllTileGaps();
}

void LoadMapHazards(bool fPreview)
{
	//Make sure we don't have any objects created before we create them from the map settings
	noncolcontainer.clean();
	objectcontainer[0].clean();
	objectcontainer[1].clean();
	objectcontainer[2].clean();

	//Create objects for all the map hazards
	for(short iMapHazard = 0; iMapHazard < g_map.iNumMapHazards; iMapHazard++)
	{
		MapHazard * hazard = &g_map.maphazards[iMapHazard];
		if(hazard->itype == 0)
		{
			for(short iFireball = 0; iFireball < hazard->iparam[0]; iFireball++)
				objectcontainer[1].add(new OMO_OrbitHazard(&spr_hazard_fireball[fPreview ? 1 : 0], (hazard->ix << 4) + 16, (hazard->iy << 4) + 16, (float)(iFireball * 24), hazard->dparam[0], hazard->dparam[1], 4, 8, 18, 18, 0, 0, 0, hazard->dparam[0] < 0.0f ? 18 : 0, 18, 18));
		}
		else if(hazard->itype == 1)
		{
			float dSector = TWO_PI / hazard->iparam[0];
			for(short iRotoDisc = 0; iRotoDisc < hazard->iparam[0]; iRotoDisc++)
			{
				float dAngle = hazard->dparam[1] + iRotoDisc * dSector;
				if(dAngle > TWO_PI)
					dAngle -= TWO_PI;

				objectcontainer[1].add(new OMO_OrbitHazard(&spr_hazard_rotodisc[fPreview ? 1 : 0], (hazard->ix << 4) + 16, (hazard->iy << 4) + 16, hazard->dparam[2], hazard->dparam[0], dAngle, 21, 8, 32, 32, 0, 0, 0, 0, 32, 32));
			}
		}
		else if(hazard->itype == 2)
		{
			noncolcontainer.add(new IO_BulletBillCannon(hazard->ix << 4, hazard->iy << 4, hazard->iparam[0], hazard->dparam[0], fPreview));
		}
		else if(hazard->itype == 3)
		{
			objectcontainer[1].add(new IO_FlameCannon(hazard->ix << 4, hazard->iy << 4, hazard->iparam[0], hazard->iparam[1] == 1));
		}
		else if(hazard->itype >= 4 && hazard->itype <= 7)
		{
			objectcontainer[1].add(new MO_PirhanaPlant(hazard->ix << 4, hazard->iy << 4, hazard->itype - 4, hazard->iparam[0], hazard->iparam[1], fPreview));
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
			list_players[iPlayer]->iSuicideCreditPlayerID = list_players[iNewSpot]->globalID;
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
	objectcontainer[0].cleandeadobjects();	

	return true;
}

/*
void EnterBossMode(short type)
{
	if(game_values.gamestate == GS_GAME && game_values.gamemode->gamemode != game_mode_boss)
	{
		//bossgamemode->SetBossType(type);

		game_values.screenfade = 2;
		game_values.screenfadespeed = 2;

		backgroundmusic[0].stop();
		ifsoundonstop(sfx_invinciblemusic);
		ifsoundonstop(sfx_timewarning);
		ifsoundonstop(sfx_slowdownmusic);

		game_values.gamestate = GS_START_GAME;
	}
}*/

bool IsExitAllowed()
{
	if(!game_values.noexit || list_players_cnt == 0)
		return true;

	for(int iPlayer = 0; iPlayer < list_players_cnt; iPlayer++)
	{
		if(game_values.playercontrol[list_players[iPlayer]->getGlobalID()] == 1)
			return false;
	}

	return true;
}

bool IsPauseAllowed()
{
	return !game_values.noexit;
}

