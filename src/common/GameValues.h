#ifndef GAMEVALUES_H
#define GAMEVALUES_H

#define TITLESTRING "Super Mario War"

#include "EyecandyStyles.h"
#include "GlobalConstants.h"
#include "GameModeSettings.h"
#include "GameplayStyles.h"
#include "input.h"
#include "MatchTypes.h"

#include <array>
#include <string>
#include <vector>

class sfxSound;
class CGameMode;
struct TourStop;

void ifSoundOnPlay(sfxSound&);

short defaultPowerupSetting(size_t presetIdx, size_t powerupIdx);

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
    BonusWheelStyle bonuswheel;
    short		fireballttl;
    short		fireballlimit;
    BoomerangStyle boomerangstyle;
    short		boomeranglife;
    short		boomeranglimit;
    short		cpudifficulty;
    WarpLockStyle warplockstyle;
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
    CInputPlayerControl inputConfiguration[MAX_PLAYERS][2]; //[NumPlayers][Keyboard/Joystick]
    short		itemrespawntime;
    bool		keeppowerup;
    short		leaflimit;
    short		pwingslimit;
    short		tanookilimit;
    short		playercontrol[MAX_PLAYERS];
    CPlayerInput playerInput;
    bool		playnextmusic;  //automatically advance to the next music track after one finishes
    short		outofboundstime;
    PowerupDropRateStyle overridepowerupsettings;
    bool		secretsenabled;
    GameModeSettings gamemodemenusettings;
    bool		music;
    short		pointspeed;
    short poweruppreset;
    std::array<std::array<short, NUM_POWERUPS>, NUM_POWERUP_PRESETS> allPowerupPresets;
    bool		randomskin[MAX_PLAYERS];
    ScoreboardStyle scoreboardstyle;
    bool		screencrunch;
    short		shieldtime;
    ShieldStyle shieldstyle;
    bool		showwinningcrown;
	short		skinids[MAX_PLAYERS];
    short		wandlimit;
    TournamentControlStyle tournamentcontrolstyle;  //ID for the player selected control style
    short		wandfreezetime;
    bool		toplayer;
    bool		teamcolors;
    bool		sound;
    bool		startgamecountdown;
    bool		startmodedisplay;
    short		storedpowerupdelay;
    MysteryMushroomSwapStyle swapstyle;
    short		respawn;

    void ReadConfig();
    void WriteConfig() const;

private:
    void ReadInputConfig();
    void ReadBinaryConfig();
    void ReadTomlConfig();

    void WriteInputConfig() const;
    void WriteTomlConfig() const;
};

// these are actually the program options
class CGameValues : public CGameConfig
{
public:
    void init();

    void resetGameplaySettings();
    void resetSecretCounters();

    bool showfps;
    bool		frameadvance;
    bool		autokill;

    AppState appstate = AppState::Splash;

    GameplayFlags flags;
    CGameMode   *gamemode;

    short screenfade = 0;
    short screenfadespeed = 0;

    MatchType matchtype;	//The currently selected match type: quick game, single, tournament, tour, world, minigame

    short		teamids[MAX_PLAYERS][MAX_TEAM_SIZE];
    short		teamcounts[MAX_PLAYERS];

    short		tournamentgames;	//How many games that are played in this tournament
    short		tournamentwinner;     //-2 for a tied tournament (for tours), -1 for no winner yet, 0 or greater for the team that has won the tournament

    short		tournamentcontrolteam;   //The team ID that currently has control
    short		tournamentnextcontrol;  //For round robin control style

    Minigame selectedminigame;

    short		tourindex;
    size_t		tourstopcurrent;
    std::vector<TourStop*> tourstops;

    short		worldindex;

    short		storedpowerups[MAX_PLAYERS];
    short		gamepowerups[MAX_PLAYERS];
    short		powerupweights[NUM_POWERUPS];

    short		worldpowerups[MAX_PLAYERS][32];
    short		worldpowerupcount[MAX_PLAYERS];

    short		worldpointsbonus;

    short		colorids[MAX_PLAYERS];

    short		bulletbilltimer[MAX_PLAYERS];
    short		bulletbillspawntimer[MAX_PLAYERS];

    size_t		loadedannouncer;
    short		loadedmusic;

    short		cputurn;


    //Player input used during game.  Reads SDL_Events and sets buttons that were pressed

    TournamentScores tournament_scores[MAX_PLAYERS];

    GameModeSettings gamemodesettings;

    bool		soundcapable;

    std::vector<bool> pfFilters;
    std::vector<short> piFilterIcons;
    short		selectedmapfilter;
    bool		fNeedWriteFilters;
    bool		fFiltersOn;

    short		singleplayermode;

    bool		worldskipscoreboard;

    bool		windaffectsplayers;
    bool		spinscreen;
    bool		reversewalk;
    bool		spotlights;

    bool		unlocksecret1part1[MAX_PLAYERS];
    short		unlocksecret1part2;
    bool		unlocksecret2part1;
    short		unlocksecret2part2;
    short		unlocksecret3part1[MAX_PLAYERS];
    short		unlocksecret3part2[MAX_PLAYERS];
    bool		unlocksecretunlocked[MAX_PLAYERS];
};

#endif // GAMEVALUES_H
