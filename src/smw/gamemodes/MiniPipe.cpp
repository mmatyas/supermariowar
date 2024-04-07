#include "MiniPipe.h"

#include "GameValues.h"
#include "ObjectContainer.h"
#include "player.h"
#include "RandomNumberGenerator.h"
#include "ResourceManager.h"
#include "objects/overmap/WO_PipeBonus.h"
#include "objects/overmap/WO_PipeCoin.h"


extern short score_cnt;
extern CObjectContainer objectcontainer[3];
extern CResourceManager* rm;
extern CGameValues game_values;


//Pipe Bonus Mini Game (used in world mode)
//Collect coins and powerups that come out of a pipe
CGM_Pipe_MiniGame::CGM_Pipe_MiniGame() : CGameMode()
{
    goal = 50;
    gamemode = game_mode_pipe_minigame;

    SetupModeStrings("Pipe Minigame", "Points", 0);
};

void CGM_Pipe_MiniGame::init()
{
    CGameMode::init();

    fReverseScoring = false;

    iNextItemTimer = 0;
    iBonusTimer = 0;
    iBonusType = 0;
    iBonusTeam = 0;
}


void CGM_Pipe_MiniGame::think()
{
    if (gameover) {
        displayplayertext();
        return;
    }

    if (--iNextItemTimer <= 0) {
        if (iBonusType == 0 || iBonusType == 2 || iBonusType == 4) {
            if (iBonusType == 2)
                iNextItemTimer = RANDOM_INT(10) + 10;
            else
                iNextItemTimer = RANDOM_INT(20) + 25;

            short iRandPowerup = RANDOM_INT(50);
            if (iBonusType == 0 && iRandPowerup < 5) { //bonuses
                objectcontainer[1].add(new OMO_PipeBonus(&rm->spr_pipegamebonus, (float)(RANDOM_INT(21) - 10) / 2.0f, -((float)RANDOM_INT(11) / 2.0f + 7.0f), 304, 256, iRandPowerup, 620, 15));
            } else if (iRandPowerup < 10) { //fireballs
                objectcontainer[1].add(new OMO_PipeBonus(&rm->spr_pipegamebonus, (float)(RANDOM_INT(21) - 10) / 2.0f, -((float)RANDOM_INT(11) / 2.0f + 7.0f), 304, 256, 5, 0, 15));
            } else { //coins
                short iRandCoin = RANDOM_INT(20);
                objectcontainer[1].add(new OMO_PipeCoin(&rm->spr_coin, (float)(RANDOM_INT(21) - 10) / 2.0f, -((float)RANDOM_INT(11) / 2.0f + 7.0f), 304, 256, -1, iRandCoin < 16 ? 2 : (iRandCoin < 19 ? 0 : 1), 15));
            }
        } else if (iBonusType == 1) {
            iNextItemTimer = RANDOM_INT(10) + 10;

            short iRandTeam = RANDOM_INT(score_cnt + 2);

            //Give an advantage to the team that got the item
            if (iRandTeam >= score_cnt)
                iRandTeam = iBonusTeam;

            short iRandPlayer = game_values.teamids[iRandTeam][RANDOM_INT(game_values.teamcounts[iRandTeam])];

            objectcontainer[1].add(new OMO_PipeCoin(&rm->spr_coin, (float)((RANDOM_INT(21)) - 10) / 2.0f, -((float)RANDOM_INT(11) / 2.0f + 7.0f), 304, 256, iRandTeam, game_values.colorids[iRandPlayer], 15));
        } else if (iBonusType == 3) {
            iNextItemTimer = RANDOM_INT(5) + 10;
            objectcontainer[1].add(new OMO_PipeCoin(&rm->spr_coin, (float)((RANDOM_INT(21)) - 10) / 2.0f, -((float)RANDOM_INT(11) / 2.0f + 7.0f), 304, 256, -1, 0, 15));
        }
    }

    if (iBonusTimer > 0 && --iBonusTimer <= 0) {
        iBonusType = 0;
        fSlowdown = false;
    }
}

PlayerKillType CGM_Pipe_MiniGame::playerkilledplayer(CPlayer &player, CPlayer &other, KillStyle style)
{
    //other.Score().AdjustScore(-2);
    return PlayerKillType::Normal;
}

PlayerKillType CGM_Pipe_MiniGame::playerkilledself(CPlayer &player, KillStyle style)
{
    //player.Score().AdjustScore(-2);
    return PlayerKillType::Normal;
}

void CGM_Pipe_MiniGame::playerextraguy(CPlayer &player, short iType)
{
    if (!gameover) {
        player.Score().AdjustScore(iType);
        CheckWinner(&player);
    }
}

PlayerKillType CGM_Pipe_MiniGame::CheckWinner(CPlayer * player)
{
    if (goal > -1) {
        if (player->Score().score >= goal) {
            player->Score().SetScore(goal);

            winningteam = player->getTeamID();
            gameover = true;

            RemovePlayersButTeam(winningteam);
            SetupScoreBoard(false);
            ShowScoreBoard();
        } else if (player->Score().score >= goal - 5 && !playedwarningsound) {
            playwarningsound();
        }
    }

    return PlayerKillType::Normal;
}

void CGM_Pipe_MiniGame::SetBonus(short iType, short iTimer, short iTeamID)
{
    iBonusType = iType;

    //This is the random bonus
    if (iBonusType == 5)
        iBonusType = RANDOM_INT(4) + 1;

    if (iBonusType == 4)
        fSlowdown = true;

    iBonusTimer = iTimer;
    iBonusTeam = iTeamID;

    if (iBonusType == 3)
        ifSoundOnPlay(rm->sfx_powerdown);
    else
        ifSoundOnPlay(rm->sfx_collectpowerup);
}
