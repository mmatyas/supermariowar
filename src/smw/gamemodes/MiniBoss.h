#pragma once

#include "GameMode.h"
#include "MatchTypes.h"


//Special mode where players try to kill a boss
class CGM_Boss_MiniGame : public CGameMode
{
    public:
        CGM_Boss_MiniGame();
        virtual ~CGM_Boss_MiniGame() {}

        void init();
        void think();
        void draw_foreground();

        PlayerKillType playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style);
        PlayerKillType playerkilledself(CPlayer &player, KillStyle style);
        void playerextraguy(CPlayer &player, short iType);

        char *getMenuString(char *buffer64);

        bool SetWinner(CPlayer * player);
        void SetBossType(Boss bosstype);
    Boss GetBossType() const {
        return iBossType;
    }

    bool HasStoredPowerups() {
        return false;
    }

    private:

        short enemytimer, poweruptimer;
        Boss iBossType = Boss::Hammer;
};
