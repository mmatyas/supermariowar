#include "GameModeSettings.h"

ClassicGameModeSettings::ClassicGameModeSettings()
    : style(0)          	//Respawn on death
    , scoring(0)        	//All kills will score
{}

FragGameModeSettings::FragGameModeSettings()
    : style(0)          	//Respawn on death
    , scoring(0)        	//All kills will score
{}

TimeGameModeSettings::TimeGameModeSettings()
    : style(0)              //Respawn on death
    , scoring(0)            //All kills will score
    , percentextratime(10)  //10% chance of a heart spawning
{}

JailGameModeSettings::JailGameModeSettings()
    : style(1)              //defaults to color jail play
    , tagfree(true)         //players on same team can free player by touching
    , timetofree(1240)      //20 seconds of jail
    , percentkey(30)        //30% chance of a key spawning
{}

CoinGameModeSettings::CoinGameModeSettings()
    : penalty(false)        //no penalty for getting stomped
    , quantity(1)           //only 1 coin on screen
    , percentextracoin(10)  //10% chance of an extra coin powerup
{}

StompGameModeSettings::StompGameModeSettings() {
    rate = 90; 				//Moderate
    enemyweight[0] = 4; 	// turn on goombas, koopa and cheep cheeps by default
    enemyweight[1] = 4;
    enemyweight[2] = 6;
    enemyweight[3] = 2;
    enemyweight[4] = 2;
    enemyweight[5] = 4;
    enemyweight[6] = 1;
    enemyweight[7] = 1;
    enemyweight[8] = 1;
}

EggGameModeSettings::EggGameModeSettings() {
    eggs[0] = 0;
    eggs[1] = 1;
    eggs[2] = 0;
    eggs[3] = 0;
    yoshis[0] = 0;
    yoshis[1] = 1;
    yoshis[2] = 0;
    yoshis[3] = 0;
    explode = 0;  			//Exploding eggs is turned off by default
}

FlagGameModeSettings::FlagGameModeSettings()
    : speed(0)              //Bases don't move by default
    , touchreturn(false)    //Don't return by touching
    , pointmove(true)       //Move base after point
    , autoreturn(1240)      //Return flag automatically after 20 seconds
    , homescore(false)      //Don't require flag to be home to score
    , centerflag(false)     //Do normal CTF, not center flag style
{}

ChickenGameModeSettings::ChickenGameModeSettings()
    : usetarget(true)   	//default to displaying a target around the chicken
    , glide(false)      	//don't give the chicken the ability to glide
{}

TagGameModeSettings::TagGameModeSettings()
    : tagontouch(true)  	//default to transfer tag on touching other players
{}

StarGameModeSettings::StarGameModeSettings()
    : time(30)              //default to 30 seconds
    , shine(0)              //default to hot potato (ztar)
    , percentextratime(10)  //10 percent chance of an extra time poweurp spawning
{}

DominationGameModeSettings::DominationGameModeSettings()
    : loseondeath(true)
    , stealondeath(false)
    , relocateondeath(false)
    , relocationfrequency(1240)  	//Relocate after 20 seconds = 1240
    , quantity(13) 					//# Players + 1 = 13
{}

KingOfTheHillModeSettings::KingOfTheHillModeSettings()
    : areasize(3)
    , relocationfrequency(1240)
    , maxmultiplier(1)   			//No multiplier
{}

RaceGameModeSettings::RaceGameModeSettings()
    : quantity(4)
    , speed(4)
    , penalty(2)  			//0 == none, 1 = 1 base, 2 = all bases lost on death
{}

FrenzyGameModeSettings::FrenzyGameModeSettings() {
    quantity = 6; 			//#players - 1
    rate = 186; 			//3 seconds
    storedshells = true; 	//Shells are stored by default
    powerupweight[0] = 0;
    powerupweight[1] = 1;  	// turn on flowers and hammers by default
    powerupweight[2] = 1;
    powerupweight[3] = 0;
    powerupweight[4] = 0;
    powerupweight[5] = 0;
    powerupweight[6] = 0;
    powerupweight[7] = 0;
    powerupweight[8] = 0;
    powerupweight[9] = 0;
    powerupweight[10] = 0;
    powerupweight[11] = 0;
}

SurvivalGameModeSettings::SurvivalGameModeSettings() {
    enemyweight[0] = 1;
    enemyweight[1] = 0;
    enemyweight[2] = 0;
    density = 20;
    speed = 4;
    shield = true;
}

GreedGameModeSettings::GreedGameModeSettings()
    : coinlife(124)			//Coins disappear after 2 seconds
    , owncoins(true)        //Can collect own coins
    , multiplier(2)         //Single multiplier
    , percentextracoin(10)  //10% chance of an extra coin powerup
{}

HealthGameModeSettings::HealthGameModeSettings()
    : startlife(6)         	//Start with 3 whole hearts (each increment is a half heart)
    , maxlife(1)           	//Maximum of 5 hearts
    , percentextralife(2) 	//20% chance of a heart spawning
{}

CollectionGameModeSettings::CollectionGameModeSettings()
    : quantity(6)       	//#players - 1
    , rate(186)         	//3 seconds to spawn
    , banktime(310)     	//5 seconds to bank
    , cardlife(310)     	//5 seconds to live
{}

ChaseGameModeSettings::ChaseGameModeSettings() {
    phantospeed = 6;        //Medium speed
    phantoquantity[0] = 1;
    phantoquantity[1] = 1;
    phantoquantity[2] = 0;
}

ShyGuyTagGameModeSettings::ShyGuyTagGameModeSettings()
    : tagonsuicide(false)
    , tagtransfer(0)
    , freetime(5)
{}

BossGameModeSettings::BossGameModeSettings()
    : bosstype(0)           //Default to hammer boss
    , difficulty(2)         //Medium difficulty
    , hitpoints(5)          //5 hits to kill
{}
