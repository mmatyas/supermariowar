#ifndef GAMEMODESETTINGS_H
#define GAMEMODESETTINGS_H

#include "GlobalConstants.h"

struct ClassicGameModeSettings {
    short style;			//on kill, either respawn the player or shield them and let them keep playing
    short scoring;			//When to credit a score, all kills or push kills only (sumo mode)

    ClassicGameModeSettings();
};

struct FragGameModeSettings {
    short style;			//on kill, either respawn the player or shield them and let them keep playing
    short scoring;			//When to credit a score, all kills or push kills only (sumo mode)

    FragGameModeSettings();
};

struct TimeGameModeSettings {
    short style;			//on kill, either respawn the player or shield them and let them keep playing
    short scoring;			//When to credit a score, all kills or push kills only (sumo mode)
    short percentextratime; //percent chance a stopwatch with extra game time will spawn

    TimeGameModeSettings();
};

struct JailGameModeSettings {
    short style;			//Style of play: classic, owned, free for all
    bool tagfree;			//Free jailed player by tagging on same team
    short timetofree;		//Time it takes for a jailed player to be freed
    short percentkey;		//percent chance a jail key will spawn

    JailGameModeSettings();
};

struct CoinGameModeSettings {
    bool penalty;			//Player loses coin if stomped on
    short quantity;			//Number of coins to have on screen at one time
    short percentextracoin; //percent chance a bonus coin will appear

    CoinGameModeSettings();
};

struct StompGameModeSettings {
    short rate;				//How fast they spawn
    short enemyweight[NUMSTOMPENEMIES];	//What ratio the enemies are chosen

    StompGameModeSettings();
};

struct EggGameModeSettings {
    short eggs[4];			//Number of eggs of each color in game
    short yoshis[4];		//Nunber of yoshis of each color in game
    short explode;			//Time until eggs explode

    EggGameModeSettings();
};

struct FlagGameModeSettings {
    short speed;			//How fast to move bases
    bool touchreturn;		//Return to base if player touches their own flag
    bool pointmove;			//Move base after point
    short autoreturn;		//Time to automatically return flag to base
    bool homescore;			//Need your flag at home base to score
    bool centerflag;		//Have a single flag that all teams fight to return to their base

    FlagGameModeSettings();
};

struct ChickenGameModeSettings {
    bool usetarget;			//Display target around chicken
    bool glide;				//Allow chicken to glide through the air (chicken gets perm leaf powerup)

    ChickenGameModeSettings();
};

struct TagGameModeSettings {
    bool tagontouch;		//Transfer tag on touch

    TagGameModeSettings();
};

struct StarGameModeSettings {
    short time;				//Time to get rid or keep star
    short shine;			//If we are playing shine mode vs. ztar vs. stars mode
    short percentextratime; //percent chance a stopwatch with extra game time will spawn

    StarGameModeSettings();
};

struct DominationGameModeSettings {
    bool loseondeath;		//Player loses his bases on death
    bool stealondeath;		//Killing player steals bases on death
    bool relocateondeath;	//Owned bases relocate on player's death
    short relocationfrequency; //How fast the bases switch spots
    short quantity;			//Number of bases

    DominationGameModeSettings();
};

struct KingOfTheHillModeSettings {
    short areasize;			//size of the hill area, 2x2, 3x3 etc.
    short relocationfrequency;  //How often the hill switches spots
    short maxmultiplier;	//as a player sits in the zone, the score will multiply

    KingOfTheHillModeSettings();
};

struct RaceGameModeSettings {
    short quantity;			//How many race goals
    short speed;			//How fast goals move
    short penalty;			//If dead players lose all tagged, one tagged or none

    RaceGameModeSettings();
};

struct FrenzyGameModeSettings {
    short quantity;			//How many powerups spawn
    short rate;				//How fast they spawn
    bool storedshells;		//Shells become stored instead of active
    short powerupweight[NUMFRENZYCARDS];	//What ratio the powerups are chosen

    FrenzyGameModeSettings();
};

struct SurvivalGameModeSettings {
    short enemyweight[NUMSURVIVALENEMIES];       //The weighting of thwomps vs. podobos
    short density;				//Number of thwomps on screen
    short speed;				//How fast thwomps go
    bool shield;				//Players are shielded when spawning

    SurvivalGameModeSettings();
};

struct GreedGameModeSettings {
    short coinlife;				//How long coins stick around before disappearing
    bool owncoins;				//You can/can't pick up your own coins
    short multiplier;			//How many coins are released when players are hit
    short percentextracoin;		//percent chance a bonus coin will appear

    GreedGameModeSettings();
};

struct HealthGameModeSettings {
    short startlife;			//Number of hearts players start with
    short maxlife;				//Number of hearts players can have
    short percentextralife;		//Percent chance an extra heart with come from a powerup block

    HealthGameModeSettings();
};

struct CollectionGameModeSettings {
    short quantity;				//How many cards spawn
    short rate;					//How fast they spawn
    short banktime;				//Amount of time needed to bank cards to score
    short cardlife;				//How long cards live after they are released from players

    CollectionGameModeSettings();
};

struct ChaseGameModeSettings {
    short phantospeed;			//How fast the phantos move
    short phantoquantity[3];	//How many phantos there are

    ChaseGameModeSettings();
};

struct ShyGuyTagGameModeSettings {
    bool tagonsuicide;			//If players become shyguys if they kill themselves
    short tagtransfer;			//If players become shyguys if they are stomped
    short freetime;				//How long all players will stay shyguys before the game is reset

    ShyGuyTagGameModeSettings();
};

struct BossGameModeSettings {
    short bosstype;				//What type of boss battle it is
    short difficulty;			//How hard the boss is to defeat
    short hitpoints;			//How much life the boss has

    BossGameModeSettings();
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

#endif // GAMEMODESETTINGS_H
