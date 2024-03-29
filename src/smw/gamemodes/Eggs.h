#pragma once

#include "GameMode.h"


class CGM_Eggs : public CGameMode
{
    public:
        CGM_Eggs();
        virtual ~CGM_Eggs() {}

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
