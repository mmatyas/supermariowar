#include "Star.h"

#include "eyecandy.h"
#include "GameValues.h"
#include "ObjectContainer.h"
#include "player.h"
#include "RandomNumberGenerator.h"
#include "ResourceManager.h"
#include "Score.h"
#include "objects/carriable/CO_Star.h"

extern CScore *score[4];
extern short score_cnt;

extern CObjectContainer objectcontainer[3];
extern CEyecandyContainer eyecandy[3];

extern CPlayer* list_players[4];
extern short list_players_cnt;

extern CResourceManager* rm;
extern CGameValues game_values;


CGM_Star::CGM_Star() : CGM_TimeLimit()
{
    goal = 5;
    gamemode = game_mode_star;
    SetupModeStrings("Star", "Lives", 1);
    iCurrentModeType = StarStyle::Ztar;
};

void CGM_Star::init()
{
    CGM_TimeLimit::init();

    fDisplayTimer = true;

    gameClock.Init(game_values.gamemodesettings.star.time < 1 ? 30 : game_values.gamemodesettings.star.time, true);

    iCurrentModeType = game_values.gamemodesettings.star.shine;
    if (iCurrentModeType == StarStyle::Random)
        iCurrentModeType = static_cast<StarStyle>(RANDOM_INT(3));

    fReverseScoring = goal == -1;

    //Set initial scores
    for (short iScore = 0; iScore < score_cnt; iScore++) {
        if (fReverseScoring)
            score[iScore]->SetScore(0);
        else
            score[iScore]->SetScore(goal);
    }

    for (short iStar = 0; iStar < 3; iStar++) {
        starItem[iStar] = NULL;
        starPlayer[iStar] = NULL;
    }

    SetupMode();
}

void CGM_Star::SetupMode()
{
    //Clean up old stars
    for (short iStar = 0; iStar < 3; iStar++) {
        if (starItem[iStar]) {
            starItem[iStar]->Drop();
            starItem[iStar]->dead = true;
            starItem[iStar] = NULL;
        }

        starPlayer[iStar] = NULL;
    }

    //If multi star, add more stars
    if (iCurrentModeType == StarStyle::Multi) {
        CPlayer * players[4];
        short iNumPlayers = GetScoreRankedPlayerList(players, fReverseScoring);

        for (short iStar = 0; iStar < iNumPlayers - 1; iStar++) {
            starPlayer[iStar] = players[iStar];

            starItem[iStar] = new CO_Star(&rm->spr_star, 1, iStar);
            starItem[iStar]->setPlayerColor(starPlayer[iStar]->getColorID());
            objectcontainer[1].add(starItem[iStar]);
        }
    } else { //otherwise, add just a single star
        starPlayer[0] = GetHighestScorePlayer(!fReverseScoring && iCurrentModeType == StarStyle::Ztar);

        starItem[0] = new CO_Star(&rm->spr_star, iCurrentModeType == StarStyle::Ztar ? 0 : 1, 0);
        objectcontainer[1].add(starItem[0]);
    }
}

void CGM_Star::think()
{
    if (gameover) {
        displayplayertext();
        return;
    }

    //Make sure there is a star player(s)
    if (iCurrentModeType == StarStyle::Multi) {
        for (short iStar1 = 0; iStar1 < list_players_cnt - 1; iStar1++) {
            //If we're missing a star player, then reassign them all
            if (!starPlayer[iStar1]) {
                CPlayer * players[4];
                short iNumPlayers = GetScoreRankedPlayerList(players, fReverseScoring);

                for (short iStar2 = 0; iStar2 < iNumPlayers - 1; iStar2++) {
                    starPlayer[iStar2] = players[iStar2];
                    starItem[iStar2]->setPlayerColor(starPlayer[iStar2]->getColorID());
                    starItem[iStar2]->placeStar();
                }

                break;
            }
        }
    } else {
        if (!starPlayer[0]) {
            starPlayer[0] = GetHighestScorePlayer(!fReverseScoring && game_values.gamemodesettings.star.shine == StarStyle::Ztar);
            starItem[0]->placeStar();
        }
    }

    //Count down the game time
    short iTime = gameClock.RunClock();
    if (iTime <= 5 && iTime > 0) {
        ifSoundOnPlay(rm->sfx_starwarning);
    }

    //If the game time ran out, somebody needs to die and scores changed
    if (iTime == 0) {
        gameClock.SetTime(game_values.gamemodesettings.star.time < 1 ? 30 : game_values.gamemodesettings.star.time);
        ifSoundOnPlay(rm->sfx_thunder);

        if (iCurrentModeType == StarStyle::Ztar) {
            if (score[starPlayer[0]->getTeamID()]->score > 1 || fReverseScoring)
                starPlayer[0]->KillPlayerMapHazard(true, KillStyle::Environment, false);

            if (fReverseScoring) {
                starPlayer[0]->Score().AdjustScore(1);
            } else {
                starPlayer[0]->Score().AdjustScore(-1);

                if (starPlayer[0]->Score().score <= 0) {
                    fDisplayTimer = !RemoveTeam(starPlayer[0]->getTeamID());
                    starPlayer[0] = NULL;
                }
            }

            starPlayer[0] = GetHighestScorePlayer(!fReverseScoring);
            starItem[0]->placeStar();
        } else if (iCurrentModeType == StarStyle::Shine) {
            for (short iPlayer = 0; iPlayer < list_players_cnt; iPlayer++) {
                if (starPlayer[0]->getTeamID() == list_players[iPlayer]->getTeamID())
                    continue;

                //Let the cleanup function remove the player on the last kill
                if (score[list_players[iPlayer]->getTeamID()]->score > 1 || fReverseScoring)
                    list_players[iPlayer]->KillPlayerMapHazard(true, KillStyle::Environment, false);
            }

            if (fReverseScoring) {
                starPlayer[0]->Score().AdjustScore(1);
            } else {
                for (short iTeam = 0; iTeam < score_cnt; iTeam++) {
                    if (starPlayer[0]->getTeamID() == iTeam)
                        continue;

                    score[iTeam]->AdjustScore(-1);

                    if (score[iTeam]->score <= 0) {
                        fDisplayTimer = !RemoveTeam(iTeam);
                    }
                }
            }

            starPlayer[0] = GetHighestScorePlayer(false);
            starItem[0]->placeStar();
        } else if (iCurrentModeType == StarStyle::Multi) {
            for (short iPlayer = 0; iPlayer < list_players_cnt; iPlayer++) {
                bool fFound = false;
                for (short iStar = 0; iStar < list_players_cnt - 1; iStar++) {
                    if (starPlayer[iStar] == list_players[iPlayer]) {
                        fFound = true;
                        break;
                    }
                }

                if (fFound)
                    continue;

                if (score[list_players[iPlayer]->getTeamID()]->score > 1 || fReverseScoring)
                    list_players[iPlayer]->KillPlayerMapHazard(true, KillStyle::Environment, false);

                bool fNeedRebalance = true;
                if (fReverseScoring) {
                    list_players[iPlayer]->Score().AdjustScore(1);
                } else {
                    list_players[iPlayer]->Score().AdjustScore(-1);

                    if (list_players[iPlayer]->Score().score <= 0) {
                        fDisplayTimer = !RemoveTeam(list_players[iPlayer]->getTeamID());

                        //Don't setup the mode if this is a random game because it will be setup below
                        if (game_values.gamemodesettings.star.shine != StarStyle::Random) {
                            SetupMode();
                            fNeedRebalance = false;
                        }
                    }
                }

                if (game_values.gamemodesettings.star.shine != StarStyle::Random && fNeedRebalance) {
                    CPlayer * players[4];
                    short iNumPlayers = GetScoreRankedPlayerList(players, fReverseScoring);

                    for (short iStar = 0; iStar < iNumPlayers - 1; iStar++) {
                        starPlayer[iStar] = players[iStar];
                        starItem[iStar]->setPlayerColor(starPlayer[iStar]->getColorID());
                        starItem[iStar]->placeStar();
                    }
                }

                break;
            }
        }

        //Play warning sound if needed
        if (!fReverseScoring) {
            if (!playedwarningsound) {
                short countscore = 0;
                for (short j = 0; j < score_cnt; j++) {
                    for (short k = 0; k < score_cnt; k++) {
                        if (j == k)
                            continue;

                        countscore += score[k]->score;
                    }

                    if (countscore <= 1) {
                        playwarningsound();
                        break;
                    }

                    countscore = 0;
                }
            }
        }

        //If random game, then choose a new game type
        if (game_values.gamemodesettings.star.shine == StarStyle::Random && fDisplayTimer) {
            iCurrentModeType = static_cast<StarStyle>(RANDOM_INT(3));
            SetupMode();
        }
    }
}

void CGM_Star::draw_foreground()
{
    if (fDisplayTimer) {
        gameClock.Draw();
    }
}


PlayerKillType CGM_Star::playerkilledplayer(CPlayer &, CPlayer &, KillStyle)
{
    return PlayerKillType::Normal;
}

PlayerKillType CGM_Star::playerkilledself(CPlayer &player, KillStyle style)
{
    return CGameMode::playerkilledself(player, style);
}

void CGM_Star::playerextraguy(CPlayer &player, short iType)
{
    if (!gameover) {
        if (iType == 5) {
            player.Score().AdjustScore(fReverseScoring ? -1 : 1);
        } else {
            gameClock.AddTime(iType * 10);
        }
    }
}

bool CGM_Star::isplayerstar(CPlayer * player)
{
    for (short iPlayer = 0; iPlayer < list_players_cnt - 1; iPlayer++) {
        if (starPlayer[iPlayer] == player)
            return true;
    }

    return false;
}

CPlayer * CGM_Star::swapplayer(short id, CPlayer * player)
{
    CPlayer * oldstar = NULL;
    if (starPlayer[id]) {
        oldstar = starPlayer[id];
        oldstar->Shield().turn_on();
        eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, oldstar->centerX() - 16, oldstar->centerY() - 16, 3, 8));
    }

    starPlayer[id] = player;

    if (iCurrentModeType == StarStyle::Multi)
        starItem[id]->setPlayerColor(starPlayer[id]->getColorID());

    if (starItem[id]->getType() == 1)
        eyecandy[2].add(new EC_GravText(&rm->game_font_large, player->centerX(), player->bottomY(), iCurrentModeType == StarStyle::Multi ? "Star Get!" : "Shine Get!", -VELJUMP*1.5));
    else
        eyecandy[2].add(new EC_GravText(&rm->game_font_large, player->centerX(), player->bottomY(), "Ztarred!", -VELJUMP*1.5));

    eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, player->centerX() - 16, player->centerY() - 16, 3, 8));
    ifSoundOnPlay(rm->sfx_transform);

    return oldstar;
}
