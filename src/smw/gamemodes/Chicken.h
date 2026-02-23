#pragma once

#include "GameMode.h"


//capture the chicken
class CGM_Chicken : public CGameMode {
public:
    CGM_Chicken();

    void init() override;
    void think() override;
    void draw_foreground() override;
    PlayerKillType playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style) override;
    PlayerKillType playerkilledself(CPlayer &player, KillStyle style) override;
    void playerextraguy(CPlayer &player, short iType) override;
    PlayerKillType CheckWinner(CPlayer * player) override;

    CPlayer* chicken() const { return m_chicken; }
    void clearChicken();

#ifdef _DEBUG
    void setdebuggoal() override {
        goal = 100;
    }
#endif

private:
    CPlayer* m_chicken = nullptr;
};
