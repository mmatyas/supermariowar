#pragma once

#include "Classic.h"


//Greed mode (players try to steal each other's coins)
class CGM_Greed : public CGM_Classic {
public:
    CGM_Greed();

    void init() override;
    PlayerKillType playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style) override;
    PlayerKillType playerkilledself(CPlayer &player, KillStyle style) override;
    void playerextraguy(CPlayer &player, short iType) override;

    PlayerKillType ReleaseCoins(CPlayer &player, KillStyle style);

#ifdef _DEBUG
    void setdebuggoal() override {
        goal = 10;
    }
#endif
};
