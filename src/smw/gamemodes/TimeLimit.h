#pragma once

#include "GameMode.h"
#include "GameModeTimer.h"


//Timelimit
class CGM_TimeLimit : public CGameMode {
public:
    CGM_TimeLimit();
    virtual ~CGM_TimeLimit() {}

    void init();
    void think();
    void draw_foreground();
    void drawtime();
    PlayerKillType playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style);
    PlayerKillType playerkilledself(CPlayer &player, KillStyle style);

    void addtime(short iTime);

#ifdef _DEBUG
    void setdebuggoal() {
        goal = 30;
    }
#endif

protected:
    GameTimerDisplay gameClock;
};
