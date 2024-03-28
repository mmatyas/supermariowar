#pragma once

#include "GameMode.h"


//Special mode where players break boxes
class CGM_Boxes_MiniGame : public CGameMode
{
    public:
        CGM_Boxes_MiniGame();
        virtual ~CGM_Boxes_MiniGame() {}

        void init();
        void think();

        PlayerKillType playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style);
        PlayerKillType playerkilledself(CPlayer &player, KillStyle style);
        void playerextraguy(CPlayer &player, short iType);

        char *getMenuString(char *buffer64);

        PlayerKillType CheckWinner(CPlayer * player);
        bool SetWinner(CPlayer * player);

    bool HasStoredPowerups() {
        return false;
    }

    private:

        void ReleaseCoin(CPlayer &player);
        void ReleaseAllCoinsFromTeam(CPlayer &player);
};
