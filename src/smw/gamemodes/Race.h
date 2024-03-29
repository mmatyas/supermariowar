#pragma once

#include "GameMode.h"


//Players tag flying targets in order to complete laps
class CGM_Race : public CGameMode
{
    public:
        virtual ~CGM_Race() {}
        CGM_Race();
        void init();
        PlayerKillType playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style);
        PlayerKillType playerkilledself(CPlayer &player, KillStyle style);
        void playerextraguy(CPlayer &player, short iType);

    short getNextGoal(short teamID) {
        return nextGoal[teamID];
    }
        void setNextGoal(short teamID);

#ifdef _DEBUG
    void setdebuggoal() {
        goal = 5;
    }
#endif

    protected:
        void PenalizeRaceGoals(CPlayer &player);
        short nextGoal[4];
        short quantity;
        short penalty;
};
