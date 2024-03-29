#ifndef GAMEMODES_H
#define GAMEMODES_H

#include "GameMode.h"
#include "gamemodes/Classic.h"
#include "gamemodes/GameModeTimer.h"

class CPlayer;
struct TourStop;
class CO_PhantoKey;
class CO_Star;


//Fraglimit
class CGM_Frag : public CGameMode
{
	public:
        CGM_Frag();
		virtual ~CGM_Frag() {}

		PlayerKillType playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style);
		PlayerKillType playerkilledself(CPlayer &player, KillStyle style);
		void playerextraguy(CPlayer &player, short iType);

		PlayerKillType CheckWinner(CPlayer * player);

#ifdef _DEBUG
    void setdebuggoal() {
        goal = 5;
    }
#endif

};


//capture the chicken
class CGM_Chicken : public CGameMode
{
	public:
        CGM_Chicken();
		virtual ~CGM_Chicken() {}

		void think();
		void draw_foreground();
		PlayerKillType playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style);
		PlayerKillType playerkilledself(CPlayer &player, KillStyle style);
		void playerextraguy(CPlayer &player, short iType);
		PlayerKillType CheckWinner(CPlayer * player);

#ifdef _DEBUG
    void setdebuggoal() {
        goal = 100;
    }
#endif

};

class CGM_Tag : public CGameMode
{
	public:
        CGM_Tag();
		virtual ~CGM_Tag() {}

		void init();
		void think();
		PlayerKillType playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style);
		PlayerKillType playerkilledself(CPlayer &player, KillStyle style);
		void playerextraguy(CPlayer &player, short iType);

#ifdef _DEBUG
    void setdebuggoal() {
        goal = 100;
    }
#endif

};

class CGM_ShyGuyTag : public CGameMode
{
	public:
        CGM_ShyGuyTag();
		virtual ~CGM_ShyGuyTag() {}

		void init();
		void think();
		void draw_foreground();
		PlayerKillType playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style);
		PlayerKillType playerkilledself(CPlayer &player, KillStyle style);
		void playerextraguy(CPlayer &player, short iType);

		void SetShyGuy(short iTeam);

#ifdef _DEBUG
    void setdebuggoal() {
        goal = 100;
    }
#endif

	private:

		PlayerKillType CheckWinner(CPlayer * player);
		short CountShyGuys();
		void FreeShyGuys();

		GameTimerDisplay gameClock;
		bool fRunClock;
		short scorecounter;
};

class CGM_Coins : public CGameMode
{
		public:
        CGM_Coins();
		virtual ~CGM_Coins() {}

		virtual void init();
		PlayerKillType playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style);
		PlayerKillType playerkilledself(CPlayer &player, KillStyle style);
		void playerextraguy(CPlayer &player, short iType);

		PlayerKillType CheckWinner(CPlayer * player);

#ifdef _DEBUG
    void setdebuggoal() {
        goal = 5;
    }
#endif

};

class CGM_Eggs : public CGameMode
{
	public:
        CGM_Eggs();
		virtual ~CGM_Eggs() {}

		void init();
		PlayerKillType playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style);
		PlayerKillType playerkilledself(CPlayer &player, KillStyle style);
		void playerextraguy(CPlayer &player, short iType);
		PlayerKillType CheckWinner(CPlayer * player);

#ifdef _DEBUG
    void setdebuggoal() {
        goal = 5;
    }
#endif

};

//Just like frag limit, but firepower cards appear randomly
class CGM_Frenzy : public CGM_Frag
{
	public:
        CGM_Frenzy();
		virtual ~CGM_Frenzy() {}

		void init();
		void think();
		char *getMenuString(char *buffer64);

	private:
		short timer;
		short iSelectedPowerup;
		short iItemWeightCount;
};

class CGM_Survival : public CGM_Classic
{
	public:
        CGM_Survival();
		virtual ~CGM_Survival() {}

		void init();
		void think();
		char *getMenuString(char *buffer64);

	protected:
		short timer;
		short ratetimer;
		short rate;
		short iSelectedEnemy;
		short iEnemyWeightCount;
};

//Similar to frag limit, but players get bonus frags for the number of players they have "owned"
class CGM_Owned : public CGameMode
{
	public:
        CGM_Owned();
		virtual ~CGM_Owned() {}

		void think();
		PlayerKillType playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style);
		PlayerKillType playerkilledself(CPlayer &player, KillStyle style);
		void playerextraguy(CPlayer &player, short iType);

#ifdef _DEBUG
    void setdebuggoal() {
        goal = 100;
    }
#endif

	private:
		PlayerKillType CheckWinner(CPlayer * player);

};

//Similar to frag limit but:
//When a player is killed by another player, they get a "jail" marker
//Jailed players move slowly
//Jailed players can be freed by teammates
//If all players on a team are jailed, bonus kill goes to other team
//Similar to frag limit, but players get bonus frags for the number of players they have "owned"
class CGM_Jail : public CGM_Frag
{
	public:
        CGM_Jail();
		virtual ~CGM_Jail() {}

		PlayerKillType playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style);
		void playerextraguy(CPlayer &player, short iType);
		char *getMenuString(char *buffer64);
};

//Similar to coin mode but you have to smash the most goombas/cheeps/koopas
class CGM_Stomp : public CGameMode
{
	public:
        CGM_Stomp();
		virtual ~CGM_Stomp() {}

		void init();
		void think();

		PlayerKillType playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style);
		PlayerKillType playerkilledself(CPlayer &player, KillStyle style);
		void playerextraguy(CPlayer &player, short iType);

		char *getMenuString(char *buffer64);

	private:
		void ResetSpawnTimer();
		PlayerKillType CheckWinner(CPlayer * player);

		short spawntimer;
		short iSelectedEnemy;
		short iEnemyWeightCount;
};

//Players tag flying targets in order to complete laps
class CGM_Race : public CGameMode
{
	public:
        virtual ~CGM_Race() {}
		CGM_Race();
		void init();
		PlayerKillType playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style);
		PlayerKillType playerkilledself(CPlayer &player, KillStyle style);
		void playerextraguy(CPlayer &player, short iType);

    short getNextGoal(short teamID) {
        return nextGoal[teamID];
    }
		void setNextGoal(short teamID);

#ifdef _DEBUG
    void setdebuggoal() {
        goal = 5;
    }
#endif

	protected:
		void PenalizeRaceGoals(CPlayer &player);
		short nextGoal[4];
		short quantity;
		short penalty;
};

#endif // GAMEMODES_H
