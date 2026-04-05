#pragma once

#include "GameMode.h"


//Collection (Collect cards for points)
class CGM_Collection : public CGameMode
{
    public:
        CGM_Collection();

        void init() override;
        void think() override;
        PlayerKillType playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style) override;
        PlayerKillType playerkilledself(CPlayer &player, KillStyle style) override;
        void playerextraguy(CPlayer &player, short iType) override;

        void ReleaseCard(CPlayer &player);

        PlayerKillType CheckWinner(CPlayer * player) override;

#ifdef _DEBUG
    void setdebuggoal() override {
        goal = 5;
    }
#endif

    private:
        short timer;
};
