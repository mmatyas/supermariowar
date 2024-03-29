#pragma once

#include "GameMode.h"

//Domination (capture the area blocks)
class CGM_Domination : public CGameMode {
public:
    CGM_Domination();
    virtual ~CGM_Domination() {}

    void init();
    PlayerKillType playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style);
    PlayerKillType playerkilledself(CPlayer &player, KillStyle style);
    void playerextraguy(CPlayer &player, short iType);
    PlayerKillType CheckWinner(CPlayer * player);

#ifdef _DEBUG
    void setdebuggoal() {
        goal = 100;
    }
#endif
};
