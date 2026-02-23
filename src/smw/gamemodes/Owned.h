#pragma once

#include "GameMode.h"

//Similar to frag limit, but players get bonus frags for the number of players they have "owned"
class CGM_Owned : public CGameMode
{
    public:
        CGM_Owned();

        void think() override;
        PlayerKillType playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style) override;
        PlayerKillType playerkilledself(CPlayer &player, KillStyle style) override;
        void playerextraguy(CPlayer &player, short iType) override;

#ifdef _DEBUG
    void setdebuggoal() override {
        goal = 100;
    }
#endif

    private:
        PlayerKillType CheckWinner(CPlayer * player) override;

};
