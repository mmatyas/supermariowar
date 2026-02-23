#pragma once

#include "GameMode.h"
#include "GameModeTimer.h"


//Timelimit
class CGM_TimeLimit : public CGameMode {
public:
    CGM_TimeLimit();

    void init() override;
    void think() override;
    void draw_foreground() override;
    void drawtime();
    PlayerKillType playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style) override;
    PlayerKillType playerkilledself(CPlayer &player, KillStyle style) override;

    void addtime(short iTime);

#ifdef _DEBUG
    void setdebuggoal() override {
        goal = 30;
    }
#endif

protected:
    GameTimerDisplay gameClock;
};
