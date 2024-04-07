#include "Eggs.h"

#include "GameValues.h"
#include "ObjectContainer.h"
#include "ResourceManager.h"
#include "objects/carriable/CO_Egg.h"
#include "objects/moving/MO_Yoshi.h"

extern CObjectContainer objectcontainer[3];
extern CResourceManager* rm;
extern CGameValues game_values;


//Egg mode:
//Grab the egg and return it to Yoshi
//Score 1 point for each
CGM_Eggs::CGM_Eggs() : CGameMode()
{
    goal = 20;
    gamemode = game_mode_eggs;

    SetupModeStrings("Yoshi's Eggs", "Eggs", 5);
};

void CGM_Eggs::init()
{
    CGameMode::init();

    //Verify that at least 1 matching yoshi and egg exist
    bool fEgg[4] = {false, false, false, false};
    bool fAtLeastOneMatch = false;

    for (short iEggs = 0; iEggs < 4; iEggs++) {
        for (short iEgg = 0; iEgg < game_values.gamemodesettings.egg.eggs[iEggs]; iEgg++) {
            if (iEgg > 9)
                break;

            fEgg[iEggs] = true;
            objectcontainer[1].add(new CO_Egg(&rm->spr_egg, iEggs));
        }
    }

    for (short iYoshis = 0; iYoshis < 4; iYoshis++) {
        for (short iYoshi = 0; iYoshi < game_values.gamemodesettings.egg.yoshis[iYoshis]; iYoshi++) {
            if (iYoshi > 9)
                break;

            if (fEgg[iYoshis])
                fAtLeastOneMatch = true;

            objectcontainer[1].add(new MO_Yoshi(&rm->spr_yoshi, iYoshis));
        }
    }

    if (!fAtLeastOneMatch) {
        objectcontainer[1].add(new CO_Egg(&rm->spr_egg, 1));
        objectcontainer[1].add(new MO_Yoshi(&rm->spr_yoshi, 1));
    }
}

PlayerKillType CGM_Eggs::playerkilledplayer(CPlayer &, CPlayer &, KillStyle)
{
    return PlayerKillType::Normal;
}

PlayerKillType CGM_Eggs::playerkilledself(CPlayer &player, KillStyle style)
{
    CGameMode::playerkilledself(player, style);

    if (player.carriedItem && player.carriedItem->getMovingObjectType() == movingobject_egg) {
        ((CO_Egg*)player.carriedItem)->placeEgg();
    }

    return PlayerKillType::Normal;
}

void CGM_Eggs::playerextraguy(CPlayer &player, short iType)
{
    if (!gameover) {
        player.Score().AdjustScore(iType);
        CheckWinner(&player);
    }
}

PlayerKillType CGM_Eggs::CheckWinner(CPlayer * player)
{
    if (goal > -1) {
        if (player->Score().score >= goal) {
            player->Score().SetScore(goal);

            winningteam = player->getTeamID();
            gameover = true;

            RemovePlayersButTeam(winningteam);
            SetupScoreBoard(false);
            ShowScoreBoard();
        } else if (player->Score().score >= goal - 2 && !playedwarningsound) {
            playwarningsound();
        }
    }

    return PlayerKillType::Normal;
}
