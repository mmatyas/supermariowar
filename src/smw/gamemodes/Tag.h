#pragma once

#include "GameMode.h"


class CGM_Tag : public CGameMode
{
    public:
        CGM_Tag();
        virtual ~CGM_Tag() {}

        void init();
        void think();
        PlayerKillType playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style);
        PlayerKillType playerkilledself(CPlayer &player, KillStyle style);
        void playerextraguy(CPlayer &player, short iType);

#ifdef _DEBUG
    void setdebuggoal() {
        goal = 100;
    }
#endif

};
