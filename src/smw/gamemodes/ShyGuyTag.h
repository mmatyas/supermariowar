#pragma once

#include "GameMode.h"
#include "GameModeTimer.h"


class CGM_ShyGuyTag : public CGameMode
{
    public:
        CGM_ShyGuyTag();

        void init() override;
        void think() override;
        void draw_foreground() override;
        PlayerKillType playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style) override;
        PlayerKillType playerkilledself(CPlayer &player, KillStyle style) override;
        void playerextraguy(CPlayer &player, short iType) override;

        void SetShyGuy(short iTeam);

#ifdef _DEBUG
    void setdebuggoal() override {
        goal = 100;
    }
#endif

    private:

        PlayerKillType CheckWinner(CPlayer * player) override;
        short CountShyGuys();
        void FreeShyGuys();

        GameTimerDisplay gameClock;
        bool fRunClock;
        short scorecounter;
};
