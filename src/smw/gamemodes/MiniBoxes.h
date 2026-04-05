#pragma once

#include "GameMode.h"


//Special mode where players break boxes
class CGM_Boxes_MiniGame : public CGameMode
{
    public:
        CGM_Boxes_MiniGame();

        void init() override;
        void think() override;

        PlayerKillType playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style) override;
        PlayerKillType playerkilledself(CPlayer &player, KillStyle style) override;
        void playerextraguy(CPlayer &player, short iType) override;

        char *getMenuString(char *buffer64);

        PlayerKillType CheckWinner(CPlayer * player) override;
        bool SetWinner(CPlayer * player);

    bool HasStoredPowerups() override {
        return false;
    }

    private:

        void ReleaseCoin(CPlayer &player);
        void ReleaseAllCoinsFromTeam(CPlayer &player);
};
