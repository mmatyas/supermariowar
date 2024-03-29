#pragma once

#include "GameMode.h"


//Collection (Collect cards for points)
class CGM_Collection : public CGameMode
{
    public:
        CGM_Collection();
        virtual ~CGM_Collection() {}

        void init();
        void think();
        PlayerKillType playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style);
        PlayerKillType playerkilledself(CPlayer &player, KillStyle style);
        void playerextraguy(CPlayer &player, short iType);

        void ReleaseCard(CPlayer &player);

        PlayerKillType CheckWinner(CPlayer * player);

#ifdef _DEBUG
    void setdebuggoal() {
        goal = 5;
    }
#endif

    private:
        short timer;
};
