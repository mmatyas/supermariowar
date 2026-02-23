#pragma once

#include "GameMode.h"


//Similar to coin mode but you have to smash the most goombas/cheeps/koopas
class CGM_Stomp : public CGameMode
{
    public:
        CGM_Stomp();

        void init() override;
        void think() override;

        PlayerKillType playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style) override;
        PlayerKillType playerkilledself(CPlayer &player, KillStyle style) override;
        void playerextraguy(CPlayer &player, short iType) override;

        char *getMenuString(char *buffer64);

    private:
        void ResetSpawnTimer();
        PlayerKillType CheckWinner(CPlayer * player) override;

        short spawntimer;
        short iSelectedEnemy;
        short iEnemyWeightCount;
};
