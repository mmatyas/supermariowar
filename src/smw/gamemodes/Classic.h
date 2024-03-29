#pragma once

#include "GameMode.h"


//mariowar classic
class CGM_Classic : public CGameMode {
public:
    CGM_Classic();
    virtual ~CGM_Classic() {}

    virtual void init();
    virtual PlayerKillType playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style);
    virtual PlayerKillType playerkilledself(CPlayer &player, KillStyle style);
    virtual void playerextraguy(CPlayer &player, short iType);

#ifdef _DEBUG
    void setdebuggoal() {
        goal = 5;
    }
#endif
};
