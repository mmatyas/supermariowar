#pragma once

#include "GameMode.h"
#include "MatchTypes.h"


//Special mode where players try to kill a boss
class CGM_Boss_MiniGame : public CGameMode
{
    public:
        CGM_Boss_MiniGame();

        void init() override;
        void think() override;
        void draw_foreground() override;

        PlayerKillType playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style) override;
        PlayerKillType playerkilledself(CPlayer &player, KillStyle style) override;
        void playerextraguy(CPlayer &player, short iType) override;

        char *getMenuString(char *buffer64);

        bool SetWinner(CPlayer * player);
        void SetBossType(Boss bosstype);
    Boss GetBossType() const {
        return iBossType;
    }

    bool HasStoredPowerups() override {
        return false;
    }

    private:

        short enemytimer, poweruptimer;
        Boss iBossType = Boss::Hammer;
};
