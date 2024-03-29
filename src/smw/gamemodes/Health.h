#pragma once

#include "Classic.h"


class CGM_Health : public CGM_Classic {
public:
    CGM_Health();
    virtual ~CGM_Health() {}

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
