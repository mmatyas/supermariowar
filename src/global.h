
#ifndef _GLOBAL_H
#define _GLOBAL_H

#define FILEBUFSIZE 1024

//----------------------------------------------------------------
// this file contains stuff that's needed by every part of smw
//----------------------------------------------------------------

#define TITLESTRING "Super Mario War"
#define VERSIONNUMBER "1.9"

#include <stdlib.h>	//srand/rand (maybe replace with an own version)
#include <string.h>
//#include <string>
#include <map>
#include <vector>
#include <list>

#include "sfx.h"
#include "gfx.h"
//#include "net.h"
#include "path.h"

#ifdef LINUXFUNC
	#include "linfunc.h"
#endif

struct STextAward
{
	const char		*name;
	gfxFont			*font;
	
	STextAward(const char *nname, gfxFont *nfont) {name = nname; font = nfont;}
};



//------------- global definitions -------------


//----------------------------------------------------------------
// DON'T CHANGE THE ORDER xxx_R xxx_L xxx_R xxx_L !!!
//----------------------------------------------------------------
// because this is used to determine the direction the player was facing in the last frame
// if the last sprite was facing right (spr & 0x1) is 0
// because the last bit of every PGFX_[action]_R is 0
// the last bit of PGFX_[action]_L is 1
// IF YOU CHANGE THE ORDER OF THE PGFX_L/R STUFF THIS WON'T WORK!

// see CPlayer::draw() on how this is used

#define PGFX_STANDING_R	0
#define PGFX_STANDING_L	1
#define PGFX_RUNNING_R	2
#define PGFX_RUNNING_L	3
#define PGFX_JUMPING_R	4
#define PGFX_JUMPING_L	5
#define PGFX_STOPPING_R	6
#define PGFX_STOPPING_L	7
#define PGFX_DEADFLYING	8
#define PGFX_DEAD		9
//--------------------------
#define PGFX_LAST		10

#define PANNOUNCER_SOUND_LAST	20
#define PAWARD_LAST		9

#define PH				25		//Player height
#define PW				22		//Player width
#define HALFPH			12
#define HALFPW			11
#define PHOFFSET		4
#define PWOFFSET		5

#define VELMOVING		4.0f		//velocity (speed) for moving left, right
#define VELSLOWMOVING	2.2f		//velocity when slow down powerup is in effect
#define	VELMOVING_CHICKEN 2.9f		//speed of the chicken in the gamemode capturethechicken
#define VELMOVINGADD	0.5f
#define VELMOVINGADDICE VELMOVINGADD / 4
#define VELTURBOMOVING	5.5f
#define VELJUMP			9.0f		//velocity for jumping
#define VELSLOWJUMP		7.0f		//velocity for jumping when slow down powerup is in effect
#define VELTURBOJUMP	10.2f		//velocity for turbo jumping
#define VELSUPERJUMP	13.0f;		//super jump (jumping off of orange note blocks)
#define VELPUSHBACK		5.0f
#define VELMAXBREAKBLOCK 3.0f
#define VELNOTEBLOCKBOUNCE 3.0f
#define VELNOTEBLOCKREPEL 5.0f
#define VELBLOCKBOUNCE  3.0f
#define VELBLOCKREPEL   3.0f
#define VELPOWERUPBOUNCE 8.1f
#define FIREBALLBOUNCE  5.0f
#define HAMMERTHROW     8.5f
#define VELMOVINGFRICTION 0.2f
#define VELICEFRICTION  0.06f
#define VELAIRFRICTION  0.06f
#define VELSTOPJUMP     5.0f
#define BOUNCESTRENGTH  0.5f
#define TAGGEDBOOST    1.0f
#define VELSUPERSTOMP	10.0f
#define VELTAILSHAKE	1.0f
#define VELKURIBOBOUNCE	3.0f
#define VELENEMYBOUNCE  7.0f

#define	GRAVITATION		0.40f		

#define MAXVELY			20.0f
#define MAXSIDEVELY		10.0f

#define CRUNCHVELOCITY	2
#define CRUNCHAMOUNT	16
#define CRUNCHMAX       32


#define MAPWIDTH		20			//width of the map
#define MAPHEIGHT		15			//height of the map
#define MAPLAYERS		4			//number of layers the map has
#define TILESIZE		32			//size of the tiles, should be dynamically read
#define PREVIEWTILESIZE	16			//size of the preview tiles
#define THUMBTILESIZE	8			//size of the thumbnail tiles

#define	MAXWARPS		32
#define MAXMAPITEMS		32
#define MAXMAPHAZARDS	30
#define NUMMAPHAZARDPARAMS 5

#define NUMSPAWNAREATYPES 6
#define MAXSPAWNAREAS   128
#define MAXDRAWAREAS   128

#define NUMSTOMPENEMIES 9
#define NUMSURVIVALENEMIES 3
#define NUMFRENZYCARDS 19

#define	MAX_BONUS_CHESTS	5
#define MAX_WORLD_BONUSES_AWARDED 3

#define TILESETSIZE		960			//30*32 Tiles by 32 pixel in a 1024*1024 bmp
#define TILESETHEIGHT   30
#define TILESETWIDTH	32

#define NUMTILETYPES	19

#define TILESETANIMATED	-1
#define TILESETNONE		-2
#define TILESETUNKNOWN	-3

#define TILEANIMATIONSIZE	38		//number of tile animations

#define MAXMUSICCATEGORY	11
#define MAXWORLDMUSICCATEGORY	9
#define WORLDMUSICBONUS MAXWORLDMUSICCATEGORY
#define WORLDMUSICSLEEP MAXWORLDMUSICCATEGORY + 1
#define WORLDMUSICWORLDS MAXWORLDMUSICCATEGORY + 2
#define MAXCATEGORYTRACKS	64

#define MAXEYECANDY		192
#define MAXOBJECTS		300
#define CORPSESTAY		200

#define MENUSPACING     36

#define WAITTIME		16		//delay between frames (default:16)
#define MAXAWARDS       10
#define MINAWARDSNEEDED 3

#define MAXTOURNAMENTGAMES 10
#define MAXWORLDITEMS	32

#define MAXRACEGOALS	8

#define NUMBONUSITEMSONWHEEL 10

#define ifsoundonplay(x) (game_values.sound ? x.play() : -1)
#define ifsoundonandreadyplay(x) (x.isready() && game_values.sound ? x.play() : -1)
#define ifsoundonplayloop(x,y) (game_values.sound ? x.playloop(y) : -1)
#define ifsoundonstop(x) (game_values.sound ? x.stop() : void(NULL))
#define ifsoundonpause(x) (game_values.sound ? x.sfx_pause() : void(NULL))
#define ifmusiconplay(x) (game_values.music ? x.play() : -1)

#define ABS(x) (x>0?x:-x)

#define TWO_PI 6.2831853f
#define PI 3.1415292f
#define THREE_HALF_PI 4.7123890f
#define THREE_QUARTER_PI 2.3561945f
#define HALF_PI 1.5707963f
#define QUARTER_PI 0.7853982f

#define DEVICE_KEYBOARD -1

#define JOYSTICK_DEAD_ZONE 16384
#define MOUSE_X_DEAD_ZONE 0
#define MOUSE_Y_DEAD_ZONE 5

#define MOUSE_UP 323
#define MOUSE_DOWN 324
#define MOUSE_LEFT 325
#define MOUSE_RIGHT 326
#define MOUSE_BUTTON_START 327

#define JOY_STICK_1_UP 0
#define JOY_STICK_1_DOWN 1
#define JOY_STICK_1_LEFT 2
#define JOY_STICK_1_RIGHT 3

#define JOY_STICK_2_UP 4
#define JOY_STICK_2_DOWN 5
#define JOY_STICK_2_LEFT 6
#define JOY_STICK_2_RIGHT 7

#define JOY_HAT_UP 8
#define JOY_HAT_DOWN 9
#define JOY_HAT_LEFT 10
#define JOY_HAT_RIGHT 11
#define JOY_BUTTON_START 12

#define KEY_NONE -1

#define MENU_ITEM_NEIGHBOR_UP 0
#define MENU_ITEM_NEIGHBOR_DOWN 1
#define MENU_ITEM_NEIGHBOR_LEFT 2
#define MENU_ITEM_NEIGHBOR_RIGHT 3

#define NUM_POWERUPS 26
#define NUM_WORLD_POWERUPS 15
#define NUM_WORLD_SCORE_BONUSES 20
#define NUM_WORLD_ITEMS NUM_POWERUPS + NUM_WORLD_POWERUPS + NUM_WORLD_SCORE_BONUSES
#define NUM_BLOCK_SETTINGS NUM_POWERUPS
#define NUM_POWERUP_PRESETS 17

#define HEALTH_POWERUP -1
#define TIME_POWERUP -2
#define JAIL_KEY_POWERUP -3
#define COIN_POWERUP -4
#define MINIGAME_COIN -5
#define SECRET1_POWERUP -6
#define SECRET2_POWERUP -7
#define SECRET3_POWERUP -8
#define SECRET4_POWERUP -9
#define NO_POWERUP -10

void GetNameFromFileName(char * szName, const char * szFileName, bool fStripAuthor = false);
std::string stripCreatorAndDotMap(const std::string &filename);

#define NUM_AUTO_FILTERS 12

#define WORLD_FOREGROUND_STAGE_OFFSET 200
#define WORLD_WINNING_TEAM_SPRITE_OFFSET 600
#define WORLD_BRIDGE_SPRITE_OFFSET 604
#define WORLD_START_SPRITE_OFFSET 608
#define WORLD_FOREGROUND_SPRITE_OFFSET 700
#define WORLD_FOREGROUND_SPRITE_ANIMATED_OFFSET 900

#define WORLD_BACKGROUND_SPRITE_SET_SIZE 60
#define WORLD_PATH_SPRITE_SET_SIZE 20

#define NUM_FRAMES_IN_TILE_ANIMATION 4
#define NUM_FRAMES_BETWEEN_TILE_ANIMATION 8

#define COUNTDOWN_START_INDEX 4

//------------- data structures / selfmade include files -------------
#include "input.h"
#include "uimenu.h"
#include "uicontrol.h"
#include "menu.h"
#include "MapList.h"
#include "FileList.h"
#include "movingplatform.h"
#include "FileIO.h"
#include "map.h"
#include "player.h"
#include "object.h"
#include "objectgame.h"
#include "objecthazard.h"
//#include "HashTable.h"
#include "eyecandy.h"
#include "gamemodes.h"
#include "world.h"
#include "TilesetManager.h"


//------------- global variables / etc -------------
//i know that using so much global variables is ugly, this will change somewhen

//gfx stuff
extern SDL_Surface		*screen;		//for gfx
extern SDL_Surface		*blitdest;		//for gfx
extern short			x_shake;
extern short			y_shake;

extern CMap				*g_map;
extern CTilesetManager	*g_tilesetmanager;
extern CPlayer			*list_players[4];
extern short			list_players_cnt;

extern SDL_Joystick		**joysticks;
extern short			joystickcount;

extern CScore			*score[4];
extern short			respawn[4];
extern short			projectiles[4];
extern short			score_cnt;

extern CObjectContainer noncolcontainer;
extern CObjectContainer objectcontainer[3];

extern CEyecandyContainer eyecandy[3];

extern STextAward		awards[PAWARD_LAST];
extern sfxSound			sfx_announcer[PANNOUNCER_SOUND_LAST];

extern bool fResumeMusic;

extern sfxSound			sfx_mip;
extern sfxSound			sfx_deathsound;
extern sfxSound			sfx_jump;
extern sfxSound			sfx_skid;
extern sfxSound			sfx_capejump;

extern sfxSound			sfx_invinciblemusic;
extern sfxSound			sfx_extraguysound;
extern sfxSound			sfx_sprout;
extern sfxSound			sfx_collectpowerup;
extern sfxSound			sfx_collectfeather;
extern sfxSound			sfx_storepowerup;
extern sfxSound			sfx_tailspin;
extern sfxSound			sfx_breakblock;
extern sfxSound			sfx_bump;
extern sfxSound			sfx_coin;
extern sfxSound			sfx_fireball;
extern sfxSound			sfx_springjump;
extern sfxSound			sfx_timewarning;
extern sfxSound			sfx_hit;
extern sfxSound			sfx_chicken;
extern sfxSound			sfx_transform;
extern sfxSound			sfx_yoshi;
extern sfxSound			sfx_pause;
extern sfxSound			sfx_bobombsound;
extern sfxSound			sfx_areatag;
extern sfxSound			sfx_cannon;
extern sfxSound			sfx_burnup;
extern sfxSound			sfx_pipe;
extern sfxSound			sfx_thunder;
extern sfxSound			sfx_slowdownmusic;
extern sfxSound			sfx_flyingsound;
extern sfxSound			sfx_storedpowerupsound;
extern sfxSound			sfx_kicksound;
extern sfxSound			sfx_racesound;
extern sfxSound			sfx_bulletbillsound;
extern sfxSound			sfx_boomerang;
extern sfxSound			sfx_spit;
extern sfxSound			sfx_starwarning;
extern sfxSound			sfx_powerdown;
extern sfxSound			sfx_switchpress;
extern sfxSound			sfx_superspring;
extern sfxSound			sfx_stun;
extern sfxSound			sfx_inventory;
extern sfxSound			sfx_worldmove;
extern sfxSound			sfx_treasurechest;
extern sfxSound			sfx_flamecannon;
extern sfxSound			sfx_wand;
extern sfxSound			sfx_enterstage;
extern sfxSound			sfx_gameover;
extern sfxSound			sfx_pickup;

extern sfxMusic			backgroundmusic[6];

extern gfxSprite        spr_tanooki, spr_statue;
extern gfxSprite		spr_starpowerup;
extern gfxSprite		spr_1uppowerup;
extern gfxSprite		spr_2uppowerup;
extern gfxSprite		spr_3uppowerup;
extern gfxSprite		spr_5uppowerup;
extern gfxSprite		spr_firepowerup;
extern gfxSprite		spr_hammerpowerup;
extern gfxSprite		spr_icewandpowerup;
extern gfxSprite		spr_podobopowerup;
extern gfxSprite		spr_poisonpowerup;
extern gfxSprite		spr_mysterymushroompowerup;
extern gfxSprite		spr_boomerangpowerup;
extern gfxSprite		spr_clockpowerup;
extern gfxSprite		spr_bobombpowerup;
extern gfxSprite		spr_powpowerup;
extern gfxSprite		spr_modpowerup;
extern gfxSprite		spr_bulletbillpowerup;
extern gfxSprite		spr_featherpowerup;
extern gfxSprite		spr_leafpowerup;
extern gfxSprite		spr_bombpowerup;
extern gfxSprite		spr_pwingspowerup;
extern gfxSprite		spr_extraheartpowerup;
extern gfxSprite		spr_extratimepowerup;
extern gfxSprite		spr_jailkeypowerup;

extern gfxSprite		spr_secret1;
extern gfxSprite		spr_secret2;
extern gfxSprite		spr_secret3;
extern gfxSprite		spr_secret4;

extern gfxSprite		spr_shade[3];
extern gfxSprite		spr_scorehearts;
extern gfxSprite		spr_scorecards;
extern gfxSprite		spr_scorecoins;

extern gfxSprite		spr_timershade;
extern gfxSprite		spr_scoretext;
extern gfxSprite		spr_racetext;
extern gfxSprite		spr_crown;
extern gfxSprite		spr_cape;
extern gfxSprite		spr_tail;
extern gfxSprite		spr_wings;

extern gfxSprite		spr_egg;
extern gfxSprite		spr_eggnumbers;
extern gfxSprite		spr_star;
extern gfxSprite		spr_flags;
extern gfxSprite		spr_frenzycards;
extern gfxSprite		spr_collectcards;
extern gfxSprite		spr_yoshi;
extern gfxSprite		spr_coin;
extern gfxSprite		spr_thwomp;
extern gfxSprite		spr_podobo;
extern gfxSprite		spr_bowserfire;
extern gfxSprite		spr_areas;
extern gfxSprite		spr_kingofthehillarea;
extern gfxSprite		spr_jail;
extern gfxSprite		spr_racegoal;
extern gfxSprite		spr_pipegamebonus;
extern gfxSprite		spr_chicken;
extern gfxSprite		spr_phanto, spr_phantokey;

extern gfxSprite		spr_bonuschest;
extern gfxSprite		spr_teleportstar;

extern gfxSprite		spr_goomba;
extern gfxSprite		spr_goombadead;
extern gfxSprite		spr_goombadeadflying;
extern gfxSprite		spr_koopa;
extern gfxSprite		spr_buzzybeetle;
extern gfxSprite		spr_spiny;
extern gfxSprite		spr_paragoomba;
extern gfxSprite		spr_parakoopa;
extern gfxSprite		spr_redparakoopa;
extern gfxSprite		spr_sledgebrothers;
extern gfxSprite		spr_sledgebrothersdead;
extern gfxSprite		spr_redkoopa;
extern gfxSprite		spr_cheepcheep;
extern gfxSprite		spr_cheepcheepdead;
extern gfxSprite		spr_bulletbill;
extern gfxSprite		spr_bulletbilldead;

extern gfxSprite		spr_fireball;
extern gfxSprite		spr_superfireball;
extern gfxSprite		spr_hammer;
extern gfxSprite		spr_iceblast;
extern gfxSprite		spr_boomerang;
extern gfxSprite		spr_shell;
extern gfxSprite		spr_shelldead;
extern gfxSprite		spr_blueblock;
extern gfxSprite		spr_spring;
extern gfxSprite		spr_spike;
extern gfxSprite		spr_bomb;
extern gfxSprite		spr_kuriboshoe;
extern gfxSprite		spr_throwbox;

extern gfxSprite		spr_sledgehammer;
extern gfxSprite		spr_superfireball;

extern gfxSprite		spr_hazard_fireball[3];
extern gfxSprite		spr_hazard_rotodisc[3];
extern gfxSprite		spr_hazard_bulletbill[3];
extern gfxSprite		spr_hazard_bulletbilldead;
extern gfxSprite		spr_hazard_flame[3];
extern gfxSprite		spr_hazard_pirhanaplant[3];

extern gfxSprite		spr_fireballexplosion;
extern gfxSprite		spr_frictionsmoke;
extern gfxSprite		spr_bobombsmoke;
extern gfxSprite		spr_explosion;
extern gfxSprite		spr_burnup;
extern gfxSprite		spr_fireworks;
extern gfxSprite		spr_poof;

extern gfxSprite		spr_spawnsmoke;
extern gfxSprite		spr_spawndoor;
extern gfxSprite		spr_bonus;
extern gfxSprite		spr_extralife;
extern gfxSprite		spr_award;
extern gfxSprite		spr_awardsolid;
extern gfxSprite		spr_awardsouls;
extern gfxSprite		spr_awardsoulspawn;
extern gfxSprite		spr_awardkillsinrow;
extern gfxSprite		spr_flagbases;
extern gfxSprite		spr_ownedtags;
extern gfxSprite		spr_warplock;
extern gfxSprite		spr_coinsparkle;
extern gfxSprite		spr_shinesparkle;
extern gfxSprite		spr_shellbounce;
extern gfxSprite		spr_superstomp;

extern gfxSprite		spr_storedpowerupsmall;
extern gfxSprite		spr_storedpoweruplarge;
extern gfxSprite		spr_powerupselector;

extern gfxSprite		spr_scoreboard;
extern gfxSprite		spr_abovearrows;

extern gfxSprite		spr_windmeter;
extern gfxSprite		spr_overlayhole;

#ifndef _SMW_EDITOR
extern gfxFont			menu_font_small;
extern gfxFont			menu_font_large;
extern gfxFont			game_font_small;
extern gfxFont			game_font_large;

extern gfxSprite		spr_noteblock;
extern gfxSprite		spr_breakableblock;
extern gfxSprite		spr_powerupblock;
extern gfxSprite		spr_donutblock;
extern gfxSprite		spr_flipblock;
extern gfxSprite		spr_bounceblock;
extern gfxSprite		spr_throwblock;
extern gfxSprite		spr_switchblocks;
extern gfxSprite		spr_viewblock;
extern gfxSprite		spr_weaponbreakableblock;
#endif

extern gfxSprite		spr_brokenyellowblock;
extern gfxSprite		spr_brokenflipblock;
extern gfxSprite		spr_brokenblueblock;
extern gfxSprite		spr_brokengrayblock;

extern gfxSprite		spr_brokeniceblock;
extern gfxSprite		spr_iceblock;

extern gfxSprite		spr_tileanimation[3];
extern gfxSprite		spr_blocks[3];
extern gfxSprite		spr_unknowntile[3];

extern gfxSprite		** spr_player[4];
extern gfxSprite		** spr_shyguy[4];
extern gfxSprite		** spr_chocobo[4];
extern gfxSprite		** spr_bobomb[4];
extern gfxSprite		spr_clouds;
extern gfxSprite		spr_ghosts;
extern gfxSprite		spr_fish;
extern gfxSprite		spr_leaves;
extern gfxSprite		spr_snow;
extern gfxSprite		spr_rain;

extern gfxSprite		spr_background;
extern gfxSprite		spr_backmap[2];
extern gfxSprite		spr_frontmap[2];
extern gfxSprite		menu_backdrop;
extern gfxSprite		spr_overlay;

extern gfxSprite		menu_shade;
extern gfxSprite		menu_smw;
extern gfxSprite		menu_version;
extern gfxSprite		menu_slider_bar;
extern gfxSprite		menu_plain_field;
extern gfxSprite		menu_player_select;
extern gfxSprite		menu_map_filter;
extern gfxSprite		menu_match_select;
extern gfxSprite		menu_dialog;
extern gfxSprite		menu_verticalarrows;

extern gfxSprite		menu_stomp;
extern gfxSprite		menu_survival;
extern gfxSprite		menu_egg;

extern gfxSprite		menu_mode_small;
extern gfxSprite		menu_mode_large;
extern gfxSprite		spr_dialog;
extern gfxSprite		spr_dialogbutton;
extern gfxSprite		spr_tournament_background;
extern gfxSprite		spr_tournament_powerup_splash;
extern gfxSprite		spr_player_select_background;
extern gfxSprite		spr_player_select_ready;
//extern gfxSprite		spr_ipfield;
extern gfxSprite		spr_selectfield;
extern gfxSprite		spr_selectfielddisabled;
extern gfxSprite		spr_map_filter_icons;
extern gfxSprite		spr_tour_markers;
extern gfxSprite		spr_menu_boxed_numbers;
extern gfxSprite		spr_countdown_numbers;
extern gfxSprite		spr_thumbnail_warps[2];
extern gfxSprite		spr_thumbnail_mapitems[2];
extern gfxSprite		spr_platformstarttile;
extern gfxSprite		spr_platformendtile;
extern gfxSprite		spr_platformpath;

extern gfxSprite		spr_worldbackground[3];
extern gfxSprite		spr_worldforeground[3];
extern gfxSprite		spr_worldforegroundspecial[3];
extern gfxSprite		spr_worldpaths[3];
extern gfxSprite		spr_worldvehicle[3];

extern gfxSprite		spr_worlditems;
extern gfxSprite		spr_worlditempopup;
extern gfxSprite		spr_worlditemssmall;
extern gfxSprite		spr_worlditemsplace;
extern gfxSprite		spr_worldbonushouse;
extern gfxSprite		spr_announcementicons;

extern MapList *maplist;
extern SkinList *skinlist;
extern AnnouncerList *announcerlist;
extern MusicList *musiclist;
extern WorldMusicList *worldmusiclist;
extern GraphicsList *menugraphicspacklist;
extern GraphicsList *worldgraphicspacklist;
extern GraphicsList *gamegraphicspacklist;
extern SoundsList *soundpacklist;
extern TourList *tourlist;
extern WorldList *worldlist;
extern FiltersList *filterslist;

extern CGameMode * gamemodes[GAMEMODE_LAST];
extern CGM_Bonus * bonushousemode;
extern CGM_Pipe_MiniGame * pipegamemode;
extern CGM_Boss_MiniGame * bossgamemode;
extern CGM_Boxes_MiniGame * boxesgamemode;
extern short currentgamemode;

extern float CapFallingVelocity(float vel);
extern float CapSideVelocity(float vel);

extern int g_iNextNetworkID;
extern int g_iNextMessageID;
extern char szIPString[32];
//extern NetServer netServer;
//extern NetClient netClient;

extern short g_iDefaultPowerupPresets[NUM_POWERUP_PRESETS][NUM_POWERUPS];
extern short g_iCurrentPowerupPresets[NUM_POWERUP_PRESETS][NUM_POWERUPS];

extern short g_iCurrentDrawIndex;

void _load_drawmsg(const std::string& f);
void _load_waitforkey();

void DrawPlatform(short pathtype, TilesetTile ** tiles, short startX, short startY, short endX, short endY, float angle, float radiusX, float radiusY, short iSize, short iPlatformWidth, short iPlatformHeight, bool fDrawPlatform, bool fDrawShadow);
void DrawMapHazard(MapHazard * hazard, short iSize, bool fDrawCenter);

//----------------- game options all parts of the game need -----------
enum gs{GS_MENU, GS_START_GAME, GS_START_WORLD, GS_END_GAME, GS_GAME, GS_QUIT};

//tournament scores
struct ts
{
	short		wins;
	short		type[MAXTOURNAMENTGAMES];
	short		total;		//used for running total in a tour
};

struct ClassicGameModeSettings
{
	short style;			//on kill, either respawn the player or shield them and let them keep playing
	short scoring;			//When to credit a score, all kills or push kills only (sumo mode)
};

struct FragGameModeSettings
{
	short style;			//on kill, either respawn the player or shield them and let them keep playing
	short scoring;			//When to credit a score, all kills or push kills only (sumo mode)
};

struct TimeGameModeSettings
{
	short style;			//on kill, either respawn the player or shield them and let them keep playing
	short scoring;			//When to credit a score, all kills or push kills only (sumo mode)
	short percentextratime; //percent chance a stopwatch with extra game time will spawn
};

struct JailGameModeSettings
{
	short style;			//Style of play: classic, owned, free for all
	bool tagfree;			//Free jailed player by tagging on same team
	short timetofree;		//Time it takes for a jailed player to be freed
	short percentkey;		//percent chance a jail key will spawn
};

struct CoinGameModeSettings
{
	bool penalty;			//Player loses coin if stomped on
	short quantity;			//Number of coins to have on screen at one time
	short percentextracoin; //percent chance a bonus coin will appear
};

struct StompGameModeSettings
{
	short rate;				//How fast they spawn
	short enemyweight[NUMSTOMPENEMIES];	//What ratio the enemies are chosen
};

struct EggGameModeSettings
{
	short eggs[4];			//Number of eggs of each color in game
	short yoshis[4];		//Nunber of yoshis of each color in game
	short explode;			//Time until eggs explode
};

struct FlagGameModeSettings
{
	short speed;			//How fast to move bases
	bool touchreturn;		//Return to base if player touches their own flag
	bool pointmove;			//Move base after point
	short autoreturn;		//Time to automatically return flag to base
	bool homescore;			//Need your flag at home base to score
	bool centerflag;		//Have a single flag that all teams fight to return to their base
};

struct ChickenGameModeSettings
{
	bool usetarget;			//Display target around chicken
	bool glide;				//Allow chicken to glide through the air (chicken gets perm leaf powerup)
};

struct TagGameModeSettings
{
	bool tagontouch;		//Transfer tag on touch
};

struct StarGameModeSettings
{
	short time;				//Time to get rid or keep star
	short shine;			//If we are playing shine mode vs. ztar vs. stars mode
	short percentextratime; //percent chance a stopwatch with extra game time will spawn
};

struct DominationGameModeSettings
{
	bool loseondeath;		//Player loses his bases on death
	bool stealondeath;		//Killing player steals bases on death
	bool relocateondeath;	//Owned bases relocate on player's death
	short relocationfrequency; //How fast the bases switch spots
	short quantity;			//Number of bases
};

struct KingOfTheHillModeSettings
{
	short areasize;			//size of the hill area, 2x2, 3x3 etc.
	short relocationfrequency;  //How often the hill switches spots
	short maxmultiplier;	//as a player sits in the zone, the score will multiply
};

struct RaceGameModeSettings
{
	short quantity;			//How many race goals
	short speed;			//How fast goals move
	short penalty;			//If dead players lose all tagged, one tagged or none
};

struct FrenzyGameModeSettings
{
	short quantity;			//How many powerups spawn
	short rate;				//How fast they spawn
	bool storedshells;		//Shells become stored instead of active
	short powerupweight[NUMFRENZYCARDS];	//What ratio the powerups are chosen
};

struct SurvivalGameModeSettings
{
	short enemyweight[NUMSURVIVALENEMIES];       //The weighting of thwomps vs. podobos
	short density;				//Number of thwomps on screen
	short speed;				//How fast thwomps go
	bool shield;				//Players are shielded when spawning
};

struct GreedGameModeSettings
{
	short coinlife;				//How long coins stick around before disappearing
	bool owncoins;				//You can/can't pick up your own coins
	short multiplier;			//How many coins are released when players are hit
	short percentextracoin;		//percent chance a bonus coin will appear
};

struct HealthGameModeSettings
{
	short startlife;			//Number of hearts players start with
	short maxlife;				//Number of hearts players can have
	short percentextralife;		//Percent chance an extra heart with come from a powerup block
};

struct CollectionGameModeSettings
{
	short quantity;				//How many cards spawn
	short rate;					//How fast they spawn
	short banktime;				//Amount of time needed to bank cards to score
	short cardlife;				//How long cards live after they are released from players
};

struct ChaseGameModeSettings
{
	short phantospeed;			//How fast the phantos move
	short phantoquantity[3];	//How many phantos there are
};

struct ShyGuyTagGameModeSettings
{
	bool tagonsuicide;			//If players become shyguys if they kill themselves
	short tagtransfer;			//If players become shyguys if they are stomped
	short freetime;				//How long all players will stay shyguys before the game is reset
};

struct BossGameModeSettings
{
	short bosstype;				//What type of boss battle it is
	short difficulty;			//How hard the boss is to defeat
	short hitpoints;			//How much life the boss has
};

struct GameModeSettings
{
	ClassicGameModeSettings classic;
	FragGameModeSettings frag;
	TimeGameModeSettings time;
	JailGameModeSettings jail;
	CoinGameModeSettings coins;
	StompGameModeSettings stomp;
	EggGameModeSettings egg;
	FlagGameModeSettings flag;
	ChickenGameModeSettings chicken;
	TagGameModeSettings tag;
	StarGameModeSettings star;
	DominationGameModeSettings domination;
	KingOfTheHillModeSettings kingofthehill;
	RaceGameModeSettings race;
	FrenzyGameModeSettings frenzy;
	SurvivalGameModeSettings survival;
	GreedGameModeSettings greed;
	HealthGameModeSettings health;
	CollectionGameModeSettings collection;
	ChaseGameModeSettings chase;
	ShyGuyTagGameModeSettings shyguytag;
	BossGameModeSettings boss;
};

struct WorldStageBonus
{
	short iWinnerPlace;
	short iBonus;
	char szBonusString[8];
};

struct TourStop
{
	const char * pszMapFile;
	short iMode;
	short iGoal;
	short iPoints;
	short iBonusType;
	char szName[128];
	
	bool fEndStage;
	short iNumBonuses;
	WorldStageBonus wsbBonuses[10];
	short iStageType;

	bool fUseSettings;
	short iNumUsedSettings;
	GameModeSettings gmsSettings;

	short iBonusTextLines;
	char szBonusText[5][128];
};

//TODO:: Move menu settings form game_values to global menu context structure
//menu context (like game_values, but for menu)
/*
struct MenuContext
{
	
};

extern MenuContext menu_context;
*/

struct gv
{
	short		playercontrol[4];
	bool		showfps;
	bool		frameadvance;
	bool		autokill;
	short		framelimiter;
	
	bool		sound;
	bool		music;

	short		soundvolume;
	short		musicvolume;

	bool		fullscreen;
	gs			gamestate;

	float		screenResizeX;
	float		screenResizeY;
	float		screenResizeW;
	float		screenResizeH;

	short		flickerfilter;
	short		hardwarefilter;
	short		softfilter;
	bool		aspectratio10x11;

	CGameMode	*gamemode;

	short		spawnstyle;
	short		awardstyle;

	short		shieldtime;
	short		shieldstyle;

	bool		pausegame;
	bool		exitinggame;
	bool		exityes;

	bool		showscoreboard;
	float		scorepercentmove;

	short		matchtype;	//The currently selected match type: quick game, single, tournament, tour, world, minigame

	short		tournamentgames;	//How many games that are played in this tournament
	short		tournamentwinner;     //-2 for a tied tournament (for tours), -1 for no winner yet, 0 or greater for the team that has won the tournament

	short		tournamentcontrolteam;   //The team ID that currently has control
	short		tournamentcontrolstyle;  //ID for the player selected control style
	short		tournamentnextcontrol;  //For round robin control style
	
	short		selectedminigame;

	short		tourindex;
	short		tourstopcurrent;
	short		tourstoptotal;
	std::vector<TourStop*> tourstops;
	
	short		worldindex;

	short		slowdownon;
	short		slowdowncounter;

	short		storedpowerups[4];
	short		gamepowerups[4];
	short		powerupweights[NUM_POWERUPS];
	short		poweruppreset;

	short		worldpowerups[4][32];
	short		worldpowerupcount[4];

	short		worldpointsbonus;

	short		teamids[4][3];
	short		teamcounts[4];
	short		skinids[4];
	short		colorids[4];
	bool		randomskin[4];

	bool		screencrunch;
	short		screenshaketimer;
	short		screenshakeplayerid;
	short		screenshaketeamid;
	bool		screenshakekillinair;
	short		screenshakekillscount;

	short		bulletbilltimer[4];
	short		bulletbillspawntimer[4];

	short		teamcollision;
	bool		toplayer;
	short		scoreboardstyle;
	bool		teamcolors;

	short		loadedannouncer;
	short		loadedmusic;

	short		teamdeadcounter;

	short		respawn;
	short		itemrespawntime;
	short		outofboundstime;
	short		hiddenblockrespawn;

	short		warplockstyle;
	short		warplocktime;

	short		cputurn;
	short		cpudifficulty;

	short		networktype;		//Type of network game: 0=stand alone, 1=direct connect
	bool		networkhost;		//If this machine is the one making collision detection and game state changes
	bool		gamehost;			//If this client is responsible for choosing game type, map, options
	char *		hostaddress;        //String form of the host ip address

	CInputPlayerControl inputConfiguration[4][2]; //[NumPlayers][Keyboard/Joystick]

	//Player input used during game.  Reads SDL_Events and sets buttons that were pressed
	CPlayerInput playerInput;

	ts			tournament_scores[4];

	GameModeSettings gamemodesettings;
	GameModeSettings gamemodemenusettings;
	
	short		fireballttl;
	short		fireballlimit;

	short		hammerdelay;
	short		hammerttl;
	bool		hammerpower;
	short		hammerlimit;

	short		boomerangstyle;
	short		boomeranglife;
	short		boomeranglimit;

	short		featherjumps;
	short		featherlimit;

	short		leaflimit;
	short		pwingslimit;

	short		tanookilimit;
	short		bombslimit;
	
	short		wandfreezetime;
	short		wandlimit;

	short		shellttl;
	short		blueblockttl;
	short		redblockttl;
	short		grayblockttl;

	bool		playskidsound;
	bool		playinvinciblesound;
	bool		playflyingsound;

	short		swapstyle;
	bool		swapplayers;
	float		swapplayersposition;
	bool		swapplayersblink;
	short		swapplayersblinkcount;

	short		screenfade;
	short		screenfadespeed;

	short		storedpowerupdelay;
	short		bonuswheel;
	bool		keeppowerup;

	bool		showwinningcrown;

	bool		playnextmusic;  //automatically advance to the next music track after one finishes

	bool		soundcapable;

	bool *		pfFilters;
	short *		piFilterIcons;
	short		selectedmapfilter;
	bool		fNeedWriteFilters;
	bool		fFiltersOn;

	short		pointspeed;

	bool		noexit;
	short		noexittimer;
	short		forceexittimer;

	short		singleplayermode;

	bool		worldskipscoreboard;

	short		overridepowerupsettings;

	float		gamewindx;
	float		gamewindy;

	bool		windaffectsplayers;
	bool		spinscreen;
	bool		reversewalk;
	bool		spotlights;

	short		suicidetime;
	bool		minigameunlocked;

	bool		startgamecountdown;
	bool		startmodedisplay;
	bool		deadteamnotice;

	bool		unlocksecret1part1[4];
	short		unlocksecret1part2;
	bool		unlocksecret2part1;
	short		unlocksecret2part2;
	short		unlocksecret3part1[4];
	short		unlocksecret3part2[4];
	bool		unlocksecretunlocked[4];
};

extern gv game_values;

struct MapMusicOverride
{
	std::string mapname;
	std::vector<std::string> songs;
};

struct WorldMusicOverride
{
	std::string worldname;
	std::string song;
};

extern std::vector<MapMusicOverride*> mapmusicoverrides;
extern std::vector<WorldMusicOverride*> worldmusicoverrides;

#endif

