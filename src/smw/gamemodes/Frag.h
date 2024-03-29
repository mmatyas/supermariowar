#pragma once

#include "GameMode.h"


//Fraglimit
class CGM_Frag : public CGameMode
{
    public:
        CGM_Frag();
        virtual ~CGM_Frag() {}

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
