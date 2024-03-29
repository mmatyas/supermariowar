#pragma once

#include "GameMode.h"
#include "GameModeTimer.h"


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
