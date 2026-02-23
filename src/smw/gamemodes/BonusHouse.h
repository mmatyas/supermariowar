#pragma once

#include "GameMode.h"

struct TourStop;


//Special mode where players can collect a bonus item
class CGM_Bonus : public CGameMode {
public:
    CGM_Bonus();

    void init() override;
    void draw_background() override;

    //Override so it doesn't display winner text after you choose a powerup
    void think() override {}

    PlayerKillType playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style) override {
        return PlayerKillType::None;
    }
    PlayerKillType playerkilledself(CPlayer &player, KillStyle style) override {
        return PlayerKillType::None;
    }

    void playerextraguy(CPlayer &player, short iType) override {}

    bool HasStoredPowerups() override {
        return false;
    }

private:
    TourStop* tsTourStop = nullptr;
};
