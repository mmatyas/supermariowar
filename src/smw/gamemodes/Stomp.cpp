#include "Stomp.h"

#include "GameValues.h"
#include "GlobalConstants.h"
#include "player.h"
#include "ObjectContainer.h"
#include "RandomNumberGenerator.h"
#include "ResourceManager.h"
#include "objects/moving/MO_CheepCheep.h"
#include "objects/walkingenemy/WE_BuzzyBeetle.h"
#include "objects/walkingenemy/WE_Goomba.h"
#include "objects/walkingenemy/WE_Koopa.h"
#include "objects/walkingenemy/WE_Spiny.h"


extern CObjectContainer objectcontainer[3];
extern std::vector<CPlayer*> players;
extern CGameValues game_values;
extern CResourceManager* rm;


//Stomp mode:
//Kill randomly appearing goomobas on map
//First one to kill the limit wins
CGM_Stomp::CGM_Stomp() : CGameMode()
{
    gamemode = game_mode_stomp;
    SetupModeStrings("Stomp", "Kills", 10);
}

void CGM_Stomp::init()
{
    CGameMode::init();
    ResetSpawnTimer();

    iEnemyWeightCount = 0;
    for (short iEnemy = 0; iEnemy < NUMSTOMPENEMIES; iEnemy++)
        iEnemyWeightCount += game_values.gamemodesettings.stomp.enemyweight[iEnemy];

    //if (iEnemyWeightCount == 0)
    //  iEnemyWeightCount = 1;
}


void CGM_Stomp::think()
{
    if (gameover) {
        displayplayertext();
    } else {
        for (CPlayer* player : players) {
            CheckWinner(player);
        }
    }

    if (!gameover) {
        //Randomly spawn enemies
        if (--spawntimer <= 0) {
            ResetSpawnTimer();

            //If all weights were zero, then randomly choose an enemy
            if (iEnemyWeightCount == 0) {
                iSelectedEnemy = RANDOM_INT(9);
            } else { //Otherwise randomly choose an enemy from the weighted list
                int iRandEnemy = RANDOM_INT(iEnemyWeightCount) + 1;
                iSelectedEnemy = 0;
                int iWeightCount = game_values.gamemodesettings.stomp.enemyweight[iSelectedEnemy];

                while (iWeightCount < iRandEnemy)
                    iWeightCount += game_values.gamemodesettings.stomp.enemyweight[++iSelectedEnemy];
            }

            if (0 == iSelectedEnemy)
                objectcontainer[0].add(new MO_Goomba(&rm->spr_goomba, RANDOM_BOOL(), false));
            else if (1 == iSelectedEnemy)
                objectcontainer[0].add(new MO_Koopa(&rm->spr_koopa, RANDOM_BOOL(), false, false, true));
            else if (2 == iSelectedEnemy)
                objectcontainer[2].add(new MO_CheepCheep(&rm->spr_cheepcheep));
            else if (3 == iSelectedEnemy)
                objectcontainer[0].add(new MO_Koopa(&rm->spr_redkoopa, RANDOM_BOOL(), true, false, false));
            else if (4 == iSelectedEnemy)
                objectcontainer[0].add(new MO_Spiny(&rm->spr_spiny, RANDOM_BOOL()));
            else if (5 == iSelectedEnemy)
                objectcontainer[0].add(new MO_BuzzyBeetle(&rm->spr_buzzybeetle, RANDOM_BOOL()));
            else if (6 == iSelectedEnemy)
                objectcontainer[0].add(new MO_Goomba(&rm->spr_paragoomba, RANDOM_BOOL(), true));
            else if (7 == iSelectedEnemy)
                objectcontainer[0].add(new MO_Koopa(&rm->spr_parakoopa, RANDOM_BOOL(), false, true, true));
            else
                objectcontainer[0].add(new MO_Koopa(&rm->spr_redparakoopa, RANDOM_BOOL(), true, true, true));
        }
    }
}

PlayerKillType CGM_Stomp::playerkilledplayer(CPlayer &player, CPlayer &other, KillStyle style)
{
    return PlayerKillType::Normal;
}

PlayerKillType CGM_Stomp::playerkilledself(CPlayer &player, KillStyle style)
{
    return PlayerKillType::Normal;
}

void CGM_Stomp::playerextraguy(CPlayer &player, short iType)
{
    if (!gameover) {
        player.Score().AdjustScore(iType);
        CheckWinner(&player);
    }
}

void CGM_Stomp::ResetSpawnTimer()
{
    spawntimer = (short)(RANDOM_INT(game_values.gamemodesettings.stomp.rate)) + game_values.gamemodesettings.stomp.rate;
}

PlayerKillType CGM_Stomp::CheckWinner(CPlayer * player)
{
    if (goal == -1)
        return PlayerKillType::Normal;

    if (player->Score().score >= goal) {
        player->Score().SetScore(goal);
        winningteam = player->getTeamID();
        gameover = true;

        SetupScoreBoard(false);
        ShowScoreBoard();
        RemovePlayersButTeam(winningteam);
        return PlayerKillType::Removed;
    } else if (player->Score().score >= goal - 2 && !playedwarningsound) {
        playwarningsound();
    }

    return PlayerKillType::Normal;
}
