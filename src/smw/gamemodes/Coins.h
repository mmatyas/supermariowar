#pragma once

#include "GameMode.h"


class CGM_Coins : public CGameMode
{
    public:
        CGM_Coins();

        void init() override;
        PlayerKillType playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style) override;
        PlayerKillType playerkilledself(CPlayer &player, KillStyle style) override;
        void playerextraguy(CPlayer &player, short iType) override;

        PlayerKillType CheckWinner(CPlayer * player) override;

#ifdef _DEBUG
    void setdebuggoal() override {
        goal = 5;
    }
#endif

};
