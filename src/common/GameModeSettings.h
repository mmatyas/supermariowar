#pragma once

#include "GameplayStyles.h"
#include "GlobalConstants.h"
#include "MatchTypes.h"

#include <array>

struct ClassicGameModeSettings {
    DeathStyle style;		//on kill, either respawn the player or shield them and let them keep playing
    ScoringStyle scoring;	//When to credit a score, all kills or push kills only (sumo mode)

    ClassicGameModeSettings();

    bool operator==(const ClassicGameModeSettings&) const = default;
};

struct FragGameModeSettings {
    DeathStyle style;		//on kill, either respawn the player or shield them and let them keep playing
    ScoringStyle scoring;	//When to credit a score, all kills or push kills only (sumo mode)

    FragGameModeSettings();

    bool operator==(const FragGameModeSettings&) const = default;
};

struct TimeGameModeSettings {
    DeathStyle style;		//on kill, either respawn the player or shield them and let them keep playing
    ScoringStyle scoring;	//When to credit a score, all kills or push kills only (sumo mode)
    short percentextratime; //percent chance a stopwatch with extra game time will spawn

    TimeGameModeSettings();

    bool operator==(const TimeGameModeSettings&) const = default;
};

struct JailGameModeSettings {
    JailStyle style;		//Style of play: classic, owned, free for all
    bool tagfree;			//Free jailed player by tagging on same team
    short timetofree;		//Time it takes for a jailed player to be freed
    short percentkey;		//percent chance a jail key will spawn

    JailGameModeSettings();

    bool operator==(const JailGameModeSettings&) const = default;
};

struct CoinGameModeSettings {
    bool penalty;			//Player loses coin if stomped on
    short quantity;			//Number of coins to have on screen at one time
    short percentextracoin; //percent chance a bonus coin will appear

    CoinGameModeSettings();

    bool operator==(const CoinGameModeSettings&) const = default;
};

struct StompGameModeSettings {
    short rate;				//How fast they spawn
    std::array<short, NUMSTOMPENEMIES> enemyweight;	//What ratio the enemies are chosen

    StompGameModeSettings();

    bool operator==(const StompGameModeSettings&) const = default;
};

struct EggGameModeSettings {
    std::array<short, 4> eggs;			//Number of eggs of each color in game
    std::array<short, 4> yoshis;		//Nunber of yoshis of each color in game
    short explode;			//Time until eggs explode

    EggGameModeSettings();

    bool operator==(const EggGameModeSettings&) const = default;
};

struct FlagGameModeSettings {
    short speed;			//How fast to move bases
    bool touchreturn;		//Return to base if player touches their own flag
    bool pointmove;			//Move base after point
    short autoreturn;		//Time to automatically return flag to base
    bool homescore;			//Need your flag at home base to score
    bool centerflag;		//Have a single flag that all teams fight to return to their base

    FlagGameModeSettings();

    bool operator==(const FlagGameModeSettings&) const = default;
};

struct ChickenGameModeSettings {
    bool usetarget;			//Display target around chicken
    bool glide;				//Allow chicken to glide through the air (chicken gets perm leaf powerup)

    ChickenGameModeSettings();

    bool operator==(const ChickenGameModeSettings&) const = default;
};

struct TagGameModeSettings {
    bool tagontouch;		//Transfer tag on touch

    TagGameModeSettings();

    bool operator==(const TagGameModeSettings&) const = default;
};

struct StarGameModeSettings {
    short time;				//Time to get rid or keep star
    StarStyle shine;		//If we are playing shine mode vs. ztar vs. stars mode
    short percentextratime; //percent chance a stopwatch with extra game time will spawn

    StarGameModeSettings();

    bool operator==(const StarGameModeSettings&) const = default;
};

struct DominationGameModeSettings {
    bool loseondeath;		//Player loses his bases on death
    bool stealondeath;		//Killing player steals bases on death
    bool relocateondeath;	//Owned bases relocate on player's death
    short relocationfrequency; //How fast the bases switch spots
    short quantity;			//Number of bases

    DominationGameModeSettings();

    bool operator==(const DominationGameModeSettings&) const = default;
};

struct KingOfTheHillModeSettings {
    short areasize;			//size of the hill area, 2x2, 3x3 etc.
    short relocationfrequency;  //How often the hill switches spots
    short maxmultiplier;	//as a player sits in the zone, the score will multiply

    KingOfTheHillModeSettings();

    bool operator==(const KingOfTheHillModeSettings&) const = default;
};

struct RaceGameModeSettings {
    short quantity;			//How many race goals
    short speed;			//How fast goals move
    short penalty;			//If dead players lose all tagged, one tagged or none

    RaceGameModeSettings();

    bool operator==(const RaceGameModeSettings&) const = default;
};

struct FrenzyGameModeSettings {
    short quantity;			//How many powerups spawn
    short rate;				//How fast they spawn
    bool storedshells;		//Shells become stored instead of active
    std::array<short, NUMFRENZYCARDS> powerupweight;	//What ratio the powerups are chosen

    FrenzyGameModeSettings();

    bool operator==(const FrenzyGameModeSettings&) const = default;
};

struct SurvivalGameModeSettings {
    std::array<short, NUMSURVIVALENEMIES> enemyweight;       //The weighting of thwomps vs. podobos
    short density;				//Number of thwomps on screen
    short speed;				//How fast thwomps go
    bool shield;				//Players are shielded when spawning

    SurvivalGameModeSettings();

    bool operator==(const SurvivalGameModeSettings&) const = default;
};

struct GreedGameModeSettings {
    short coinlife;				//How long coins stick around before disappearing
    bool owncoins;				//You can/can't pick up your own coins
    short multiplier;			//How many coins are released when players are hit
    short percentextracoin;		//percent chance a bonus coin will appear

    GreedGameModeSettings();

    bool operator==(const GreedGameModeSettings&) const = default;
};

struct HealthGameModeSettings {
    short startlife;			//Number of hearts players start with
    short maxlife;				//Number of hearts players can have
    short percentextralife;		//Percent chance an extra heart with come from a powerup block

    HealthGameModeSettings();

    bool operator==(const HealthGameModeSettings&) const = default;
};

struct CollectionGameModeSettings {
    short quantity;				//How many cards spawn
    short rate;					//How fast they spawn
    short banktime;				//Amount of time needed to bank cards to score
    short cardlife;				//How long cards live after they are released from players

    CollectionGameModeSettings();

    bool operator==(const CollectionGameModeSettings&) const = default;
};

struct ChaseGameModeSettings {
    short phantospeed;			//How fast the phantos move
    std::array<short, 3> phantoquantity;	//How many phantos there are

    ChaseGameModeSettings();

    bool operator==(const ChaseGameModeSettings&) const = default;
};

struct ShyGuyTagGameModeSettings {
    bool tagonsuicide;			//If players become shyguys if they kill themselves
    short tagtransfer;			//If players become shyguys if they are stomped
    short freetime;				//How long all players will stay shyguys before the game is reset

    ShyGuyTagGameModeSettings();

    bool operator==(const ShyGuyTagGameModeSettings&) const = default;
};

struct BossGameModeSettings {
    Boss bosstype;				//What type of boss battle it is
    short difficulty;			//How hard the boss is to defeat
    short hitpoints;			//How much life the boss has

    BossGameModeSettings();

    bool operator==(const BossGameModeSettings&) const = default;
};

struct GameModeSettings {
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
