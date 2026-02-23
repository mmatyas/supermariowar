#pragma once

#include "Classic.h"


class CGM_Health : public CGM_Classic {
public:
    CGM_Health();

    void init() override;
    PlayerKillType playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style) override;
    PlayerKillType playerkilledself(CPlayer &player, KillStyle style) override;
    void playerextraguy(CPlayer &player, short iType) override;

#ifdef _DEBUG
    void setdebuggoal() override {
        goal = 5;
    }
#endif
};
