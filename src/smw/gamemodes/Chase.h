#pragma once

#include "GameMode.h"

class CO_PhantoKey;


//Chase (player is chased by phanto)
class CGM_Chase : public CGameMode {
public:
    CGM_Chase();

    void init() override;
    void think() override;
    PlayerKillType playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style) override;
    PlayerKillType playerkilledself(CPlayer &player, KillStyle style) override;
    void playerextraguy(CPlayer &player, short iType) override;

    PlayerKillType CheckWinner(CPlayer * player) override;

    CPlayer * GetKeyHolder();

#ifdef _DEBUG
    void setdebuggoal() override {
        goal = 50;
    }
#endif

private:
    CO_PhantoKey * key;
};
