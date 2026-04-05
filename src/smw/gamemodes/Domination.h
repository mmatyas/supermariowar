#pragma once

#include "GameMode.h"

//Domination (capture the area blocks)
class CGM_Domination : public CGameMode {
public:
    CGM_Domination();

    void init() override;
    PlayerKillType playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style) override;
    PlayerKillType playerkilledself(CPlayer &player, KillStyle style) override;
    void playerextraguy(CPlayer &player, short iType) override;
    PlayerKillType CheckWinner(CPlayer * player) override;

#ifdef _DEBUG
    void setdebuggoal() override {
        goal = 100;
    }
#endif
};
