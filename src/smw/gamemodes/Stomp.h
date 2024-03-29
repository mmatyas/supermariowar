#pragma once

#include "GameMode.h"


//Similar to coin mode but you have to smash the most goombas/cheeps/koopas
class CGM_Stomp : public CGameMode
{
    public:
        CGM_Stomp();
        virtual ~CGM_Stomp() {}

        void init();
        void think();

        PlayerKillType playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style);
        PlayerKillType playerkilledself(CPlayer &player, KillStyle style);
        void playerextraguy(CPlayer &player, short iType);

        char *getMenuString(char *buffer64);

    private:
        void ResetSpawnTimer();
        PlayerKillType CheckWinner(CPlayer * player);

        short spawntimer;
        short iSelectedEnemy;
        short iEnemyWeightCount;
};
