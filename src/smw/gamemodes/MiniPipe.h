#pragma once

#include "GameMode.h"


//Special mode where players collect coins from a pipe
class CGM_Pipe_MiniGame : public CGameMode
{
    public:
        CGM_Pipe_MiniGame();
        virtual ~CGM_Pipe_MiniGame() {}

        void init();
        void think();

        PlayerKillType playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style);
        PlayerKillType playerkilledself(CPlayer &player, KillStyle style);
        void playerextraguy(CPlayer &player, short iType);
        PlayerKillType CheckWinner(CPlayer * player);

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
