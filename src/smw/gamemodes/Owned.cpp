#include "Owned.h"

#include "GameValues.h"
#include "player.h"
#include "ResourceManager.h"
#include "Score.h"

extern CScore *score[4];
extern CPlayer* list_players[4];
extern short list_players_cnt;

extern CResourceManager* rm;
extern CGameValues game_values;


//Owned:
//Players rack up points like domination for each player they have "owned"
CGM_Owned::CGM_Owned() : CGameMode()
{
    goal = 200;
    gamemode = game_mode_owned;

    SetupModeStrings("Owned", "Points", 50);
}

void CGM_Owned::think()
{
    if (gameover) {
        displayplayertext();
    } else {
        static short counter = 0;

        if (++counter >= game_values.pointspeed) {
            counter = 0;

            for (short i = 0; i < list_players_cnt; i++) {
                for (short k = 0; k < list_players_cnt; k++) {
                    if (i == k)
                        continue;

                    if (list_players[k]->ownerPlayerID == list_players[i]->getGlobalID())
                        list_players[i]->Score().AdjustScore(1);
                }

                if (goal > -1) {
                    if (list_players[i]->Score().score >= goal) {
                        list_players[i]->Score().SetScore(goal);

                        winningteam = list_players[i]->getTeamID();
                        gameover = true;

                        RemovePlayersButTeam(winningteam);
                        SetupScoreBoard(false);
                        ShowScoreBoard();
                    } else if (list_players[i]->Score().score >= goal * 0.8 && !playedwarningsound) {
                        playwarningsound();
                    }
                }
            }
        }
    }
}

PlayerKillType CGM_Owned::playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style)
{
    if (!gameover) {
        //Give a bonus to the killer if he already owned this player
        if (other.ownerPlayerID == inflictor.getGlobalID())
            inflictor.Score().AdjustScore(5);

        //Release all players owned by the killed player
        for (short i = 0; i < list_players_cnt; i++) {
            if (list_players[i]->ownerPlayerID == other.getGlobalID()) {
                list_players[i]->ownerPlayerID = -1;
            }
        }

        //Assign owned status to the killed player
        if (other.getTeamID() != inflictor.getTeamID()) {
            other.ownerPlayerID = inflictor.getGlobalID();
            other.ownerColorOffsetX = inflictor.getColorID() * 48;
        }

        return CheckWinner(&inflictor);
    }

    return PlayerKillType::Normal;
}

PlayerKillType CGM_Owned::playerkilledself(CPlayer &player, KillStyle style)
{
    CGameMode::playerkilledself(player, style);

    for (short i = 0; i < list_players_cnt; i++) {
        if (list_players[i]->ownerPlayerID == player.getGlobalID()) {
            list_players[i]->ownerPlayerID = -1;
        }
    }

    return PlayerKillType::Normal;
}

void CGM_Owned::playerextraguy(CPlayer &player, short iType)
{
    if (!gameover) {
        player.Score().AdjustScore(10 * iType);
        CheckWinner(&player);
    }
}

PlayerKillType CGM_Owned::CheckWinner(CPlayer * player)
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
    } else if (player->Score().score >= goal * 0.8 && !playedwarningsound) {
        playwarningsound();
    }

    return PlayerKillType::Normal;
}
