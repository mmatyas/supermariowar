#include "Jail.h"

#include "eyecandy.h"
#include "GameValues.h"
#include "player.h"
#include "ResourceManager.h"
#include "Score.h"
#include "objects/overmap/Area.h"

extern CScore *score[4];
extern short score_cnt;

extern CPlayer* list_players[4];
extern short list_players_cnt;

extern CEyecandyContainer eyecandy[3];
extern CResourceManager* rm;
extern CGameValues game_values;


//Owned:
//Frag limit death match, but players get bonus frags for the number of players they have "owned"
CGM_Jail::CGM_Jail() : CGM_Frag()
{
    gamemode = game_mode_jail;
    goal = 20;
    szModeName = "Jail";
}

PlayerKillType CGM_Jail::playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style)
{
    if (!gameover) {
        //Penalize killing your teammates
        if (inflictor.getTeamID() == other.getTeamID()) {
            inflictor.Score().AdjustScore(-1);
        } else {
            inflictor.Score().AdjustScore(1);
            inflictor.Jail().escape(inflictor);

            other.Jail().lockInBy(inflictor);

            //Apply rules for "Classic" jail
            if (game_values.gamemodesettings.jail.style == 0) {
                short jailedteams[4];

                short i;
                for (i = 0; i < score_cnt; i++)
                    jailedteams[i] = game_values.teamcounts[i];

                //Figure out which teams have been jailed
                for (i = 0; i < list_players_cnt; i++) {
                    if (list_players[i]->Jail().isActive()) {
                        jailedteams[list_players[i]->getTeamID()]--;
                    }
                }

                //Determine if a single team is the only one not completely jailed
                short iTeamPoint = -1;
                for (i = 0; i < score_cnt; i++) {
                    if (jailedteams[i] == 0)
                        continue;

                    if (iTeamPoint < 0) {
                        iTeamPoint = i;
                    } else {
                        iTeamPoint = -1;
                        break;
                    }
                }

                //if only a single team has not been jailed, award points
                if (iTeamPoint >= 0) {
                    short numjailedplayers = 0;

                    for (short iP = 0; iP < list_players_cnt; iP++) {
                        //If they weren't just the one killed and they were jailed, give them a transform cloud
                        if (list_players[iP] != &other && list_players[iP]->Jail().isActive()) {
                            eyecandy[2].add(new EC_SingleAnimation(&rm->spr_poof, list_players[iP]->centerX() - 24, list_players[iP]->centerY() - 24, 4, 5));
                            ifSoundOnPlay(rm->sfx_transform);
                        }

                        if (list_players[iP]->Jail().isActive() && list_players[iP]->getTeamID() != iTeamPoint)
                            numjailedplayers++;

                        list_players[iP]->Jail().timer = 0;
                    }

                    //Give extra bonus score for being on the non-jailed team
                    if (numjailedplayers > 1)
                        score[iTeamPoint]->AdjustScore(1);
                }
            }
            //Apply rules for "Owned" jail
            else if (game_values.gamemodesettings.jail.style == 1) {
                short jailedteams[4] = {-1, -1, -1, -1};

                //Figure out which teams have been jailed
                for (short i = 0; i < list_players_cnt; i++) {
                    short * piMarker = &jailedteams[list_players[i]->getTeamID()];

                    if (*piMarker == -2)
                        continue;

                    if (!list_players[i]->Jail().isActive())
                        *piMarker = -2; //Flag that the team is not completely jailed
                    else if (*piMarker == -1)
                        *piMarker = list_players[i]->Jail().owner_teamID;
                    else if (*piMarker != list_players[i]->Jail().owner_teamID)
                        *piMarker = -2; //Flag means team is not completely jailed or jailed by different teams
                }

                //Determine if a single team is the only one not completely jailed
                short iTeamPoint = -1;
                for (short i = 0; i < score_cnt; i++) {
                    short iJailOwner = -1;
                    for (short j = 0; j < score_cnt; j++) {
                        if (i == j)
                            continue;

                        //Other team is not completely jailed or jailed by different teams
                        if (jailedteams[j] == -2) {
                            iJailOwner = -1;
                            break;
                        }

                        if (iJailOwner == -1)
                            iJailOwner = jailedteams[j];
                        else if (iJailOwner != jailedteams[j]) { //Not all teams were jailed by same team
                            iJailOwner = -1;
                            break;
                        }
                    }

                    if (iJailOwner >= 0) {
                        iTeamPoint = iJailOwner;
                        break;
                    }
                }

                //if only a single team has not been jailed, award points
                if (iTeamPoint >= 0) {
                    short numjailedplayers = 0;

                    for (short i = 0; i < list_players_cnt; i++) {
                        if (list_players[i]->Jail().isActive() && list_players[i]->getTeamID() != iTeamPoint)
                            numjailedplayers++;
                    }

                    //Give extra bonus score for being on the non-jailed team
                    if (numjailedplayers > 1) {
                        score[iTeamPoint]->AdjustScore(1);

                        //Release other teams if a bonus was awarded for locking them up
                        for (short i = 0; i < list_players_cnt; i++) {
                            //Don't release players that were not jailed by this team
                            if (list_players[i]->Jail().owner_teamID != iTeamPoint)
                                continue;

                            //If they weren't just the one killed and they were jailed, give them a transform cloud
                            if (list_players[i] != &other && list_players[i]->Jail().isActive()) {
                                eyecandy[2].add(new EC_SingleAnimation(&rm->spr_poof, list_players[i]->centerX() - 24, list_players[i]->centerY() - 24, 4, 5));
                                ifSoundOnPlay(rm->sfx_transform);
                            }

                            list_players[i]->Jail().timer = 0;
                        }
                    }
                }
            }
        }

        //Don't end the game if the goal is infinite
        if (goal == -1)
            return PlayerKillType::Normal;

        if (inflictor.Score().score >= goal) {
            winningteam = inflictor.getTeamID();
            gameover = true;

            RemovePlayersButTeam(winningteam);

            SetupScoreBoard(false);
            ShowScoreBoard();

            return PlayerKillType::Removed;
        } else if (inflictor.Score().score >= goal - 3 && !playedwarningsound) {
            playwarningsound();
        }
    }

    return PlayerKillType::Normal;
}

void CGM_Jail::playerextraguy(CPlayer &player, short iType)
{
    if (!gameover) {
        player.Score().AdjustScore(iType);
        player.Jail().timer = 0;

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
        } else if (player.Score().score >= goal - 3 && !playedwarningsound) {
            playwarningsound();
        }
    }
}
