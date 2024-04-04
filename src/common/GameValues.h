#ifndef GAMEVALUES_H
#define GAMEVALUES_H

#include "EyecandyStyles.h"
#include "GameModeSettings.h"
#include "GameplayStyles.h"
#include "input.h"
#include "MatchTypes.h"

#include <vector>

class sfxSound;

void ifSoundOnPlay(sfxSound&);
#define ifsoundonandreadyplay(x) (x.isready() && game_values.sound ? x.play() : -1)
#define ifSoundOnPlayLoop(x,y) (game_values.sound ? x.playloop(y) : -1)
#define ifsoundonstop(x) (game_values.sound ? x.stop() : void(NULL))
#define ifsoundonpause(x) (game_values.sound ? x.sfx_pause() : void(NULL))
#define ifmusiconplay(x) (game_values.music ? x.play() : -1)

class CGameMode;

enum class AppState: unsigned char {
    Splash,
    Menu,
    StartGame,
    StartWorld,
    EndGame,
    Game,
    Quit,
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


/// Gameplay-specific variables, which are reset on the start of a game.
struct GameplayFlags {
    bool pausegame = false;
    bool exitinggame = false;
    bool exityes = false;

    bool noexit = false;
    short noexittimer = 0;
    short forceexittimer = 0;

    short teamdeadcounter = 0;

    short screenshaketimer = 0;
    short screenshakeplayerid = -1;
    short screenshaketeamid = -1;
    short screenshakekillscount = 0;
    bool screenshakekillinair = false;

    short slowdownon = -1;
    short slowdowncounter = 0;

    bool showscoreboard = false;
    float scorepercentmove = 0.0f;

    bool playskidsound = false;
    bool playinvinciblesound = false;
    bool playflyingsound = false;

    bool swapplayers = false;
    float swapplayersposition = 0.0f;
    bool swapplayersblink = false;
    short swapplayersblinkcount = 0;

    float gamewindx = 0.0f;
    float gamewindy = 0.0f;

    bool windaffectsplayers = false;
    bool spinscreen = false;
    bool reversewalk = false;
    bool spotlights = false;
};


// the real configuration class
class CGameConfig {
public:
    bool		showfps;
    short		soundvolume;
    short		musicvolume;
    bool		fullscreen;
    SpawnStyle spawnstyle;
    AwardStyle awardstyle;
    TeamCollisionStyle teamcollision;
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
    bool		secretsenabled;
    GameModeSettings gamemodemenusettings;
    bool		music;
    short		pointspeed;
    short		poweruppreset;
    bool		randomskin[4];
    ScoreboardStyle scoreboardstyle;
    bool		screencrunch;
    short		shieldtime;
    short		shieldstyle;
    bool		showwinningcrown;
	short		skinids[4];
    short		wandlimit;
    TournamentControlStyle tournamentcontrolstyle;  //ID for the player selected control style
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

    AppState appstate = AppState::Splash;

    GameplayFlags flags;
    CGameMode   *gamemode;

    short screenfade = 0;
    short screenfadespeed = 0;

    MatchType matchtype;	//The currently selected match type: quick game, single, tournament, tour, world, minigame

    short		tournamentgames;	//How many games that are played in this tournament
    short		tournamentwinner;     //-2 for a tied tournament (for tours), -1 for no winner yet, 0 or greater for the team that has won the tournament

    short		tournamentcontrolteam;   //The team ID that currently has control
    short		tournamentnextcontrol;  //For round robin control style

    Minigame selectedminigame;

    short		tourindex;
    short		tourstopcurrent;
    short		tourstoptotal;
    std::vector<TourStop*> tourstops;

    short		worldindex;

    short		storedpowerups[4];
    short		gamepowerups[4];
    short		powerupweights[NUM_POWERUPS];

    short		worldpowerups[4][32];
    short		worldpowerupcount[4];

    short		worldpointsbonus;

    short		colorids[4];

    short		bulletbilltimer[4];
    short		bulletbillspawntimer[4];

    short		loadedannouncer;
    short		loadedmusic;

    short		cputurn;


    //Player input used during game.  Reads SDL_Events and sets buttons that were pressed

    TournamentScores tournament_scores[4];

    GameModeSettings gamemodesettings;

    bool		soundcapable;

    bool *		pfFilters;
    short *		piFilterIcons;
    short		selectedmapfilter;
    bool		fNeedWriteFilters;
    bool		fFiltersOn;

    short		singleplayermode;

    bool		worldskipscoreboard;

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
