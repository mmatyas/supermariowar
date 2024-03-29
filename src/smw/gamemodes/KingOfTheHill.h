#pragma once

#include "Domination.h"


//Domination (capture the area blocks)
class CGM_KingOfTheHill : public CGM_Domination
{
    public:
        CGM_KingOfTheHill();
        virtual ~CGM_KingOfTheHill() {}

        void init();
        PlayerKillType playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style);
        PlayerKillType playerkilledself(CPlayer &player, KillStyle style);
        void playerextraguy(CPlayer &player, short iType);

#ifdef _DEBUG
    void setdebuggoal() {
        goal = 100;
    }
#endif
};
