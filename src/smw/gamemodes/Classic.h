#pragma once

#include "GameMode.h"


//mariowar classic
class CGM_Classic : public CGameMode {
public:
    CGM_Classic();

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
