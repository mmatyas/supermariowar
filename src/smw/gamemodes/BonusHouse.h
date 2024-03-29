#pragma once

#include "GameMode.h"

struct TourStop;


//Special mode where players can collect a bonus item
class CGM_Bonus : public CGameMode {
public:
    CGM_Bonus();
    virtual ~CGM_Bonus() {}

    void init();
    void draw_background();

    //Override so it doesn't display winner text after you choose a powerup
    void think() {}

    PlayerKillType playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style) {
        return PlayerKillType::None;
    }
    PlayerKillType playerkilledself(CPlayer &player, KillStyle style) {
        return PlayerKillType::None;
    }

    void playerextraguy(CPlayer &player, short iType) {}

    bool HasStoredPowerups() {
        return false;
    }

private:
    TourStop* tsTourStop = nullptr;
};
