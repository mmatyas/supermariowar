#ifndef GAMEMODE_H
#define GAMEMODE_H

#define GAMEMODE_NUM_OPTIONS 21

#include "GameModeType.h"
#include "PlayerKillStyles.h"
#include "PlayerKillTypes.h"
#include <string>

class CPlayer;


struct SModeOption {
	std::string szName;
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
		virtual PlayerKillType playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style);
		virtual PlayerKillType playerkilledself(CPlayer &player, KillStyle style);
		virtual void playerextraguy(CPlayer &player, short iType);

    virtual PlayerKillType CheckWinner(CPlayer *) {
        return PlayerKillType::Normal;
    }

    virtual bool HasStoredPowerups() {
        return true;
    }

		void displayplayertext();
		void playwarningsound();

		short winningteam;
		bool gameover;

    GameModeType getgamemode() {
        return gamemode;
    }
		GameModeType gamemode;

    const std::string& GetModeName() const {
        return szModeName;
    }
    const std::string& GetGoalName() const {
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
		std::string szModeName;
		std::string szGoalName;
		SModeOption modeOptions[GAMEMODE_NUM_OPTIONS];

		void SetupModeStrings(const char * szMode, const char * szGoal, short iGoalSpacing);

		CPlayer * GetHighestScorePlayer(bool fGetHighest);
		short GetScoreRankedPlayerList(CPlayer * players[4], bool fGetHighest);

		bool fReverseScoring;
};

void RemovePlayersButTeam(short teamid);
bool RemoveTeam(short teamid);
void SetupScoreBoard(bool fOrderMatters);
void ShowScoreBoard();
void RemovePlayersButHighestScoring();

#endif // GAMEMODE_H
