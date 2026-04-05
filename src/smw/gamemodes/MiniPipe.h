#pragma once

#include "GameMode.h"


//Special mode where players collect coins from a pipe
class CGM_Pipe_MiniGame : public CGameMode
{
    public:
        CGM_Pipe_MiniGame();

        void init() override;
        void think() override;

        PlayerKillType playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style) override;
        PlayerKillType playerkilledself(CPlayer &player, KillStyle style) override;
        void playerextraguy(CPlayer &player, short iType) override;
        PlayerKillType CheckWinner(CPlayer * player) override;

        void SetBonus(short iType, short iTimer, short iTeamID);
    bool IsSlowdown() {
        return fSlowdown;
    }

    bool HasStoredPowerups() override {
        return false;
    }

    private:

        short iNextItemTimer;
        short iBonusTimer, iBonusType, iBonusTeam;

        bool fSlowdown;
};
