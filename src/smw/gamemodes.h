#ifndef GAMEMODES_H
#define GAMEMODES_H

#include "GameMode.h"

class CPlayer;
struct TourStop;
class CO_PhantoKey;
class CO_Star;

class GameTimerDisplay
{
    public:
        GameTimerDisplay();
        ~GameTimerDisplay() {}

        void Init(short iTime, bool fCountDown);
        short RunClock();
        void Draw();
        void SetTime(short iTime);
        void AddTime(short iTime);

    protected:
        void SetDigitCounters();

        short timeleft;
        bool countdown;

        short framesleft_persecond;
        short iDigitLeftSrcX;
        short iDigitMiddleSrcX;
        short iDigitRightSrcX;
        short iDigitLeftDstX;
        short iDigitMiddleDstX;
        short iDigitRightDstX;
        short iScoreOffsetX;

        short iFramesPerSecond;
};


//Fraglimit
class CGM_Frag : public CGameMode
{
	public:
        CGM_Frag();
		virtual ~CGM_Frag() {}

		short playerkilledplayer(CPlayer &inflictor, CPlayer &other, killstyle style);
		short playerkilledself(CPlayer &player, killstyle style);
		void playerextraguy(CPlayer &player, short iType);

		short CheckWinner(CPlayer * player);

#ifdef _DEBUG
    void setdebuggoal() {
        goal = 5;
    }
#endif

};


//Timelimit
class CGM_TimeLimit : public CGameMode
{
	public:
        CGM_TimeLimit();
		virtual ~CGM_TimeLimit() {}

		void init();
		void think();
		void draw_foreground();
		void drawtime();
		short playerkilledplayer(CPlayer &inflictor, CPlayer &other, killstyle style);
		short playerkilledself(CPlayer &player, killstyle style);

		void addtime(short iTime);

#ifdef _DEBUG
    void setdebuggoal() {
        goal = 30;
    }
#endif

	protected:
		GameTimerDisplay gameClock;

};


//mariowar classic
class CGM_Classic : public CGameMode
{
	public:
        CGM_Classic();
		virtual ~CGM_Classic() {}

		virtual void init();
		virtual short playerkilledplayer(CPlayer &inflictor, CPlayer &other, killstyle style);
		virtual short playerkilledself(CPlayer &player, killstyle style);
		virtual void playerextraguy(CPlayer &player, short iType);

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
		short playerkilledplayer(CPlayer &inflictor, CPlayer &other, killstyle style);
		short playerkilledself(CPlayer &player, killstyle style);
		void playerextraguy(CPlayer &player, short iType);
		short CheckWinner(CPlayer * player);

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
		short playerkilledplayer(CPlayer &inflictor, CPlayer &other, killstyle style);
		short playerkilledself(CPlayer &player, killstyle style);
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
		short playerkilledplayer(CPlayer &inflictor, CPlayer &other, killstyle style);
		short playerkilledself(CPlayer &player, killstyle style);
		void playerextraguy(CPlayer &player, short iType);

		void SetShyGuy(short iTeam);

#ifdef _DEBUG
    void setdebuggoal() {
        goal = 100;
    }
#endif

	private:

		short CheckWinner(CPlayer * player);
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
		short playerkilledplayer(CPlayer &inflictor, CPlayer &other, killstyle style);
		short playerkilledself(CPlayer &player, killstyle style);
		void playerextraguy(CPlayer &player, short iType);

		short CheckWinner(CPlayer * player);

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
		short playerkilledplayer(CPlayer &inflictor, CPlayer &other, killstyle style);
		short playerkilledself(CPlayer &player, killstyle style);
		void playerextraguy(CPlayer &player, short iType);
		short CheckWinner(CPlayer * player);

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


//Domination (capture the area blocks)
class CGM_Domination : public CGameMode
{
	public:
        CGM_Domination();
		virtual ~CGM_Domination() {}

		void init();
		short playerkilledplayer(CPlayer &inflictor, CPlayer &other, killstyle style);
		short playerkilledself(CPlayer &player, killstyle style);
		void playerextraguy(CPlayer &player, short iType);
		short CheckWinner(CPlayer * player);

#ifdef _DEBUG
    void setdebuggoal() {
        goal = 100;
    }
#endif
};

//Similar to frag limit, but players get bonus frags for the number of players they have "owned"
class CGM_Owned : public CGameMode
{
	public:
        CGM_Owned();
		virtual ~CGM_Owned() {}

		void think();
		short playerkilledplayer(CPlayer &inflictor, CPlayer &other, killstyle style);
		short playerkilledself(CPlayer &player, killstyle style);
		void playerextraguy(CPlayer &player, short iType);

#ifdef _DEBUG
    void setdebuggoal() {
        goal = 100;
    }
#endif

	private:
		short CheckWinner(CPlayer * player);

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

		short playerkilledplayer(CPlayer &inflictor, CPlayer &other, killstyle style);
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

		short playerkilledplayer(CPlayer &inflictor, CPlayer &other, killstyle style);
		short playerkilledself(CPlayer &player, killstyle style);
		void playerextraguy(CPlayer &player, short iType);

		char *getMenuString(char *buffer64);

	private:
		void ResetSpawnTimer();
		short CheckWinner(CPlayer * player);

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
		short playerkilledplayer(CPlayer &inflictor, CPlayer &other, killstyle style);
		short playerkilledself(CPlayer &player, killstyle style);
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


//Star mode - shared timer ticks down and the players must pass around
//a hot potato.  When the timer hits zero, the player with the star
//loses a point.
class CGM_Star : public CGM_TimeLimit
{
	public:
        CGM_Star();
		virtual ~CGM_Star() {}

		void init();
		void think();
		void draw_foreground();
		short playerkilledplayer(CPlayer &inflictor, CPlayer &other, killstyle style);
		short playerkilledself(CPlayer &player, killstyle style);
		void playerextraguy(CPlayer &player, short iType);

		bool isplayerstar(CPlayer * player);
		CPlayer * swapplayer(short id, CPlayer * player);
    CPlayer * getstarplayer(short id) {
        return starPlayer[id];
    }

    short getcurrentmodetype() {
        return iCurrentModeType;
    }

#ifdef _DEBUG
    void setdebuggoal() {
        goal = 1;
    }
#endif

	private:
		void SetupMode();

		CO_Star * starItem[3];
		CPlayer * starPlayer[3];
		short iCurrentModeType;
		bool fDisplayTimer;
};

//Capture The Flag mode - each team has a base and a flag
//Protect your colored flag from being taken and score a point
//for stealing another teams flag and returning it to your base
class CGM_CaptureTheFlag : public CGameMode
{
	public:
        CGM_CaptureTheFlag();
		virtual ~CGM_CaptureTheFlag() {}

		void init();
		short playerkilledplayer(CPlayer &inflictor, CPlayer &other, killstyle style);
		short playerkilledself(CPlayer &player, killstyle style);
		void playerextraguy(CPlayer &player, short iType);
		short CheckWinner(CPlayer * player);

#ifdef _DEBUG
    void setdebuggoal() {
        goal = 5;
    }
#endif
};


//Domination (capture the area blocks)
class CGM_KingOfTheHill : public CGM_Domination
{
	public:
        CGM_KingOfTheHill();
		virtual ~CGM_KingOfTheHill() {}

		void init();
		short playerkilledplayer(CPlayer &inflictor, CPlayer &other, killstyle style);
		short playerkilledself(CPlayer &player, killstyle style);
		void playerextraguy(CPlayer &player, short iType);

#ifdef _DEBUG
    void setdebuggoal() {
        goal = 100;
    }
#endif
};

//Greed mode (players try to steal each other's coins)
class CGM_Greed : public CGM_Classic
{
	public:
        CGM_Greed();
		virtual ~CGM_Greed() {}

		void init();
		short playerkilledplayer(CPlayer &inflictor, CPlayer &other, killstyle style);
		short playerkilledself(CPlayer &player, killstyle style);
		void playerextraguy(CPlayer &player, short iType);

		short ReleaseCoins(CPlayer &player, killstyle style);

#ifdef _DEBUG
    void setdebuggoal() {
        goal = 10;
    }
#endif

};

//health mode
class CGM_Health : public CGM_Classic
{
	public:
        CGM_Health();
		virtual ~CGM_Health() {}

		virtual void init();
		virtual short playerkilledplayer(CPlayer &inflictor, CPlayer &other, killstyle style);
		virtual short playerkilledself(CPlayer &player, killstyle style);
		virtual void playerextraguy(CPlayer &player, short iType);

#ifdef _DEBUG
    void setdebuggoal() {
        goal = 5;
    }
#endif

};

//Collection (Collect cards for points)
class CGM_Collection : public CGameMode
{
	public:
        CGM_Collection();
		virtual ~CGM_Collection() {}

		void init();
		void think();
		short playerkilledplayer(CPlayer &inflictor, CPlayer &other, killstyle style);
		short playerkilledself(CPlayer &player, killstyle style);
		void playerextraguy(CPlayer &player, short iType);

		void ReleaseCard(CPlayer &player);

		short CheckWinner(CPlayer * player);

#ifdef _DEBUG
    void setdebuggoal() {
        goal = 5;
    }
#endif

	private:
		short timer;
};


//Chase (player is chased by phanto)
class CGM_Chase : public CGameMode
{
	public:
        CGM_Chase();
		virtual ~CGM_Chase() {}

		void init();
		void think();
		short playerkilledplayer(CPlayer &inflictor, CPlayer &other, killstyle style);
		short playerkilledself(CPlayer &player, killstyle style);
		void playerextraguy(CPlayer &player, short iType);

		short CheckWinner(CPlayer * player);

		CPlayer * GetKeyHolder();

#ifdef _DEBUG
    void setdebuggoal() {
        goal = 50;
    }
#endif

	private:
		CO_PhantoKey * key;
};


//Special mode where players try to kill a boss
class CGM_Boss_MiniGame : public CGameMode
{
	public:
        CGM_Boss_MiniGame();
		virtual ~CGM_Boss_MiniGame() {}

		void init();
		void think();
		void draw_foreground();

		short playerkilledplayer(CPlayer &inflictor, CPlayer &other, killstyle style);
		short playerkilledself(CPlayer &player, killstyle style);
		void playerextraguy(CPlayer &player, short iType);

		char *getMenuString(char *buffer64);

		bool SetWinner(CPlayer * player);
		void SetBossType(short bosstype);
    short GetBossType() {
        return iBossType;
    }

    bool HasStoredPowerups() {
        return false;
    }

	private:

		short enemytimer, poweruptimer;
		short iBossType;
};


//Special mode where players can collect a bonus item
class CGM_Bonus : public CGameMode
{
	public:
        CGM_Bonus();
		virtual ~CGM_Bonus() {}

		void init();
		void draw_background();

		//Override so it doesn't display winner text after you choose a powerup
		void think() {}

    short playerkilledplayer(CPlayer &inflictor, CPlayer &other, killstyle style) {
        return false;
    }
    short playerkilledself(CPlayer &player, killstyle style) {
        return false;
    }
		void playerextraguy(CPlayer &player, short iType) {}

    bool HasStoredPowerups() {
        return false;
    }

	private:

		TourStop * tsTourStop;
};

//Special mode where players collect coins from a pipe
class CGM_Pipe_MiniGame : public CGameMode
{
	public:
        CGM_Pipe_MiniGame();
		virtual ~CGM_Pipe_MiniGame() {}

		void init();
		void think();

		short playerkilledplayer(CPlayer &inflictor, CPlayer &other, killstyle style);
		short playerkilledself(CPlayer &player, killstyle style);
		void playerextraguy(CPlayer &player, short iType);
		short CheckWinner(CPlayer * player);

		void SetBonus(short iType, short iTimer, short iTeamID);
    bool IsSlowdown() {
        return fSlowdown;
    }

    bool HasStoredPowerups() {
        return false;
    }

	private:

		short iNextItemTimer;
		short iBonusTimer, iBonusType, iBonusTeam;

		bool fSlowdown;
};


//Special mode where players break boxes
class CGM_Boxes_MiniGame : public CGameMode
{
	public:
        CGM_Boxes_MiniGame();
		virtual ~CGM_Boxes_MiniGame() {}

		void init();
		void think();

		short playerkilledplayer(CPlayer &inflictor, CPlayer &other, killstyle style);
		short playerkilledself(CPlayer &player, killstyle style);
		void playerextraguy(CPlayer &player, short iType);

		char *getMenuString(char *buffer64);

		short CheckWinner(CPlayer * player);
		bool SetWinner(CPlayer * player);

    bool HasStoredPowerups() {
        return false;
    }

	private:

		void ReleaseCoin(CPlayer &player);
		void ReleaseAllCoinsFromTeam(CPlayer &player);
};

#endif // GAMEMODES_H
