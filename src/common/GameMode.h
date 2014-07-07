#ifndef GAMEMODE_H
#define GAMEMODE_H

#define GAMEMODE_NUM_OPTIONS 21

#include "PlayerKillStyles.h"
#include "PlayerKillTypes.h"

class CPlayer;

//Enum for each game mode
enum GameModeType {
    game_mode_classic,
    game_mode_frag,
    game_mode_timelimit,
    game_mode_jail,
    game_mode_coins,
    game_mode_stomp,
    game_mode_eggs,
    game_mode_ctf,
    game_mode_chicken,
    game_mode_tag,
    game_mode_star,
    game_mode_domination,
    game_mode_koth,
    game_mode_race,
    game_mode_owned,
    game_mode_frenzy,
    game_mode_survival,
    game_mode_greed,
    game_mode_health,
    game_mode_collection,
    game_mode_chase,
    game_mode_shyguytag,
    GAMEMODE_LAST,
    game_mode_bonus = 999,
    game_mode_pipe_minigame = 1000,
    game_mode_boss_minigame = 1001,
    game_mode_boxes_minigame = 1002
};

struct SModeOption {
	char szName[64];
	short iValue;
};

//gamemode base class
class CGameMode
{
	public:

		CGameMode();
        virtual ~CGameMode();

		virtual void init();  //called once when the game is started
		virtual void think();	//called once a frame
		virtual void draw_background() {}
		virtual void draw_foreground() {}
		//called when a player stomps another player, after the p2p logic has run
		//returns true if the other was deleted
		virtual short playerkilledplayer(CPlayer &inflictor, CPlayer &other, killstyle style);
		virtual short playerkilledself(CPlayer &player, killstyle style);
		virtual void playerextraguy(CPlayer &player, short iType);

    virtual short CheckWinner(CPlayer *) {
        return player_kill_normal;
    }

    virtual bool HasStoredPowerups() {
        return true;
    }

		void transferbobombifneeded(CPlayer &inflictor, CPlayer &other);
		void displayplayertext();
		void playwarningsound();

		short winningteam;
		bool gameover;

		CPlayer *chicken;
		CPlayer *tagged;
		CPlayer *frenzyowner;

    GameModeType getgamemode() {
        return gamemode;
    }
		GameModeType gamemode;

    char * GetModeName() {
        return szModeName;
    }
    char * GetGoalName() {
        return szGoalName;
    }
    SModeOption * GetOptions() {
        return modeOptions;
    }

		bool playedwarningsound;
		short goal;

		short GetClosestGoal(short iGoal);
    bool GetReverseScoring() {
        return fReverseScoring;
    }

#ifdef _DEBUG
		virtual void setdebuggoal() {};
#endif

	protected:
		char szModeName[64];
		char szGoalName[64];
		SModeOption modeOptions[GAMEMODE_NUM_OPTIONS];

		void SetupModeStrings(const char * szMode, const char * szGoal, short iGoalSpacing);

		CPlayer * GetHighestScorePlayer(bool fGetHighest);
		short GetScoreRankedPlayerList(CPlayer * players[4], bool fGetHighest);

		bool fReverseScoring;
};

#endif // GAMEMODE_H
