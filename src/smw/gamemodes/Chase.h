#pragma once

#include "GameMode.h"

class CO_PhantoKey;


//Chase (player is chased by phanto)
class CGM_Chase : public CGameMode {
public:
    CGM_Chase();
    virtual ~CGM_Chase() {}

    void init();
    void think();
    PlayerKillType playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style);
    PlayerKillType playerkilledself(CPlayer &player, KillStyle style);
    void playerextraguy(CPlayer &player, short iType);

    PlayerKillType CheckWinner(CPlayer * player);

    CPlayer * GetKeyHolder();

#ifdef _DEBUG
    void setdebuggoal() {
        goal = 50;
    }
#endif

private:
    CO_PhantoKey * key;
};
