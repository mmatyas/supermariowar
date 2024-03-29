#pragma once

#include "GameMode.h"


//Capture The Flag mode - each team has a base and a flag
//Protect your colored flag from being taken and score a point
//for stealing another teams flag and returning it to your base
class CGM_CaptureTheFlag : public CGameMode
{
    public:
        CGM_CaptureTheFlag();
        virtual ~CGM_CaptureTheFlag() {}

        void init();
        PlayerKillType playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style);
        PlayerKillType playerkilledself(CPlayer &player, KillStyle style);
        void playerextraguy(CPlayer &player, short iType);
        PlayerKillType CheckWinner(CPlayer * player);

#ifdef _DEBUG
    void setdebuggoal() {
        goal = 5;
    }
#endif
};
