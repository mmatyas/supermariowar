#pragma once

#include "Domination.h"


//Domination (capture the area blocks)
class CGM_KingOfTheHill : public CGM_Domination
{
    public:
        CGM_KingOfTheHill();

        void init() override;
        PlayerKillType playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style) override;
        PlayerKillType playerkilledself(CPlayer &player, KillStyle style) override;
        void playerextraguy(CPlayer &player, short iType) override;

#ifdef _DEBUG
    void setdebuggoal() override {
        goal = 100;
    }
#endif
};
