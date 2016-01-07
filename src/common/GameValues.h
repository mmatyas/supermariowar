#ifndef GAMEVALUES_H
#define GAMEVALUES_H

#include "GameModeSettings.h"
#include "input.h"

#include <vector>

#define ifSoundOnPlay(x) if (game_values.sound) x.play()
#define ifsoundonandreadyplay(x) (x.isready() && game_values.sound ? x.play() : -1)
#define ifSoundOnPlayLoop(x,y) (game_values.sound ? x.playloop(y) : -1)
#define ifsoundonstop(x) (game_values.sound ? x.stop() : void(NULL))
#define ifsoundonpause(x) (game_values.sound ? x.sfx_pause() : void(NULL))
#define ifmusiconplay(x) (game_values.music ? x.play() : -1)

class CGameMode;

enum CurrentGameState {
    GS_SPLASH,
    GS_MENU,
    GS_START_GAME,
    GS_START_WORLD,
    GS_END_GAME,
    GS_GAME,
    GS_QUIT
};

enum MatchType {
    MATCH_TYPE_SINGLE_GAME = 0,
    MATCH_TYPE_TOURNAMENT = 1,
    MATCH_TYPE_TOUR = 2,
    MATCH_TYPE_WORLD = 4,
    MATCH_TYPE_MINIGAME = 3,
    MATCH_TYPE_QUICK_GAME = 5,
    MATCH_TYPE_NET_GAME = 6
};

//tournament scores
struct TournamentScores {
    short       wins;
    short       type[MAXTOURNAMENTGAMES];
    short       total;      //used for running total in a tour
};

struct WorldStageBonus {
    short iWinnerPlace;
    short iBonus;
    char szBonusString[8];
};

struct TourStop {
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

enum awardstyle {
    award_style_none = 0,
    award_style_fireworks = 1,
    award_style_swirl = 2,
    award_style_halo = 3,
    award_style_souls = 4,
    award_style_text = 5
};


// the real configuration class
class CGameConfig {
public:
    bool		showfps;
    short		soundvolume;
    short		musicvolume;
    bool		fullscreen;
    short		spawnstyle;
    short		awardstyle;
    short		teamcollision;
    short		shellttl;
    short		blueblockttl;
    short		redblockttl;
    short		grayblockttl;
    short		bombslimit;
    short		bonuswheel;
    short		fireballttl;
    short		fireballlimit;
    short		boomerangstyle;
    short		boomeranglife;
    short		boomeranglimit;
    short		cpudifficulty;
    short		warplockstyle;
    short		warplocktime;
    short		hammerttl;
    short		hammerdelay;
    short		hammerlimit;
    bool		hammerpower;
    short		featherjumps;
    short		featherlimit;
    short		suicidetime;
    bool		deadteamnotice;
    short		framelimiter;
    short		hiddenblockrespawn;
    CInputPlayerControl inputConfiguration[4][2]; //[NumPlayers][Keyboard/Joystick]
    short		itemrespawntime;
    bool		keeppowerup;
    short		leaflimit;
    short		pwingslimit;
    short		tanookilimit;
    short		playercontrol[4];
    CPlayerInput playerInput;
    bool		playnextmusic;  //automatically advance to the next music track after one finishes
    short		outofboundstime;
    short		overridepowerupsettings;
    bool		minigameunlocked;
    GameModeSettings gamemodemenusettings;
    bool		music;
    short		pointspeed;
    short		poweruppreset;
    bool		randomskin[4];
    short		scoreboardstyle;
    bool		screencrunch;
    short		shieldtime;
    short		shieldstyle;
    bool		showwinningcrown;
	short		skinids[4];
    short		wandlimit;
    short		tournamentcontrolstyle;  //ID for the player selected control style
    short		wandfreezetime;
    short		teamids[4][3];
    bool		toplayer;
    bool		teamcolors;
    bool		sound;
    bool		startgamecountdown;
    bool		startmodedisplay;
    short		storedpowerupdelay;
    short		teamcounts[4];
    short		swapstyle;
    short		respawn;
};

// these are actually the program options
class CGameValues : public CGameConfig
{
public:
    void init();

    void ReadBinaryConfig();
    void WriteConfig();

    void SetupDefaultGameModeSettings();
    void resetGameplaySettings();
    void resetSecretCounters();

    bool		frameadvance;
    bool		autokill;

    CurrentGameState   gamestate;

    float		screenResizeX;
    float		screenResizeY;
    float		screenResizeW;
    float		screenResizeH;

    short		flickerfilter;
    short		hardwarefilter;
    short		softfilter;
    bool		aspectratio10x11;

    CGameMode   *gamemode;

    bool		pausegame;
    bool		exitinggame;
    bool		exityes;

    bool		showscoreboard;
    float		scorepercentmove;

    short		matchtype;	//The currently selected match type: quick game, single, tournament, tour, world, minigame

    short		tournamentgames;	//How many games that are played in this tournament
    short		tournamentwinner;     //-2 for a tied tournament (for tours), -1 for no winner yet, 0 or greater for the team that has won the tournament

    short		tournamentcontrolteam;   //The team ID that currently has control
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

    short		worldpowerups[4][32];
    short		worldpowerupcount[4];

    short		worldpointsbonus;

    short		colorids[4];

    short		screenshaketimer;
    short		screenshakeplayerid;
    short		screenshaketeamid;
    bool		screenshakekillinair;
    short		screenshakekillscount;

    short		bulletbilltimer[4];
    short		bulletbillspawntimer[4];

    short		loadedannouncer;
    short		loadedmusic;

    short		teamdeadcounter;

    short		cputurn;


    //Player input used during game.  Reads SDL_Events and sets buttons that were pressed

    TournamentScores tournament_scores[4];

    GameModeSettings gamemodesettings;

	bool		playskidsound;
    bool		playinvinciblesound;
    bool		playflyingsound;

    bool		swapplayers;
    float		swapplayersposition;
    bool		swapplayersblink;
    short		swapplayersblinkcount;

    short		screenfade;
    short		screenfadespeed;

    bool		soundcapable;

    bool *		pfFilters;
    short *		piFilterIcons;
    short		selectedmapfilter;
    bool		fNeedWriteFilters;
    bool		fFiltersOn;

    bool		noexit;
    short		noexittimer;
    short		forceexittimer;

    short		singleplayermode;

    bool		worldskipscoreboard;

    float		gamewindx;
    float		gamewindy;

    bool		windaffectsplayers;
    bool		spinscreen;
    bool		reversewalk;
    bool		spotlights;

    bool		unlocksecret1part1[4];
    short		unlocksecret1part2;
    bool		unlocksecret2part1;
    short		unlocksecret2part2;
    short		unlocksecret3part1[4];
    short		unlocksecret3part2[4];
    bool		unlocksecretunlocked[4];
};

#endif // GAMEVALUES_H
