#include "Owned.h"

#include "GameValues.h"
#include "player.h"
#include "ResourceManager.h"
#include "Score.h"

extern CScore *score[4];
extern std::vector<CPlayer*> players;

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

            for (size_t i = 0; i < players.size(); i++) {
                for (size_t k = 0; k < players.size(); k++) {
                    if (i == k)
                        continue;

                    if (players[k]->ownerPlayerID == players[i]->getGlobalID())
                        players[i]->Score().AdjustScore(1);
                }

                if (goal > -1) {
                    if (players[i]->Score().score >= goal) {
                        players[i]->Score().SetScore(goal);

                        winningteam = players[i]->getTeamID();
                        gameover = true;

                        RemovePlayersButTeam(winningteam);
                        SetupScoreBoard(false);
                        ShowScoreBoard();
                    } else if (players[i]->Score().score >= goal * 0.8 && !playedwarningsound) {
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
        for (CPlayer* player : players) {
            if (player->ownerPlayerID == other.getGlobalID()) {
                player->ownerPlayerID = -1;
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

    for (CPlayer* other : players) {
        if (other->ownerPlayerID == player.getGlobalID()) {
            other->ownerPlayerID = -1;
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
