#pragma once

#include "GameMode.h"


//capture the chicken
class CGM_Chicken : public CGameMode {
public:
    CGM_Chicken();
    virtual ~CGM_Chicken() {}

    void init() override;
    void think();
    void draw_foreground();
    PlayerKillType playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style);
    PlayerKillType playerkilledself(CPlayer &player, KillStyle style);
    void playerextraguy(CPlayer &player, short iType);
    PlayerKillType CheckWinner(CPlayer * player);

    CPlayer* chicken() const { return m_chicken; }
    void clearChicken();

#ifdef _DEBUG
    void setdebuggoal() {
        goal = 100;
    }
#endif

private:
    CPlayer* m_chicken = nullptr;
};
