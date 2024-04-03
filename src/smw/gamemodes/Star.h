#pragma once

#include "TimeLimit.h"
#include "GameplayStyles.h"

class CO_Star;


//Star mode - shared timer ticks down and the players must pass around
//a hot potato.  When the timer hits zero, the player with the star
//loses a point.
class CGM_Star : public CGM_TimeLimit
{
    public:
        CGM_Star();
        virtual ~CGM_Star() {}

        void init();
        void think();
        void draw_foreground();
        PlayerKillType playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style);
        PlayerKillType playerkilledself(CPlayer &player, KillStyle style);
        void playerextraguy(CPlayer &player, short iType);

        bool isplayerstar(CPlayer * player);
        CPlayer * swapplayer(short id, CPlayer * player);
    CPlayer * getstarplayer(short id) {
        return starPlayer[id];
    }

    StarStyle getcurrentmodetype() const {
        return iCurrentModeType;
    }

#ifdef _DEBUG
    void setdebuggoal() {
        goal = 1;
    }
#endif

    private:
        void SetupMode();

        CO_Star * starItem[3];
        CPlayer * starPlayer[3];
        StarStyle iCurrentModeType;
        bool fDisplayTimer;
};
