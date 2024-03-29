#include "Race.h"

#include "GameValues.h"
#include "ObjectContainer.h"
#include "player.h"
#include "ResourceManager.h"
#include "Score.h"
#include "objects/overmap/RaceGoal.h"

extern CScore *score[4];
extern CObjectContainer objectcontainer[3];
extern CGameValues game_values;
extern CResourceManager* rm;


//Race
//Touch all the flying blocks in order
//Each successful curcuit you complete (before getting killed)
//Counts as one point
CGM_Race::CGM_Race() : CGameMode()
{
    goal = 10;
    gamemode = game_mode_race;
    quantity = 3;
    penalty = 0;

    SetupModeStrings("Race", "Laps", 2);
}

void CGM_Race::init()
{
    CGameMode::init();

    quantity = game_values.gamemodesettings.race.quantity;
    if (quantity < 2)
        game_values.gamemodesettings.race.quantity = quantity = 2;

    penalty = game_values.gamemodesettings.race.penalty;
    if (penalty < 0 || penalty > 2)
        game_values.gamemodesettings.race.penalty = penalty = 0;

    for (short iRaceGoal = 0; iRaceGoal < quantity; iRaceGoal++)
        objectcontainer[2].add(new OMO_RaceGoal(&rm->spr_racegoal, iRaceGoal));

    for (short iPlayer = 0; iPlayer < 4; iPlayer++)
        nextGoal[iPlayer] = 0;
}

PlayerKillType CGM_Race::playerkilledplayer(CPlayer &, CPlayer &other, KillStyle style)
{
    PenalizeRaceGoals(other);
    return PlayerKillType::Normal;
}

PlayerKillType CGM_Race::playerkilledself(CPlayer &player, KillStyle style)
{
    CGameMode::playerkilledself(player, style);

    PenalizeRaceGoals(player);
    return PlayerKillType::Normal;
}

void CGM_Race::playerextraguy(CPlayer &player, short iType)
{
    if (!gameover) {
        player.Score().AdjustScore(1 + (iType == 5 ? 1 : 0));

        //Don't end the game if the goal is infinite
        if (goal == -1)
            return;

        if (player.Score().score >= goal) {
            player.Score().SetScore(goal);
            winningteam = player.getTeamID();
            gameover = true;

            RemovePlayersButTeam(winningteam);
            SetupScoreBoard(false);
            ShowScoreBoard();
        } else if (player.Score().score >= goal - 1 && !playedwarningsound) {
            playwarningsound();
        }
    }
}


void CGM_Race::setNextGoal(short teamID)
{
    if (++nextGoal[teamID] >= quantity) {
        nextGoal[teamID] = 0;
        objectcontainer[2].removePlayerRaceGoals(teamID, -1);

        if (!gameover) {
            score[teamID]->AdjustScore(1);

            //Don't end the game if the goal is infinite
            if (goal == -1)
                return;

            if (score[teamID]->score >= goal) {
                score[teamID]->SetScore(goal);
                winningteam = teamID;
                gameover = true;

                RemovePlayersButTeam(winningteam);
                SetupScoreBoard(false);
                ShowScoreBoard();

            } else if (score[teamID]->score >= goal - 1 && !playedwarningsound) {
                playwarningsound();
            }
        }
    }
}

//Player loses control of his areas
void CGM_Race::PenalizeRaceGoals(CPlayer &player)
{
    objectcontainer[2].removePlayerRaceGoals(player.getTeamID(), nextGoal[player.getTeamID()] - 1);

    if (2 == penalty)
        nextGoal[player.getTeamID()] = 0;
    else if (1 == penalty)
        if (nextGoal[player.getTeamID()] > 0)
            nextGoal[player.getTeamID()]--;
}
