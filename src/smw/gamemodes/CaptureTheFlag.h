#pragma once

#include "GameMode.h"


//Capture The Flag mode - each team has a base and a flag
//Protect your colored flag from being taken and score a point
//for stealing another teams flag and returning it to your base
class CGM_CaptureTheFlag : public CGameMode
{
    public:
        CGM_CaptureTheFlag();

        void init() override;
        PlayerKillType playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style) override;
        PlayerKillType playerkilledself(CPlayer &player, KillStyle style) override;
        void playerextraguy(CPlayer &player, short iType) override;
        PlayerKillType CheckWinner(CPlayer * player) override;

#ifdef _DEBUG
    void setdebuggoal() override {
        goal = 5;
    }
#endif
};
