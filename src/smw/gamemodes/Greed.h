#pragma once

#include "Classic.h"


//Greed mode (players try to steal each other's coins)
class CGM_Greed : public CGM_Classic {
public:
    CGM_Greed();
    virtual ~CGM_Greed() {}

    void init();
    PlayerKillType playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style);
    PlayerKillType playerkilledself(CPlayer &player, KillStyle style);
    void playerextraguy(CPlayer &player, short iType);

    PlayerKillType ReleaseCoins(CPlayer &player, KillStyle style);

#ifdef _DEBUG
    void setdebuggoal() {
        goal = 10;
    }
#endif
};
