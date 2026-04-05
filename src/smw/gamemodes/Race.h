#pragma once

#include "GameMode.h"


//Players tag flying targets in order to complete laps
class CGM_Race : public CGameMode
{
    public:
        CGM_Race();
        void init() override;
        PlayerKillType playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style) override;
        PlayerKillType playerkilledself(CPlayer &player, KillStyle style) override;
        void playerextraguy(CPlayer &player, short iType) override;

    short getNextGoal(short teamID) {
        return nextGoal[teamID];
    }
        void setNextGoal(short teamID);

#ifdef _DEBUG
    void setdebuggoal() override {
        goal = 5;
    }
#endif

    protected:
        void PenalizeRaceGoals(CPlayer &player);
        short nextGoal[4];
        short quantity;
        short penalty;
};
