#pragma once

#include "GameMode.h"

//Similar to frag limit, but players get bonus frags for the number of players they have "owned"
class CGM_Owned : public CGameMode
{
    public:
        CGM_Owned();
        virtual ~CGM_Owned() {}

        void think();
        PlayerKillType playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style);
        PlayerKillType playerkilledself(CPlayer &player, KillStyle style);
        void playerextraguy(CPlayer &player, short iType);

#ifdef _DEBUG
    void setdebuggoal() {
        goal = 100;
    }
#endif

    private:
        PlayerKillType CheckWinner(CPlayer * player);

};
