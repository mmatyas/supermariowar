#include "gamemodes.h"

#include "eyecandy.h"
#include "GameValues.h"
#include "gfx/gfxFont.h"
#include "net.h"
#include "object.h"
#include "ObjectContainer.h"
#include "objectgame.h"
#include "RandomNumberGenerator.h"
#include "ResourceManager.h"
#include "Score.h"

#include <cmath>
#include <cstdlib> // abs()
#include <cstring>

extern gfxFont font[3];

extern CScore *score[4];
extern short score_cnt;

extern CObjectContainer objectcontainer[3];
extern CEyecandyContainer eyecandy[3];

extern CPlayer* list_players[4];
extern short list_players_cnt;

extern CResourceManager* rm;
extern CGameValues game_values;

extern short CountAliveTeams(short * lastteam);

void RemovePlayersButTeam(short teamid)
{
    for (short iPlayer = 0; iPlayer < list_players_cnt; iPlayer++) {
        if (list_players[iPlayer]->getTeamID() != teamid) {
            list_players[iPlayer]->state = player_dead;
        }
    }
}

void RemovePlayersButHighestScoring()
{
    short iMaxScore = -1;

    //Figure out what the maximum score is
    for (short iScore = 0; iScore < score_cnt; iScore++) {
        if (score[iScore]->score > iMaxScore)
            iMaxScore = score[iScore]->score;
    }

    //Remove all players that don't have that max score
    for (short iPlayer = 0; iPlayer < list_players_cnt; iPlayer++) {
        if (list_players[iPlayer]->Score().score < iMaxScore) {
            list_players[iPlayer]->state = player_dead;
        }
    }
}

void SetupScoreBoard(bool fOrderMatters)
{
    short i, j;

    bool doneWithScore[4] = {false, false, false, false};

    short oldmax = -1;
    short max = -1;
    for (i = 0; i < score_cnt; i++) {
        oldmax = max;
        max = -1;

        for (j = 0; j < score_cnt; j++) {
            if (!doneWithScore[j]) {
                //The boxes minigame doesn't use "score" it uses "subscore[0]" to determine the winner
                if (game_values.gamemode->gamemode == game_mode_boxes_minigame) {
                    if (max == -1 || score[j]->subscore[0] > score[max]->subscore[0] ||
                            (score[j]->subscore[0] == score[max]->subscore[0] && score[j]->score > score[max]->score)) { //or it is tied but they died later in the game
                        max = j;
                    }
                } else {
                    //If this player's score is bigger
                    if (max == -1 || score[j]->score > score[max]->score ||
                            (score[j]->score == score[max]->score && score[j]->order > score[max]->order)) { //or it is tied but they died later in the game
                        max = j;
                    }
                }
            }
        }

        score[max]->displayorder = i;
        score[max]->place = i;

        if (!fOrderMatters && i > 0) {
            if (game_values.gamemode->gamemode == game_mode_boxes_minigame) {
                if (score[oldmax]->subscore[0] == score[max]->subscore[0] &&
                        score[oldmax]->score == score[max]->score)
                    score[max]->place = score[oldmax]->place;
            } else {
                if (score[oldmax]->score == score[max]->score)
                    score[max]->place = score[oldmax]->place;
            }
        }

        doneWithScore[max] = true;	//this is the next biggest score - it doesn't belong to the remaining scores from now on
    }

    //Add the treasure chests to the map in world mode if there were any awards for winning this match
    if (game_values.matchtype == MATCH_TYPE_WORLD && game_values.gamemode->winningteam > -1 && game_values.gamemode->gamemode != game_mode_bonus) {
        TourStop * tourStop = game_values.tourstops[game_values.tourstopcurrent];
        short iNumBonuses = tourStop->iNumBonuses;

        for (short iBonus = 0; iBonus < iNumBonuses; iBonus++) {
            if (tourStop->wsbBonuses[iBonus].iWinnerPlace == 0) {
                short iBonusType = tourStop->wsbBonuses[iBonus].iBonus;
                objectcontainer[0].add(new PU_TreasureChestBonus(&rm->spr_bonuschest, 1, 0, 30, 30, 1, 1, iBonusType));
                game_values.noexittimer = 0;
                game_values.noexit = false;
            }
        }
    }
}

void ShowScoreBoard()
{
    game_values.showscoreboard = true;

    short iScoreboardElementHeight = 45;
    if (game_values.gamemode->gamemode == game_mode_health || game_values.gamemode->gamemode == game_mode_collection || game_values.gamemode->gamemode == game_mode_boxes_minigame)
        iScoreboardElementHeight = 63;

    for (short i = 0; i < score_cnt; i++) {
        score[i]->destx = 309 - 34 * game_values.teamcounts[i];
        score[i]->desty = score[i]->displayorder * iScoreboardElementHeight + 140;
    }

    if (game_values.music) {
        ifsoundonstop(rm->sfx_invinciblemusic);
        ifsoundonstop(rm->sfx_timewarning);
        ifsoundonstop(rm->sfx_slowdownmusic);

        rm->backgroundmusic[1].play(true, false);
    }
}

//Returns true if all but one team is dead
bool RemoveTeam(short teamid)
{
    //If we have already removed this team then return
    if (score[teamid]->order > -1)
        return game_values.teamdeadcounter == score_cnt - 1;

    //kill all players on the dead team
    short iAnnouncementColor = -1;
    for (short iPlayer = 0; iPlayer < list_players_cnt; iPlayer++) {
        if (list_players[iPlayer]->getTeamID() == teamid) {
            if (iAnnouncementColor == -1)
                iAnnouncementColor = list_players[iPlayer]->getColorID();

            list_players[iPlayer]->state = player_dead;
        }
    }

    score[teamid]->order = game_values.teamdeadcounter++;

    //Announce that a team was removed
    if (game_values.deadteamnotice && game_values.teamdeadcounter < score_cnt - 1) {
        eyecandy[2].add(new EC_Announcement(&rm->game_font_large, &rm->spr_announcementicons, "Team Removed!", iAnnouncementColor, 90, 200));
        ifsoundonandreadyplay(rm->sfx_announcer[iAnnouncementColor + 16]);
    }

    return game_values.teamdeadcounter == score_cnt - 1;
}


GameTimerDisplay::GameTimerDisplay()
{
    iFramesPerSecond = 1000 / WAITTIME;
}

void GameTimerDisplay::Init(short iTime, bool fCountDown)
{
    timeleft = iTime;
    countdown = fCountDown;

    SetDigitCounters();
    framesleft_persecond = iFramesPerSecond;

    if (game_values.scoreboardstyle == 0)
        iScoreOffsetX = 5;
    else
        iScoreOffsetX = 291;

    SetDigitCounters();
    framesleft_persecond = iFramesPerSecond;
}

short GameTimerDisplay::RunClock()
{
    if (timeleft > 0 || !countdown) {
        if (--framesleft_persecond < 1) {
            framesleft_persecond = iFramesPerSecond;

            if (countdown) {
                --timeleft;
            } else {
                ++timeleft;
            }

            SetDigitCounters();

            return timeleft;
        }
    }

    return -1;
}


void GameTimerDisplay::Draw()
{
    rm->spr_timershade.draw(iScoreOffsetX, 5);
    rm->spr_scoretext.draw(iDigitRightDstX, 13, iDigitRightSrcX, 0, 16, 16);

    if (iDigitLeftSrcX > 0) {
        rm->spr_scoretext.draw(iDigitMiddleDstX, 13, iDigitMiddleSrcX, 0, 16, 16);
        rm->spr_scoretext.draw(iDigitLeftDstX, 13, iDigitLeftSrcX, 0, 16, 16);
    } else if (iDigitMiddleSrcX > 0) {
        rm->spr_scoretext.draw(iDigitMiddleDstX, 13, iDigitMiddleSrcX, 0, 16, 16);
    }
}

void GameTimerDisplay::SetDigitCounters()
{
    short iDigits = timeleft;
    while (iDigits > 999)
        iDigits -= 1000;

    iDigitLeftSrcX = iDigits / 100 * 16;
    iDigitMiddleSrcX = iDigits % 100 / 10 * 16;
    iDigitRightSrcX = iDigits % 10 * 16;

    if (iDigitLeftSrcX == 0) {
        if (iDigitMiddleSrcX == 0) {
            iDigitRightDstX = iScoreOffsetX + 21;
        } else {
            iDigitMiddleDstX = iScoreOffsetX + 12;
            iDigitRightDstX = iScoreOffsetX + 30;
        }
    } else {
        iDigitLeftDstX = iScoreOffsetX + 3;
        iDigitMiddleDstX = iScoreOffsetX + 21;
        iDigitRightDstX = iScoreOffsetX + 39;
    }
}

void GameTimerDisplay::SetTime(short iTime)
{
    timeleft = iTime;
    SetDigitCounters();
}

void GameTimerDisplay::AddTime(short iTime)
{
    timeleft += iTime;
    SetDigitCounters();
}


CGameMode::CGameMode()
{
    winningteam = -1;
    gamemode = game_mode_frag;
    SetupModeStrings("Free Play", "Frags", 5);
    fReverseScoring = false;
}

CGameMode::~CGameMode()
{}

void CGameMode::init()  //called once when the game is started
{
    if (goal == 1000)
        goal = 999; //Cap goal for 3 digit scoreboard

    chicken = NULL;
    tagged = NULL;
    frenzyowner = NULL;

    winningteam = -1;
    gameover = false;
    playedwarningsound = false;

    for (short iScore = 0; iScore < score_cnt; iScore++) {
        score[iScore]->SetScore(0);
    }
};

void CGameMode::think()
{
    if (netplay.active) {
        for (short k = 0; k < list_players_cnt; k++) {
            if (netplay.player_disconnected[k])
                list_players[k]->spawnText("Disconnected!");
        }
    }

    if (gameover)
        displayplayertext();
}

void CGameMode::displayplayertext()
{
    if (winningteam > -1) {
        for (short k = 0; k < list_players_cnt; k++) {
            if (list_players[k]->getTeamID() == winningteam)
                list_players[k]->spawnText("Winner!");
        }
    }
}

void CGameMode::playwarningsound()
{
    if (playedwarningsound)
        return;

    playedwarningsound = true;
    ifsoundonstop(rm->sfx_invinciblemusic);

    if (game_values.music && game_values.sound)
        rm->backgroundmusic[0].stop();

    ifSoundOnPlay(rm->sfx_timewarning);
}

void CGameMode::SetupModeStrings(const char * szMode, const char * szGoal, short iGoalSpacing)
{
    strcpy(szModeName, szMode);
    strcpy(szGoalName, szGoal);

    for (short iMode = 0; iMode < GAMEMODE_NUM_OPTIONS; iMode++) {
        if (iMode == GAMEMODE_NUM_OPTIONS - 1) {
            modeOptions[iMode].iValue = -1;
            strcpy(modeOptions[iMode].szName, "Unlimited");
        } else {
            modeOptions[iMode].iValue = (iMode + 1) * iGoalSpacing;
            sprintf(modeOptions[iMode].szName, "%d", modeOptions[iMode].iValue);
        }
    }
}

short CGameMode::playerkilledplayer(CPlayer &inflictor, CPlayer &other, killstyle style)
{
    //Penalize killing your team mates
    if (!gameover) {
        if (inflictor.getTeamID() == other.getTeamID())
            inflictor.Score().AdjustScore(-1);
        else
            inflictor.Score().AdjustScore(1);
    }

    return player_kill_normal;
};

short CGameMode::playerkilledself(CPlayer &player, killstyle style)
{
    if (player.IsBobomb())
        player.SetCorpseType(2); //flag to use bobomb corpse sprite

    return player_kill_normal;
}

void CGameMode::playerextraguy(CPlayer &player, short iType)
{
    if (!gameover)
        player.Score().AdjustScore(iType);
}

CPlayer * CGameMode::GetHighestScorePlayer(bool fGetHighest)
{
    short i, j;
    short count = 1;
    short tiedplayers[4];
    tiedplayers[0] = 0;

    //Find the first non-dead player and use them for the first player to compare to
    for (j = 0; j < list_players_cnt; j++) {
        if (!list_players[j]->isdead()) {
            count = 1;
            tiedplayers[0] = j;
            break;
        }
    }

    //Loop through all players, comparing scores to find the highest/lowest
    for (i = j + 1; i < list_players_cnt; i++) {
        if (!list_players[i]->isdead()) {
            if ((!fGetHighest && list_players[i]->Score().score < list_players[tiedplayers[0]]->Score().score) ||
                    (fGetHighest && list_players[i]->Score().score > list_players[tiedplayers[0]]->Score().score)) {
                count = 1;
                tiedplayers[0] = i;
            } else if (list_players[i]->Score().score == list_players[tiedplayers[0]]->Score().score) {
                tiedplayers[count] = i;
                count++;
            }
        }
    }

    return list_players[tiedplayers[RANDOM_INT(count)]];
}

//Returns number of players in list
short CGameMode::GetScoreRankedPlayerList(CPlayer * players[4], bool fGetHighest)
{
    short iNumPlayersInList = 0;

    for (short iIndex = 0; iIndex < list_players_cnt; iIndex++) {
        if (list_players[iIndex]->isdead())
            continue;

        players[iNumPlayersInList++] = list_players[iIndex];
    }

    //Bubble sort players in to score order
    bool fNeedSwap = true;

    while (fNeedSwap) {
        fNeedSwap = false;
        short iRandom = 0;
        for (short iIndex = 0; iIndex < iNumPlayersInList - 1; iIndex++) {
            if ((fGetHighest && players[iIndex]->Score().score < players[iIndex + 1]->Score().score) ||
                    (!fGetHighest && players[iIndex]->Score().score > players[iIndex + 1]->Score().score) ||
                    (players[iIndex]->Score().score == players[iIndex + 1]->Score().score && RANDOM_BOOL() && iRandom++ < 5)) {
                CPlayer * pTemp = players[iIndex];
                players[iIndex] = players[iIndex + 1];
                players[iIndex + 1] = pTemp;

                fNeedSwap = true;
            }
        }
    }

    return iNumPlayersInList;
}

short CGameMode::GetClosestGoal(short iGoal)
{
    short iDifference = 16000;
    short iOptionValue = 0;

    for (short iOption = 0; iOption < GAMEMODE_NUM_OPTIONS - 1; iOption++) {
        short iDiff = abs(modeOptions[iOption].iValue - iGoal);
        if (iDiff < iDifference) {
            iOptionValue = modeOptions[iOption].iValue;
            iDifference = iDiff;
        }
    }

    return iOptionValue;
}

//fraglimit:

CGM_Frag::CGM_Frag() : CGameMode()
{
    goal = 20;
    gamemode = game_mode_frag;

    SetupModeStrings("Frag Limit", "Kills", 5);
};

short CGM_Frag::playerkilledplayer(CPlayer &inflictor, CPlayer &other, killstyle style)
{
    if (gameover)
        return player_kill_normal;

    //Don't score if "sumo" style scoring is turned on
    if (game_values.gamemode->gamemode != game_mode_frag || game_values.gamemodesettings.frag.scoring == 0 || style == kill_style_push) {
        //Penalize killing your team mates
        if (inflictor.getTeamID() == other.getTeamID())
            inflictor.Score().AdjustScore(-1);
        else
            inflictor.Score().AdjustScore(1);
    }

    short iRet = CheckWinner(&inflictor);

    if (game_values.gamemode->gamemode == game_mode_frag && game_values.gamemodesettings.frag.style == 1) {
        ifSoundOnPlay(rm->sfx_powerdown);
        other.Shield().reset();
        return player_kill_nonkill;
    }

    return iRet;
}

short CGM_Frag::playerkilledself(CPlayer &player, killstyle style)
{
    CGameMode::playerkilledself(player, style);

    if (!gameover) {
        player.Score().AdjustScore(-1);

        if (game_values.gamemode->gamemode == game_mode_frag && game_values.gamemodesettings.frag.style == 1) {
            ifSoundOnPlay(rm->sfx_powerdown);
            player.Shield().reset();
            return player_kill_nonkill;
        }
    }

    return player_kill_normal;
}

void CGM_Frag::playerextraguy(CPlayer &player, short iType)
{
    if (!gameover) {
        player.Score().AdjustScore(iType);
        CheckWinner(&player);
    }
}

short CGM_Frag::CheckWinner(CPlayer * player)
{
    if (goal > -1) {
        if (player->Score().score >= goal) {
            player->Score().SetScore(goal);
            winningteam = player->getTeamID();
            gameover = true;

            RemovePlayersButTeam(winningteam);
            SetupScoreBoard(false);
            ShowScoreBoard();

            return player_kill_removed;
        } else if (player->Score().score >= goal - 2 && !playedwarningsound) {
            playwarningsound();
        }
    }

    return player_kill_normal;
}


//timelimit
CGM_TimeLimit::CGM_TimeLimit() : CGameMode()
{
    goal = 60;
    gamemode = game_mode_timelimit;

    SetupModeStrings("Time Limit", "Time", 30);
};

void CGM_TimeLimit::init()
{
    CGameMode::init();

    if (goal == -1)
        gameClock.Init(0, false);
    else
        gameClock.Init(goal, true);
}


void CGM_TimeLimit::think()
{
    CGameMode::think();
    short iTime = gameClock.RunClock();

    if (goal > 0) {
        if (iTime == 20 && !playedwarningsound) {
            playwarningsound();
        }

        if (iTime == 0) {
            //the game ends
            SetupScoreBoard(false);
            ShowScoreBoard();

            RemovePlayersButHighestScoring();
            gameover = true;

            CountAliveTeams(&winningteam);
        }
    }
}


short CGM_TimeLimit::playerkilledplayer(CPlayer &inflictor, CPlayer &other, killstyle style)
{
    if (!gameover) {
        if (game_values.gamemode->gamemode != game_mode_timelimit || game_values.gamemodesettings.time.scoring == 0 || style == kill_style_push) {
            //Penalize killing your team mates
            if (inflictor.getTeamID() == other.getTeamID())
                inflictor.Score().AdjustScore(-1);
            else
                inflictor.Score().AdjustScore(1);
        }

        if (game_values.gamemode->gamemode == game_mode_timelimit && game_values.gamemodesettings.time.style == 1) {
            ifSoundOnPlay(rm->sfx_powerdown);
            other.Shield().reset();
            return player_kill_nonkill;
        }
    }

    return player_kill_normal;
}

short CGM_TimeLimit::playerkilledself(CPlayer &player, killstyle style)
{
    CGameMode::playerkilledself(player, style);

    if (player.Score().score > 0 && !gameover)
        player.Score().AdjustScore(-1);

    return player_kill_normal;
}

void CGM_TimeLimit::draw_foreground()
{
    if (!gameover) {
        gameClock.Draw();
    }
}

void CGM_TimeLimit::addtime(short iTime)
{
    if (!gameover) {
        gameClock.AddTime(iTime);
    }
}

//mariowar (x lives - counting down)
CGM_Classic::CGM_Classic() : CGameMode()
{
    goal = 10;
    gamemode = game_mode_classic;

    SetupModeStrings("Classic", "Lives", 5);
};

void CGM_Classic::init()
{
    CGameMode::init();

    fReverseScoring = goal == -1;

    for (short iScore = 0; iScore < score_cnt; iScore++) {
        if (fReverseScoring)
            score[iScore]->SetScore(0);
        else
            score[iScore]->SetScore(goal);
    }
}


short CGM_Classic::playerkilledplayer(CPlayer &inflictor, CPlayer &other, killstyle style)
{
    if (gameover)
        return player_kill_normal;

    //If we are playing classic "sumo" mode, then only score hazard kills
    if (game_values.gamemode->gamemode != game_mode_classic || game_values.gamemodesettings.classic.scoring == 0 || style == kill_style_push) {
        if (fReverseScoring) {
            other.Score().AdjustScore(1);
        } else {
            other.Score().AdjustScore(-1);

            if (!playedwarningsound) {
                short countscore = 0;
                for (short k = 0; k < score_cnt; k++) {
                    if (&inflictor.Score() == score[k])
                        continue;

                    countscore += score[k]->score;
                }

                if (countscore <= 2) {
                    playwarningsound();
                }
            }

            if (other.Score().score <= 0) {
                RemoveTeam(other.getTeamID());
                return player_kill_removed;
            }
        }
    }

    if (game_values.gamemode->gamemode == game_mode_classic && game_values.gamemodesettings.classic.style == 1) {
        ifSoundOnPlay(rm->sfx_powerdown);
        other.Shield().reset();
        return player_kill_nonkill;
    }

    return player_kill_normal;
}

short CGM_Classic::playerkilledself(CPlayer &player, killstyle style)
{
    CGameMode::playerkilledself(player, style);

    if (!gameover) {
        if (fReverseScoring) {
            player.Score().AdjustScore(1);
        } else {
            player.Score().AdjustScore(-1);

            if (!playedwarningsound) {
                short countscore = 0;
                bool playwarning = false;
                for (short j = 0; j < score_cnt; j++) {
                    for (short k = 0; k < score_cnt; k++) {
                        if (j == k)
                            continue;

                        countscore += score[k]->score;
                    }

                    if (countscore <= 2) {
                        playwarning = true;
                        break;
                    }

                    countscore = 0;
                }

                if (playwarning)
                    playwarningsound();
            }

            if (player.Score().score <= 0) {
                RemoveTeam(player.getTeamID());
                return player_kill_removed;
            }
        }

        if (game_values.gamemode->gamemode == game_mode_classic && game_values.gamemodesettings.classic.style == 1) {
            ifSoundOnPlay(rm->sfx_powerdown);
            player.Shield().reset();
            return player_kill_nonkill;
        }
    }

    return player_kill_normal;
}

void CGM_Classic::playerextraguy(CPlayer &player, short iType)
{
    if (!gameover) {
        if (fReverseScoring)
            player.Score().AdjustScore(-iType);
        else
            player.Score().AdjustScore(iType);
    }
}


//capture the chicken
//one player is the chicken
//if he is killed the attacker becomes the chicken.
//get points for being the chicken
CGM_Chicken::CGM_Chicken() : CGameMode()
{
    goal = 200;
    gamemode = game_mode_chicken;

    SetupModeStrings("Chicken", "Points", 50);
}

void CGM_Chicken::think()
{
    if (gameover) {
        displayplayertext();
        return;
    }

    if (chicken) {
        if ( chicken->getVelX() > VELMOVING_CHICKEN )
            chicken->velx = VELMOVING_CHICKEN;
        else if (chicken->getVelX() < -VELMOVING_CHICKEN)
            chicken->velx = -VELMOVING_CHICKEN;

        static short counter = 0;

        if (chicken->isready() && !chicken->IsTanookiStatue()) {
            if (++counter >= game_values.pointspeed) {
                counter = 0;
                chicken->Score().AdjustScore(1);
                CheckWinner(chicken);
            }
        }
    }
}

void CGM_Chicken::draw_foreground()
{
    //Draw the chicken indicator around the chicken
    if (game_values.gamemodesettings.chicken.usetarget && !gameover && chicken) {
        if (chicken->iswarping())
            rm->spr_chicken.draw(chicken->leftX() - PWOFFSET - 16, chicken->topY() - PHOFFSET - 16, 0, 0, 64, 64, chicken->GetWarpState(), chicken->GetWarpPlane());
        else if (chicken->isready())
            rm->spr_chicken.draw(chicken->centerX() - 32, chicken->centerY() - 32);
    }
}


short CGM_Chicken::playerkilledplayer(CPlayer &inflictor, CPlayer &other, killstyle style)
{
    if (!chicken || &other == chicken) {
        chicken = &inflictor;
        eyecandy[2].add(new EC_GravText(&rm->game_font_large, inflictor.centerX(), inflictor.bottomY(), "Chicken!", -VELJUMP*1.5));
        //eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, inflictor.centerX() - 16, inflictor.centerY() - 16, 3, 8));
        eyecandy[2].add(new EC_SingleAnimation(&rm->spr_poof, inflictor.centerX() - 24, inflictor.centerY() - 24, 4, 5));
        ifSoundOnPlay(rm->sfx_transform);

        if (&other == chicken)
            other.SetCorpseType(1); //flag to use chicken corpse sprite
    } else if (&inflictor == chicken) {
        if (!gameover) {
            inflictor.Score().AdjustScore(5);
            return CheckWinner(&inflictor);
        }
    }

    return player_kill_normal;
}

short CGM_Chicken::playerkilledself(CPlayer &player, killstyle style)
{
    CGameMode::playerkilledself(player, style);

    if (chicken == &player) {
        player.SetCorpseType(1); //flag to use chocobo corpse sprite

        if (!gameover)
            chicken = NULL;
    }

    return player_kill_normal;
}

void CGM_Chicken::playerextraguy(CPlayer &player, short iType)
{
    if (!gameover) {
        player.Score().AdjustScore(10 * iType);
        CheckWinner(&player);
    }
}

short CGM_Chicken::CheckWinner(CPlayer * player)
{
    if (goal == -1)
        return player_kill_normal;

    if (player->Score().score >= goal) {
        player->Score().SetScore(goal);
        winningteam = player->getTeamID();
        gameover = true;

        SetupScoreBoard(false);
        ShowScoreBoard();
        RemovePlayersButTeam(winningteam);
        return player_kill_removed;
    } else if (player->Score().score >= goal * 0.8 && !playedwarningsound) {
        playwarningsound();
    }

    return player_kill_normal;
}


//tag mode (leper mode suggestion from ziotok)
//one player is "it"
//if he killes another player, they become "it"
//the player that is "it" loses life until dead.
//the "it" player is chosen at random.  Someone is
//always "it".
CGM_Tag::CGM_Tag() : CGameMode()
{
    goal = 200;
    gamemode = game_mode_tag;

    SetupModeStrings("Tag", "Points", 50);
}

void CGM_Tag::init()
{
    CGameMode::init();
    fReverseScoring = goal == -1;

    for (short iScore = 0; iScore < score_cnt; iScore++) {
        if (fReverseScoring)
            score[iScore]->SetScore(0);
        else
            score[iScore]->SetScore(goal);
    }
}

void CGM_Tag::think()
{
    if (gameover) {
        displayplayertext();
    } else {
        if (!tagged) {
            tagged = GetHighestScorePlayer(!fReverseScoring);
        }

        static short counter = 0;

        if (tagged->isready()) {
            if (++counter >= game_values.pointspeed) {
                counter = 0;

                if (fReverseScoring)
                    tagged->Score().AdjustScore(1);
                else
                    tagged->Score().AdjustScore(-1);
            }
        }

        if (fReverseScoring)
            return;

        short countscore = 0;
        bool playwarning = false;
        for (short j = 0; j < score_cnt; j++) {
            for (short k = 0; k < score_cnt; k++) {
                if (j == k)
                    continue;

                countscore += score[k]->score;
            }

            if (countscore <= goal * 0.2) {
                playwarning = true;
                break;
            }

            countscore = 0;
        }

        if (playwarning && !playedwarningsound) {
            playwarningsound();
        }

        if (tagged->Score().score <= 0) {
            RemoveTeam(tagged->getTeamID());
            tagged = NULL;
        }
    }
}

short CGM_Tag::playerkilledplayer(CPlayer &inflictor, CPlayer &other, killstyle style)
{
    if (&inflictor == tagged) {
        tagged = &other;
        inflictor.Shield().reset();
        eyecandy[2].add(new EC_GravText(&rm->game_font_large, other.centerX(), other.bottomY(), "Tagged!", -VELJUMP*1.5));
        eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, other.centerX() - 16, other.centerY() - 16, 3, 8));
        ifSoundOnPlay(rm->sfx_transform);
    }

    if (!gameover) {
        if (fReverseScoring) {
            other.Score().AdjustScore(5);
            return player_kill_normal;
        } else {
            other.Score().AdjustScore(-5);
        }

        short countscore = 0;
        for (short k = 0; k < score_cnt; k++) {
            if (&inflictor.Score() == score[k])
                continue;

            countscore += score[k]->score;
        }

        if (countscore <= goal * 0.2 && !playedwarningsound) {
            playwarningsound();
        }

        if (other.Score().score <= 0) {
            other.Score().SetScore(0);

            RemoveTeam(other.getTeamID());
            return player_kill_removed;
        }
    }

    return player_kill_normal;
}

short CGM_Tag::playerkilledself(CPlayer &player, killstyle style)
{
    CGameMode::playerkilledself(player, style);

    if (!gameover) {
        if (fReverseScoring) {
            player.Score().AdjustScore(5);
            return player_kill_normal;
        } else {
            player.Score().AdjustScore(-5);
        }

        short countscore = 0;
        bool playwarning = false;
        for (short j = 0; j < score_cnt; j++) {
            for (short k = 0; k < score_cnt; k++) {
                if (j == k)
                    continue;

                countscore += score[k]->score;
            }

            if (countscore <= goal * 0.2) {
                playwarning = true;
                break;
            }

            countscore = 0;
        }

        if (playwarning && !playedwarningsound) {
            playwarningsound();
        }

        if (player.Score().score <= 0) {
            player.Score().SetScore(0);
            RemoveTeam(player.getTeamID());
            return player_kill_removed;
        }
    }

    return player_kill_normal;
}

void CGM_Tag::playerextraguy(CPlayer &player, short iType)
{
    if (!gameover) {
        if (fReverseScoring)
            player.Score().AdjustScore(-10 * iType);
        else
            player.Score().AdjustScore(10 * iType);
    }
}


//shyguy tag mode
//First player killed becomes the shyguy
//He can then tag other players to also become shy guys
//Players that are not shy guys will be slowly scoring points
//When all players become shyguys, then the mode is reset with no shyguys
CGM_ShyGuyTag::CGM_ShyGuyTag() : CGameMode()
{
    goal = 200;
    gamemode = game_mode_shyguytag;

    SetupModeStrings("Shyguy Tag", "Points", 50);
    scorecounter = 0;
}

void CGM_ShyGuyTag::init()
{
    CGameMode::init();
    fReverseScoring = goal == -1;

    fRunClock = false;
    gameClock.Init(0, true);

    for (short iScore = 0; iScore < score_cnt; iScore++) {
        score[iScore]->SetScore(0);
    }

    for (short iPlayer = 0; iPlayer < list_players_cnt; iPlayer++) {
        list_players[iPlayer]->ownerColorOffsetX = list_players[iPlayer]->getColorID() * 48;
    }
}

void CGM_ShyGuyTag::think()
{
    if (gameover) {
        displayplayertext();
        return;
    }

    //See how many players are shy guys
    short shyguycount = CountShyGuys();

    //If we are not waiting to clear, check if we need to start waiting
    if (!fRunClock) {
        if (shyguycount == list_players_cnt) {
            if (game_values.gamemodesettings.shyguytag.freetime > 0) {
                fRunClock = true;
                gameClock.SetTime(game_values.gamemodesettings.shyguytag.freetime);
                ifSoundOnPlay(rm->sfx_starwarning);
            } else {
                FreeShyGuys();
            }
        }
    } else {
        short iTime = gameClock.RunClock();

        if (iTime == 0) { //Clear the shy guys
            fRunClock = false;
            FreeShyGuys();
        } else if (iTime > 0) {
            ifSoundOnPlay(rm->sfx_starwarning);
        }
    }

    //Award points to non shyguys
    if (shyguycount > 0) {
        if (++scorecounter >= game_values.pointspeed) {
            bool playwarning = false;
            scorecounter = 0;

            CPlayer * pCheckWinner = NULL;
            bool fAlreadyScored[4] = {false, false, false, false};
            for (short iPlayer = 0; iPlayer < list_players_cnt; iPlayer++) {
                if (!list_players[iPlayer]->shyguy) {
                    short iTeam = list_players[iPlayer]->getTeamID();
                    if (!fAlreadyScored[iTeam]) {
                        fAlreadyScored[iTeam] = true;
                        list_players[iPlayer]->Score().AdjustScore(shyguycount);

                        pCheckWinner = list_players[iPlayer];
                    }
                }
            }

            if (pCheckWinner && !fReverseScoring)
                CheckWinner(pCheckWinner);
        }
    }
}

//Draw count down timer here
void CGM_ShyGuyTag::draw_foreground()
{
    if (fRunClock) {
        gameClock.Draw();
    }
}

short CGM_ShyGuyTag::playerkilledplayer(CPlayer &inflictor, CPlayer &other, killstyle style)
{
    if (gameover || other.shyguy)
        return player_kill_normal;

    if (CountShyGuys() == 0 || game_values.gamemodesettings.shyguytag.tagtransfer != 0) {
        SetShyGuy(other.getTeamID());
    }

    return player_kill_normal;
}

short CGM_ShyGuyTag::playerkilledself(CPlayer &player, killstyle style)
{
    CGameMode::playerkilledself(player, style);

    if (!gameover && game_values.gamemodesettings.shyguytag.tagonsuicide) {
        SetShyGuy(player.getTeamID());
    }

    return player_kill_normal;
}

void CGM_ShyGuyTag::playerextraguy(CPlayer &player, short iType)
{
    if (!gameover) {
        player.Score().AdjustScore(10 * iType);
        CheckWinner(&player);
    }
}

void CGM_ShyGuyTag::SetShyGuy(short iTeam)
{
    for (short iPlayer = 0; iPlayer < list_players_cnt; iPlayer++) {
        if (list_players[iPlayer]->getTeamID() == iTeam) {
            CPlayer * player = list_players[iPlayer];
            player->shyguy = true;
            eyecandy[2].add(new EC_GravText(&rm->game_font_large, player->centerX(), player->bottomY(), "Shyguy!", -VELJUMP*1.5));
            eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, player->centerX() - 16, player->centerY() - 16, 3, 8));

            player->StripPowerups();
            player->ClearPowerupStates();
        }
    }

    ifSoundOnPlay(rm->sfx_transform);
}

void CGM_ShyGuyTag::FreeShyGuys()
{
    ifSoundOnPlay(rm->sfx_thunder);

    for (short iPlayer = 0; iPlayer < list_players_cnt; iPlayer++) {
        CPlayer * player = list_players[iPlayer];
        player->shyguy = false;
        eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, player->centerX() - 16, player->centerY() - 16, 3, 8));
    }
}

short CGM_ShyGuyTag::CheckWinner(CPlayer * player)
{
    if (gameover)
        return player_kill_normal;

    if (player->Score().score >= goal) {
        player->Score().SetScore(goal);
        SetupScoreBoard(false);
        ShowScoreBoard();

        RemovePlayersButHighestScoring();
        gameover = true;

        CountAliveTeams(&winningteam);
    } else if (!playedwarningsound && goal != -1 && player->Score().score >= goal * 0.8) {
        playwarningsound();
    }

    return player_kill_normal;
}

short CGM_ShyGuyTag::CountShyGuys()
{
    short shyguycount = 0;
    for (short iPlayer = 0; iPlayer < list_players_cnt; iPlayer++) {
        if (list_players[iPlayer]->shyguy)
            shyguycount++;
    }

    return shyguycount;
}

//Coin mode:
//Collect randomly appearing coins on map
//First one to set amount wins
CGM_Coins::CGM_Coins() : CGameMode()
{
    goal = 20;
    gamemode = game_mode_coins;

    SetupModeStrings("Coin Collection", "Coins", 5);
};

void CGM_Coins::init()
{
    CGameMode::init();

    if (game_values.gamemodesettings.coins.quantity < 1)
        game_values.gamemodesettings.coins.quantity = 1;

    for (short iCoin = 0; iCoin < game_values.gamemodesettings.coins.quantity; iCoin++)
        objectcontainer[1].add(new MO_Coin(&rm->spr_coin, 0.0f, 0.0f, 0, 0, 2, 0, 0, 0, true));
}


short CGM_Coins::playerkilledplayer(CPlayer &player, CPlayer &other, killstyle style)
{
    if (gameover)
        return player_kill_normal;

    if (game_values.gamemodesettings.coins.penalty)
        other.Score().AdjustScore(-1);

    return player_kill_normal;
}

short CGM_Coins::playerkilledself(CPlayer &player, killstyle style)
{
    if (gameover)
        return player_kill_normal;

    if (game_values.gamemodesettings.coins.penalty)
        player.Score().AdjustScore(-1);

    return player_kill_normal;
}

void CGM_Coins::playerextraguy(CPlayer &player, short iType)
{
    if (!gameover) {
        player.Score().AdjustScore(iType << 1);
        CheckWinner(&player);
    }
}

short CGM_Coins::CheckWinner(CPlayer * player)
{
    if (!gameover && goal > -1) {
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

    return player_kill_normal;
}


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

short CGM_Eggs::playerkilledplayer(CPlayer &, CPlayer &, killstyle)
{
    return player_kill_normal;
}

short CGM_Eggs::playerkilledself(CPlayer &player, killstyle style)
{
    CGameMode::playerkilledself(player, style);

    if (player.carriedItem && player.carriedItem->getMovingObjectType() == movingobject_egg) {
        ((CO_Egg*)player.carriedItem)->placeEgg();
    }

    return player_kill_normal;
}

void CGM_Eggs::playerextraguy(CPlayer &player, short iType)
{
    if (!gameover) {
        player.Score().AdjustScore(iType);
        CheckWinner(&player);
    }
}

short CGM_Eggs::CheckWinner(CPlayer * player)
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

    return player_kill_normal;
}

//Fireball:
//Frag limit death match, but powerup cards appear randomly
CGM_Frenzy::CGM_Frenzy() : CGM_Frag()
{
    gamemode = game_mode_frenzy;
    strcpy(szModeName, "Frenzy");
}

void CGM_Frenzy::init()
{
    CGameMode::init();
    timer = 0;

    iItemWeightCount = 0;
    for (short iPowerup = 0; iPowerup < NUMFRENZYCARDS; iPowerup++)
        iItemWeightCount += game_values.gamemodesettings.frenzy.powerupweight[iPowerup];
}


void CGM_Frenzy::think()
{
    if (gameover) {
        displayplayertext();
    } else {
        short iPowerupQuantity = game_values.gamemodesettings.frenzy.quantity;

        if ((iPowerupQuantity != 0 && ++timer >= game_values.gamemodesettings.frenzy.rate) || (iPowerupQuantity == 0 && !frenzyowner)) {
            timer = 0;

            if (0 == iPowerupQuantity)
                iPowerupQuantity = 1;
            if (5 < iPowerupQuantity)
                iPowerupQuantity = list_players_cnt + iPowerupQuantity - 7;

            if (objectcontainer[1].countTypes(object_frenzycard) < iPowerupQuantity) {
                if (iItemWeightCount == 0) {
                    //If all weights are zero, then choose the random powerup
                    iSelectedPowerup = NUMFRENZYCARDS - 1;
                } else {
                    //Randomly choose a powerup from the weighted list
                    int iRandPowerup = RANDOM_INT(iItemWeightCount) + 1;
                    iSelectedPowerup = 0;
                    int iWeightCount = game_values.gamemodesettings.frenzy.powerupweight[iSelectedPowerup];

                    while (iWeightCount < iRandPowerup)
                        iWeightCount += game_values.gamemodesettings.frenzy.powerupweight[++iSelectedPowerup];
                }

                objectcontainer[1].add(new MO_FrenzyCard(&rm->spr_frenzycards, iSelectedPowerup));
            }
        }
    }

    if (frenzyowner) {
        if (0 == iSelectedPowerup) {
            if (!frenzyowner->IsBobomb())
                frenzyowner = NULL;
        } else if (5 > iSelectedPowerup) {
            if (frenzyowner->powerup != iSelectedPowerup)
                frenzyowner = NULL;
        } else if (5 == iSelectedPowerup) {
            if (game_values.gamepowerups[frenzyowner->getGlobalID()] != 9)
                frenzyowner = NULL;
        } else if (6 == iSelectedPowerup) {
            if (game_values.gamepowerups[frenzyowner->getGlobalID()] != 16)
                frenzyowner = NULL;
        } else if (7 == iSelectedPowerup) {
            if (game_values.gamepowerups[frenzyowner->getGlobalID()] != 10)
                frenzyowner = NULL;
        }
    }
}


//Survival Mode! - just like mario war classic, but you have
// to dodge thwomps from the sky.  Idea from ziotok.
CGM_Survival::CGM_Survival() : CGM_Classic()
{
    goal = 20;
    gamemode = game_mode_survival;

    strcpy(szModeName, "Survival");
}

void CGM_Survival::init()
{
    CGM_Classic::init();

    rate = 3 * game_values.gamemodesettings.survival.density;
    timer = (short)(RANDOM_INT(21) - 10 + rate);
    ratetimer = 0;

    iEnemyWeightCount = 0;
    for (short iEnemy = 0; iEnemy < NUMSURVIVALENEMIES; iEnemy++)
        iEnemyWeightCount += game_values.gamemodesettings.survival.enemyweight[iEnemy];

    if (iEnemyWeightCount == 0)
        iEnemyWeightCount = 1;
}

void CGM_Survival::think()
{
    if (gameover) {
        displayplayertext();
    } else {
        if (--timer <= 0) {
            if (++ratetimer == 10) {
                ratetimer = 0;

                if (--rate < game_values.gamemodesettings.survival.density)
                    rate = game_values.gamemodesettings.survival.density;
            }

            //Randomly choose an enemy from the weighted list
            int iRandEnemy = RANDOM_INT(iEnemyWeightCount) + 1;
            iSelectedEnemy = 0;
            int iWeightCount = game_values.gamemodesettings.survival.enemyweight[iSelectedEnemy];

            while (iWeightCount < iRandEnemy)
                iWeightCount += game_values.gamemodesettings.survival.enemyweight[++iSelectedEnemy];

#pragma warning("Replace all these magic constants with proportional values")
			if (0 == iSelectedEnemy) {
                objectcontainer[2].add(new OMO_Thwomp(&rm->spr_thwomp, (short)RANDOM_INT(smw->ScreenWidth * 0.92f), (float)game_values.gamemodesettings.survival.speed / 2.0f + (float)(RANDOM_INT(20))/10.0f));
                timer = (short)(RANDOM_INT(21) - 10 + rate);
            } else if (1 == iSelectedEnemy) {
                objectcontainer[2].add(new MO_Podobo(&rm->spr_podobo, (short)RANDOM_INT(smw->ScreenWidth * 0.95f ), smw->ScreenHeight, -(float(RANDOM_INT(9)) / 2.0f) - 8.0f, -1, -1, -1, false));
                timer = (short)(RANDOM_INT(21) - 10 + rate - 20);
            } else {
                float dSpeed = ((float)(RANDOM_INT(21) + 20)) / 10.0f;
                float dVel = RANDOM_BOOL() ? dSpeed : -dSpeed;

                short x = -54;
                if (dVel < 0)
                    x = 694;

				objectcontainer[2].add(new OMO_BowserFire(&rm->spr_bowserfire, x, (short)RANDOM_INT( smw->ScreenHeight * 0.93f ), dVel, 0.0f, -1, -1, -1));
                timer = (short)(RANDOM_INT(21) - 10 + rate);
            }
        }
    }
}


//Domination (capture the area blocks)
//Touch all the dotted blocks to turn them your color
//The more blocks you have, the faster you rack up points

CGM_Domination::CGM_Domination() : CGameMode()
{
    goal = 200;
    gamemode = game_mode_domination;

    SetupModeStrings("Domination", "Points", 50);
}

void CGM_Domination::init()
{
    CGameMode::init();

    short iNumAreas = game_values.gamemodesettings.domination.quantity;

    if (iNumAreas < 1)
        game_values.gamemodesettings.domination.quantity = iNumAreas = 1;

    if (iNumAreas > 18)
        iNumAreas = 2 * list_players_cnt + iNumAreas - 22;
    else if (iNumAreas > 10)
        iNumAreas = list_players_cnt + iNumAreas - 12;

    for (short k = 0; k < iNumAreas; k++)
        objectcontainer[0].add(new OMO_Area(&rm->spr_areas, iNumAreas));
}

short CGM_Domination::playerkilledplayer(CPlayer &player, CPlayer &other, killstyle style)
{
    //Update areas the dead player owned
    objectcontainer[0].adjustPlayerAreas(&player, &other);

    return player_kill_normal;
}

short CGM_Domination::playerkilledself(CPlayer &player, killstyle style)
{
    CGameMode::playerkilledself(player, style);

    //Update areas the dead player owned
    objectcontainer[0].adjustPlayerAreas(NULL, &player);

    return player_kill_normal;
}

void CGM_Domination::playerextraguy(CPlayer &player, short iType)
{
    if (!gameover) {
        player.Score().AdjustScore(10 * iType);
        CheckWinner(&player);
    }
}

short CGM_Domination::CheckWinner(CPlayer * player)
{
    if (goal > -1) {
        if (player->Score().score >= goal) {
            player->Score().SetScore(goal);
            winningteam = player->getTeamID();
            gameover = true;

            SetupScoreBoard(false);
            ShowScoreBoard();
            RemovePlayersButTeam(winningteam);
        } else if (player->Score().score >= goal * 0.8 && !playedwarningsound) {
            playwarningsound();
        }
    }

    return player_kill_normal;
}

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

short CGM_Owned::playerkilledplayer(CPlayer &inflictor, CPlayer &other, killstyle style)
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

    return player_kill_normal;
}

short CGM_Owned::playerkilledself(CPlayer &player, killstyle style)
{
    CGameMode::playerkilledself(player, style);

    for (short i = 0; i < list_players_cnt; i++) {
        if (list_players[i]->ownerPlayerID == player.getGlobalID()) {
            list_players[i]->ownerPlayerID = -1;
        }
    }

    return player_kill_normal;
}

void CGM_Owned::playerextraguy(CPlayer &player, short iType)
{
    if (!gameover) {
        player.Score().AdjustScore(10 * iType);
        CheckWinner(&player);
    }
}

short CGM_Owned::CheckWinner(CPlayer * player)
{
    if (goal == -1)
        return player_kill_normal;

    if (player->Score().score >= goal) {
        player->Score().SetScore(goal);
        winningteam = player->getTeamID();
        gameover = true;

        SetupScoreBoard(false);
        ShowScoreBoard();
        RemovePlayersButTeam(winningteam);
        return player_kill_removed;
    } else if (player->Score().score >= goal * 0.8 && !playedwarningsound) {
        playwarningsound();
    }

    return player_kill_normal;
}


//Owned:
//Frag limit death match, but players get bonus frags for the number of players they have "owned"
CGM_Jail::CGM_Jail() : CGM_Frag()
{
    gamemode = game_mode_jail;
    goal = 20;

    strcpy(szModeName, "Jail");
}

short CGM_Jail::playerkilledplayer(CPlayer &inflictor, CPlayer &other, killstyle style)
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
            return player_kill_normal;

        if (inflictor.Score().score >= goal) {
            winningteam = inflictor.getTeamID();
            gameover = true;

            RemovePlayersButTeam(winningteam);

            SetupScoreBoard(false);
            ShowScoreBoard();

            return player_kill_removed;
        } else if (inflictor.Score().score >= goal - 3 && !playedwarningsound) {
            playwarningsound();
        }
    }

    return player_kill_normal;
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
    //	iEnemyWeightCount = 1;
}


void CGM_Stomp::think()
{
    if (gameover) {
        displayplayertext();
    } else {
        for (short i = 0; i < list_players_cnt; i++) {
            CheckWinner(list_players[i]);
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

short CGM_Stomp::playerkilledplayer(CPlayer &player, CPlayer &other, killstyle style)
{
    return player_kill_normal;
}

short CGM_Stomp::playerkilledself(CPlayer &player, killstyle style)
{
    return player_kill_normal;
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

short CGM_Stomp::CheckWinner(CPlayer * player)
{
    if (goal == -1)
        return player_kill_normal;

    if (player->Score().score >= goal) {
        player->Score().SetScore(goal);
        winningteam = player->getTeamID();
        gameover = true;

        SetupScoreBoard(false);
        ShowScoreBoard();
        RemovePlayersButTeam(winningteam);
        return player_kill_removed;
    } else if (player->Score().score >= goal - 2 && !playedwarningsound) {
        playwarningsound();
    }

    return player_kill_normal;
}


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

short CGM_Race::playerkilledplayer(CPlayer &, CPlayer &other, killstyle style)
{
    PenalizeRaceGoals(other);
    return player_kill_normal;
}

short CGM_Race::playerkilledself(CPlayer &player, killstyle style)
{
    CGameMode::playerkilledself(player, style);

    PenalizeRaceGoals(player);
    return player_kill_normal;
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


/********************************************
* star mode
********************************************/
CGM_Star::CGM_Star() : CGM_TimeLimit()
{
    goal = 5;
    gamemode = game_mode_star;
    SetupModeStrings("Star", "Lives", 1);
    iCurrentModeType = 0;
};

void CGM_Star::init()
{
    CGM_TimeLimit::init();

    fDisplayTimer = true;

    gameClock.Init(game_values.gamemodesettings.star.time < 1 ? 30 : game_values.gamemodesettings.star.time, true);

    if (game_values.gamemodesettings.star.shine < 0 || game_values.gamemodesettings.star.shine > 3)
        game_values.gamemodesettings.star.shine = 0;

    iCurrentModeType = game_values.gamemodesettings.star.shine;
    if (iCurrentModeType == 3)
        iCurrentModeType = RANDOM_INT(3);

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
    if (iCurrentModeType == 2) {
        CPlayer * players[4];
        short iNumPlayers = GetScoreRankedPlayerList(players, fReverseScoring);

        for (short iStar = 0; iStar < iNumPlayers - 1; iStar++) {
            starPlayer[iStar] = players[iStar];

            starItem[iStar] = new CO_Star(&rm->spr_star, 1, iStar);
            starItem[iStar]->setPlayerColor(starPlayer[iStar]->getColorID());
            objectcontainer[1].add(starItem[iStar]);
        }
    } else { //otherwise, add just a single star
        starPlayer[0] = GetHighestScorePlayer(!fReverseScoring && iCurrentModeType == 0);

        starItem[0] = new CO_Star(&rm->spr_star, iCurrentModeType == 0 ? 0 : 1, 0);
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
    if (iCurrentModeType == 2) {
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
            starPlayer[0] = GetHighestScorePlayer(!fReverseScoring && !game_values.gamemodesettings.star.shine);
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

        if (iCurrentModeType == 0) {
            if (score[starPlayer[0]->getTeamID()]->score > 1 || fReverseScoring)
                starPlayer[0]->KillPlayerMapHazard(true, kill_style_environment, false);

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
        } else if (iCurrentModeType == 1) {
            for (short iPlayer = 0; iPlayer < list_players_cnt; iPlayer++) {
                if (starPlayer[0]->getTeamID() == list_players[iPlayer]->getTeamID())
                    continue;

                //Let the cleanup function remove the player on the last kill
                if (score[list_players[iPlayer]->getTeamID()]->score > 1 || fReverseScoring)
                    list_players[iPlayer]->KillPlayerMapHazard(true, kill_style_environment, false);
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
        } else if (iCurrentModeType == 2) {
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
                    list_players[iPlayer]->KillPlayerMapHazard(true, kill_style_environment, false);

                bool fNeedRebalance = true;
                if (fReverseScoring) {
                    list_players[iPlayer]->Score().AdjustScore(1);
                } else {
                    list_players[iPlayer]->Score().AdjustScore(-1);

                    if (list_players[iPlayer]->Score().score <= 0) {
                        fDisplayTimer = !RemoveTeam(list_players[iPlayer]->getTeamID());

                        //Don't setup the mode if this is a random game because it will be setup below
                        if (game_values.gamemodesettings.star.shine != 3) {
                            SetupMode();
                            fNeedRebalance = false;
                        }
                    }
                }

                if (game_values.gamemodesettings.star.shine != 3 && fNeedRebalance) {
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
        if (game_values.gamemodesettings.star.shine == 3 && fDisplayTimer) {
#pragma warning ("neagix: This should be 4, possibly?")
            iCurrentModeType = RANDOM_INT(3);
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


short CGM_Star::playerkilledplayer(CPlayer &, CPlayer &, killstyle)
{
    return player_kill_normal;
}

short CGM_Star::playerkilledself(CPlayer &player, killstyle style)
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

    if (iCurrentModeType == 2)
        starItem[id]->setPlayerColor(starPlayer[id]->getColorID());

    if (starItem[id]->getType() == 1)
        eyecandy[2].add(new EC_GravText(&rm->game_font_large, player->centerX(), player->bottomY(), iCurrentModeType == 2 ? "Star Get!" : "Shine Get!", -VELJUMP*1.5));
    else
        eyecandy[2].add(new EC_GravText(&rm->game_font_large, player->centerX(), player->bottomY(), "Ztarred!", -VELJUMP*1.5));

    eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, player->centerX() - 16, player->centerY() - 16, 3, 8));
    ifSoundOnPlay(rm->sfx_transform);

    return oldstar;
}


//Capture The Flag mode - each team has a base and a flag
//Protect your colored flag from being taken and score a point
//for stealing another teams flag and returning it to your base
CGM_CaptureTheFlag::CGM_CaptureTheFlag() : CGameMode()
{
    goal = 20;
    gamemode = game_mode_ctf;

    SetupModeStrings("Capture The Flag", "Flags", 5);
};

void CGM_CaptureTheFlag::init()
{
    CGameMode::init();

    bool fTeamUsed[4] = {false, false, false, false};

    for (short iPlayer = 0; iPlayer < list_players_cnt; iPlayer++) {
        short iTeamID = list_players[iPlayer]->getTeamID();
        if (!fTeamUsed[iTeamID]) {
            fTeamUsed[iTeamID] = true;

            short iColorID = list_players[iPlayer]->getColorID();
            MO_FlagBase * base = new MO_FlagBase(&rm->spr_flagbases, iTeamID, iColorID);
            objectcontainer[0].add(base);

            if (!game_values.gamemodesettings.flag.centerflag) {
                CO_Flag * flag = new CO_Flag(&rm->spr_flags, base, iTeamID, iColorID);
                objectcontainer[1].add(flag);
            }
        }
    }

    if (game_values.gamemodesettings.flag.centerflag) {
        CO_Flag * centerflag = new CO_Flag(&rm->spr_flags, NULL, -1, -1);
        objectcontainer[1].add(centerflag);
    }
}

short CGM_CaptureTheFlag::playerkilledplayer(CPlayer &, CPlayer &, killstyle)
{
    return player_kill_normal;
}

short CGM_CaptureTheFlag::playerkilledself(CPlayer &player, killstyle style)
{
    CGameMode::playerkilledself(player, style);

    if (player.carriedItem && player.carriedItem->getMovingObjectType() == movingobject_flag) {
        ((CO_Flag*)player.carriedItem)->placeFlag();
        ifSoundOnPlay(rm->sfx_transform);
    }

    return player_kill_normal;
}

void CGM_CaptureTheFlag::playerextraguy(CPlayer &player, short iType)
{
    if (gameover)
        return;

    player.Score().AdjustScore(iType);
    CheckWinner(&player);
}

short CGM_CaptureTheFlag::CheckWinner(CPlayer * player)
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

    return player_kill_normal;
}

//King of the Hill (Control an area for a certain amount of time)
CGM_KingOfTheHill::CGM_KingOfTheHill() : CGM_Domination()
{
    goal = 200;
    gamemode = game_mode_koth;

    SetupModeStrings("King Of The Hill", "Points", 50);
}

void CGM_KingOfTheHill::init()
{
    CGameMode::init();
    objectcontainer[2].add(new OMO_KingOfTheHillZone(&rm->spr_kingofthehillarea));
}

short CGM_KingOfTheHill::playerkilledplayer(CPlayer &inflictor, CPlayer &other, killstyle style)
{
    return player_kill_normal;
}

short CGM_KingOfTheHill::playerkilledself(CPlayer &player, killstyle style)
{
    return CGameMode::playerkilledself(player, style);
}

void CGM_KingOfTheHill::playerextraguy(CPlayer &player, short iType)
{
    if (!gameover) {
        player.Score().AdjustScore(5 * iType);
        CheckWinner(&player);
    }
}
/*
kill_style_stomp = 0,
kill_style_star = 1,
kill_style_fireball = 2,
kill_style_bobomb = 3,
kill_style_bounce = 4,
kill_style_pow = 5,
kill_style_goomba = 6,
kill_style_bulletbill = 7,
kill_style_hammer = 8,
kill_style_shell = 9,
kill_style_throwblock = 10,
kill_style_cheepcheep = 11,
kill_style_koopa = 12,
kill_style_boomerang = 13,
kill_style_feather = 14,
kill_style_iceblast = 15,
kill_style_podobo = 16,
kill_style_bomb = 17,
kill_style_leaf = 18,
kill_style_pwings = 19,
kill_style_kuriboshoe = 20,
kill_style_poisonmushroom = 21,
kill_style_environment = 22,
kill_style_push = 23,
kill_style_buzzybeetle = 24,
kill_style_spiny = 25,
kill_style_phanto = 26
*/
//Greed - steal other players coins - if you have 0 coins, you're removed from the game!
short g_iKillStyleDamage[KILL_STYLE_LAST] = {5,5,3,8,3,5,2,3,3,5, 5,2,2,3,5,8,3,8,5,5, 8,5,3,3,2,2,2};

CGM_Greed::CGM_Greed() : CGM_Classic()
{
    goal = 40;
    gamemode = game_mode_greed;

    SetupModeStrings("Greed", "Coins", 10);
};

void CGM_Greed::init()
{
    CGameMode::init();

    short iGoal = goal == -1 ? 0 : goal;

    for (short iScore = 0; iScore < score_cnt; iScore++) {
        score[iScore]->SetScore(iGoal);
    }
}

short CGM_Greed::playerkilledplayer(CPlayer &inflictor, CPlayer &other, killstyle style)
{
    if (gameover)
        return player_kill_normal;

    //create coins around player
    return ReleaseCoins(other, style);
}

short CGM_Greed::playerkilledself(CPlayer &player, killstyle style)
{
    if (gameover)
        return player_kill_normal;

    //create coins around player
    return ReleaseCoins(player, style);
}

void CGM_Greed::playerextraguy(CPlayer &player, short iType)
{
    if (!gameover)
        player.Score().AdjustScore(iType * 5);
}

short CGM_Greed::ReleaseCoins(CPlayer &player, killstyle style)
{
    ifSoundOnPlay(rm->sfx_cannon);

    player.Shield().turn_on();

    short iDamage = g_iKillStyleDamage[style] * game_values.gamemodesettings.greed.multiplier / 2;

    if (goal != -1) {
        if (player.Score().score < iDamage)
            iDamage = player.Score().score;

        player.Score().AdjustScore(-iDamage);
    }

    short ix = player.centerX() - 16;
    short iy = player.centerY() - 16;

    for (short k = 0; k < iDamage; k++) {
        float vel = 7.0f + ((float)RANDOM_INT(9)) / 2.0f;
        float angle = -((float)RANDOM_INT(314)) / 100.0f;
        float velx = vel * cos(angle);
        float vely = vel * sin(angle);

        objectcontainer[1].add(new MO_Coin(&rm->spr_coin, velx, vely, ix, iy, player.getColorID(), player.getTeamID(), 1, 30, false));
    }

    //Play warning sound if game is almost over
    if (goal != -1 && !playedwarningsound) {
        bool playwarning = false;
        for (short j = 0; j < score_cnt; j++) {
            short countscore = 0;
            for (short k = 0; k < score_cnt; k++) {
                if (j == k)
                    continue;

                countscore += score[k]->score;
            }

            if (countscore <= 10) {
                playwarning = true;
                break;
            }
        }

        if (playwarning)
            playwarningsound();
    }

    if (goal != -1 && player.Score().score <= 0) {
        RemoveTeam(player.getTeamID());
        return player_kill_removed;
    }

    return player_kill_nonkill;
}

//mariowar (x lives - counting down)
CGM_Health::CGM_Health() : CGM_Classic()
{
    goal = 5;
    gamemode = game_mode_health;

    SetupModeStrings("Health", "Lives", 1);
};

void CGM_Health::init()
{
    CGM_Classic::init();

    for (short iScore = 0; iScore < score_cnt; iScore++) {
        score[iScore]->subscore[0] = game_values.gamemodesettings.health.startlife;
        score[iScore]->subscore[1] = score[iScore]->subscore[0];
    }
}

short CGM_Health::playerkilledplayer(CPlayer &inflictor, CPlayer &other, killstyle style)
{
    if (gameover)
        return player_kill_normal;

    if (style == kill_style_bobomb || style == kill_style_bomb || style == kill_style_iceblast)
        other.Score().subscore[0] -= 2;
    else
        other.Score().subscore[0]--;

    if (other.Score().subscore[0] <= 0) {
        other.Score().subscore[0] = 0;
        short iRet = CGM_Classic::playerkilledplayer(inflictor, other, style);

        if (iRet == player_kill_normal)
            other.Score().subscore[0] = other.Score().subscore[1];

        return iRet;
    } else {
        ifSoundOnPlay(rm->sfx_powerdown);
        other.Shield().reset();
    }

    return player_kill_nonkill;
}

short CGM_Health::playerkilledself(CPlayer &player, killstyle style)
{
    if (gameover)
        return player_kill_normal;

    if (style == kill_style_bobomb || style == kill_style_bomb || style == kill_style_environment)
        player.Score().subscore[0] -= 2;
    else
        player.Score().subscore[0]--;

    if (player.Score().subscore[0] <= 0) {
        player.Score().subscore[0] = 0;
        short iRet = CGM_Classic::playerkilledself(player, style);

        if (iRet == player_kill_normal)
            player.Score().subscore[0] = player.Score().subscore[1];

        return iRet;
    } else {
        ifSoundOnPlay(rm->sfx_powerdown);
        player.Shield().reset();
    }

    return player_kill_nonkill;
}

void CGM_Health::playerextraguy(CPlayer &player, short iType)
{
    if (!gameover) {
        player.Score().subscore[0] += iType;

        if (player.Score().subscore[0] > player.Score().subscore[1])
            player.Score().subscore[0] = player.Score().subscore[1];
    }
}


//Collection (collect cards for points)
CGM_Collection::CGM_Collection() : CGameMode()
{
    goal = 30;
    gamemode = game_mode_collection;

    SetupModeStrings("Card Collection", "Points", 10);
}

void CGM_Collection::init()
{
    CGameMode::init();
    timer = 0;

    //Zero out the number of cards teams have
    for (short iScore = 0; iScore < score_cnt; iScore++) {
        score[iScore]->subscore[0] = 0;
        score[iScore]->subscore[1] = 0;
    }
}

void CGM_Collection::think()
{
    if (++timer >= game_values.gamemodesettings.collection.rate) {
        timer = 0;

        short iPowerupQuantity = game_values.gamemodemenusettings.collection.quantity;

        if (5 < iPowerupQuantity)
            iPowerupQuantity = list_players_cnt + iPowerupQuantity - 7;

        if (objectcontainer[1].countMovingTypes(movingobject_collectioncard) < iPowerupQuantity) {
            short iRandom = RANDOM_INT(5);
            short iRandomCard = 0;
            if (iRandom == 4)
                iRandomCard = 2;
            else if (iRandom >= 2)
                iRandomCard = 1;

            objectcontainer[1].add(new MO_CollectionCard(&rm->spr_collectcards, 0, iRandomCard, 0, 0.0f, 0.0f, 0, 0));
        }
    }

    if (gameover) {
        displayplayertext();
        return;
    }

    //Check if this team has collected 3 cards
    for (short iScore = 0; iScore < score_cnt; iScore++) {
        if (score[iScore]->subscore[0] >= 3) {
            if (++score[iScore]->subscore[2] >= game_values.gamemodemenusettings.collection.banktime) {
                short iPoints = 1;

                if ((score[iScore]->subscore[1] & 63) == 0) //All Mushrooms
                    iPoints = 2;
                else if ((score[iScore]->subscore[1] & 63) == 21) //All Flowers
                    iPoints = 3;
                else if ((score[iScore]->subscore[1] & 63) == 42) //All Stars
                    iPoints = 5;

                score[iScore]->AdjustScore(iPoints);

                score[iScore]->subscore[0] = 0;
                score[iScore]->subscore[1] = 0;
                score[iScore]->subscore[2] = 0;
            }
        }
    }

    for (short i = 0; i < list_players_cnt; i++) {
        CheckWinner(list_players[i]);
    }
}

short CGM_Collection::playerkilledplayer(CPlayer &player, CPlayer &other, killstyle style)
{
    //Causes a card to come out of the player
    ReleaseCard(other);

    return player_kill_normal;
}

short CGM_Collection::playerkilledself(CPlayer &player, killstyle style)
{
    CGameMode::playerkilledself(player, style);

    //Causes a card to come out of the player
    ReleaseCard(player);

    return player_kill_normal;
}

void CGM_Collection::ReleaseCard(CPlayer &player)
{
    if (player.Score().subscore[0] > 0) {
        short ix = player.centerX() - 16;
        short iy = player.centerY() - 16;

        float vel = 7.0f + (float)RANDOM_INT(9) / 2.0f;
        float angle = -(float)RANDOM_INT(314) / 100.0f;
        float velx = vel * cos(angle);
        float vely = vel * sin(angle);

        player.Score().subscore[0]--;

        short iCardMask = 3 << (player.Score().subscore[0] << 1);
        short iCardMaskInverted = ~iCardMask;

        short iValue = (player.Score().subscore[1] & iCardMask) >> (player.Score().subscore[0] << 1);

        player.Score().subscore[1] &= iCardMaskInverted;

        objectcontainer[1].add(new MO_CollectionCard(&rm->spr_collectcards, 1, iValue, 30, velx, vely, ix, iy));
    }
}

void CGM_Collection::playerextraguy(CPlayer &player, short iType)
{
    if (!gameover) {
        player.Score().AdjustScore(iType);
        CheckWinner(&player);
    }
}

short CGM_Collection::CheckWinner(CPlayer * player)
{
    if (goal > -1) {
        if (player->Score().score >= goal) {
            player->Score().SetScore(goal);
            winningteam = player->getTeamID();
            gameover = true;

            SetupScoreBoard(false);
            ShowScoreBoard();
            RemovePlayersButTeam(winningteam);

            for (short iScore = 0; iScore < score_cnt; iScore++) {
                score[iScore]->subscore[0] = 0;
                score[iScore]->subscore[1] = 0;
                score[iScore]->subscore[2] = 0;
            }
        } else if (player->Score().score >= goal * 0.8 && !playedwarningsound) {
            playwarningsound();
        }
    }

    return player_kill_normal;
}


//Chase (hold a key for points while phantos chase you)
CGM_Chase::CGM_Chase() : CGameMode()
{
    goal = 200;
    gamemode = game_mode_chase;

    SetupModeStrings("Phanto", "Points", 50);
}

void CGM_Chase::init()
{
    CGameMode::init();

    //Add phantos based on settings
    for (short iPhanto = 0; iPhanto < 3; iPhanto++) {
        for (short iNumPhantos = 0; iNumPhantos < game_values.gamemodesettings.chase.phantoquantity[iPhanto]; iNumPhantos++)
			objectcontainer[1].add(new OMO_Phanto(&rm->spr_phanto, RANDOM_INT(smw->ScreenWidth), RANDOM_BOOL() ? -32 - CRUNCHMAX : smw->ScreenHeight, 0.0f, 0.0f, iPhanto));
    }

    //Add a key
    key = new CO_PhantoKey(&rm->spr_phantokey);
    objectcontainer[1].add(key);
}

void CGM_Chase::think()
{
    if (gameover) {
        displayplayertext();
        return;
    }

    CPlayer * keyholder = key->owner;
    if (keyholder) {
        static short counter = 0;

        if (keyholder->isready() && !keyholder->IsTanookiStatue()) {
            if (++counter >= game_values.pointspeed) {
                counter = 0;
                keyholder->Score().AdjustScore(1);
                CheckWinner(keyholder);
            }
        }
    }
}

short CGM_Chase::playerkilledplayer(CPlayer &player, CPlayer &other, killstyle style)
{
    return player_kill_normal;
}

short CGM_Chase::playerkilledself(CPlayer &player, killstyle style)
{
    CGameMode::playerkilledself(player, style);

    return player_kill_normal;
}

void CGM_Chase::playerextraguy(CPlayer &player, short iType)
{
    if (!gameover) {
        player.Score().AdjustScore(5 * iType);
        CheckWinner(&player);
    }
}

short CGM_Chase::CheckWinner(CPlayer * player)
{
    if (goal == -1)
        return player_kill_normal;

    if (player->Score().score >= goal) {
        player->Score().SetScore(goal);
        winningteam = player->getTeamID();
        gameover = true;

        SetupScoreBoard(false);
        ShowScoreBoard();
        RemovePlayersButTeam(winningteam);

        return player_kill_removed;
    } else if (player->Score().score >= goal * 0.8 && !playedwarningsound) {
        playwarningsound();
    }

    return player_kill_normal;
}

CPlayer * CGM_Chase::GetKeyHolder()
{
    return key->owner;
}


//Boss Mode
//Person to score fatal hit to boss wins!
CGM_Boss_MiniGame::CGM_Boss_MiniGame() : CGameMode()
{
    gamemode = game_mode_boss_minigame;
    SetupModeStrings("Boss", "Lives", 5);
    iBossType = 0;
}

void CGM_Boss_MiniGame::init()
{
    CGameMode::init();

    fReverseScoring = false;

    enemytimer = (short)(RANDOM_INT(120) + 120);
    poweruptimer = 120;

    for (short iScore = 0; iScore < score_cnt; iScore++)
        score[iScore]->SetScore(goal);

    objectcontainer[0].add(new MO_SledgeBrother(&rm->spr_sledgebrothers, (iBossType == 0 ? 256 : (iBossType == 1 ? 256 : smw->ScreenWidth/2)), iBossType));
}


void CGM_Boss_MiniGame::think()
{
    if (!gameover && list_players_cnt == 0) {
        gameover = true;

        if (game_values.music) {
            ifsoundonstop(rm->sfx_invinciblemusic);
            ifsoundonstop(rm->sfx_timewarning);
            ifsoundonstop(rm->sfx_slowdownmusic);
            ifSoundOnPlay(rm->sfx_gameover);

            rm->backgroundmusic[1].stop();
        }
    }

    if (gameover) {
        displayplayertext();
    } else {
        if (iBossType == 0) {
            //Randomly spawn koopas
            if (--enemytimer <= 0) {
                objectcontainer[0].add(new MO_Koopa(&rm->spr_koopa, RANDOM_BOOL(), false, false, true));
                enemytimer = (short)RANDOM_INT(120) + 120;  //Spawn koopas slowly
            }
        } else if (iBossType == 1) {

        } else if (iBossType == 2) {
            //Only create podobos if the difficulty is moderate or greater
            if (--enemytimer <= 0 && game_values.gamemodesettings.boss.difficulty >= 2) {
                objectcontainer[2].add(new MO_Podobo(&rm->spr_podobo, (short)RANDOM_INT(smw->ScreenWidth * 0.95f), smw->ScreenHeight, -(float(RANDOM_INT(9)) / 2.0f) - 9.0f, -1, -1, -1, false));
                enemytimer = (short)(RANDOM_INT(80) + 60);
            }

            if (--poweruptimer <= 0) {
                poweruptimer = (short)(RANDOM_INT(80) + 60);

                if (objectcontainer[1].countTypes(object_frenzycard) < list_players_cnt) {
                    objectcontainer[1].add(new MO_FrenzyCard(&rm->spr_frenzycards, 0));
                }
            }
        }
    }
}

void CGM_Boss_MiniGame::draw_foreground()
{
    if (gameover) {
        if (winningteam == -1) {
            rm->game_font_large.drawCentered(smw->ScreenWidth/2, 96, "You Failed To Defeat");

            if (iBossType == 0)
                rm->game_font_large.drawCentered(smw->ScreenWidth/2, 118, "The Mighty Sledge Brother");
            else if (iBossType == 1)
                rm->game_font_large.drawCentered(smw->ScreenWidth/2, 118, "The Mighty Bomb Brother");
            else if (iBossType == 2)
                rm->game_font_large.drawCentered(smw->ScreenWidth/2, 118, "The Mighty Flame Brother");
        }
    }
}

short CGM_Boss_MiniGame::playerkilledplayer(CPlayer &inflictor, CPlayer &other, killstyle style)
{
    if (!gameover) {
        other.Score().AdjustScore(-1);

        if (!playedwarningsound) {
            short countscore = 0;
            for (short k = 0; k < score_cnt; k++)
                countscore += score[k]->score;

            if (countscore <= 2)
                playwarningsound();
        }

        if (other.Score().score <= 0) {
            RemoveTeam(other.getTeamID());
            return player_kill_removed;
        }
    }

    return player_kill_normal;
}

short CGM_Boss_MiniGame::playerkilledself(CPlayer &player, killstyle style)
{
    CGameMode::playerkilledself(player, style);

    if (!gameover) {
        player.Score().AdjustScore(-1);

        if (!playedwarningsound) {
            short countscore = 0;
            for (short k = 0; k < score_cnt; k++)
                countscore += score[k]->score;

            if (countscore <= 2)
                playwarningsound();
        }

        if (player.Score().score <= 0) {
            RemoveTeam(player.getTeamID());
            return player_kill_removed;
        }
    }

    return player_kill_normal;
}

void CGM_Boss_MiniGame::playerextraguy(CPlayer &player, short iType)
{
    if (!gameover)
        player.Score().AdjustScore(iType);
}

bool CGM_Boss_MiniGame::SetWinner(CPlayer * player)
{
    winningteam = player->getTeamID();
    gameover = true;

    RemovePlayersButTeam(winningteam);

    for (short iScore = 0; iScore < score_cnt; iScore++) {
        if (winningteam == iScore)
            continue;

        score[iScore]->SetScore(0);
    }

    SetupScoreBoard(false);

    if (game_values.music) {
        ifsoundonstop(rm->sfx_invinciblemusic);
        ifsoundonstop(rm->sfx_timewarning);
        ifsoundonstop(rm->sfx_slowdownmusic);

        rm->backgroundmusic[1].play(true, false);
    }

    //game_values.noexit = true;

    /*
    if (iBossType == 0)
        objectcontainer[0].add(new PU_SledgeHammerPowerup(&rm->spr_sledgehammerpowerup, 304, -32, 1, 0, 30, 30, 1, 1));
    else if (iBossType == 1)
        objectcontainer[0].add(new PU_BombPowerup(&rm->spr_bombpowerup, 304, -32, 1, 0, 30, 30, 1, 1));
    else if (iBossType == 2)
        objectcontainer[0].add(new PU_PodoboPowerup(&rm->spr_podobopowerup, 304, -32, 1, 0, 30, 30, 1, 1));
    */

    return true;
}

void CGM_Boss_MiniGame::SetBossType(short bosstype)
{
    iBossType = bosstype;
}


//Bonus Mode (not really a game mode, but involves using the map so we need a mode to play)
CGM_Bonus::CGM_Bonus() : CGameMode()
{
    gamemode = game_mode_bonus;
    SetupModeStrings("Bonus", "", 0);
}

void CGM_Bonus::init()
{
    CGameMode::init();

    //Unlock the chests
    game_values.noexit = true;

    //Will cause the flow to skip the scoreboard screen and go straight back to the world map
    game_values.worldskipscoreboard = true;

    //Add number of treasure chests to the bonus house
    tsTourStop = game_values.tourstops[game_values.tourstopcurrent];
    short iNumBonuses = tsTourStop->iNumBonuses;

    bool fChestUsed[MAX_BONUS_CHESTS];
    short iChestOrder[MAX_BONUS_CHESTS];
    short iNumChests = 0;

    if (tsTourStop->iBonusType == 0) {
        for (short iChest = 0; iChest < iNumBonuses; iChest++)
            iChestOrder[iNumChests++] = iChest;
    } else {
        for (short iChest = 0; iChest < iNumBonuses; iChest++)
            fChestUsed[iChest] = false;

        for (short iChest = 0; iChest < iNumBonuses; iChest++) {
            short iRandChest = RANDOM_INT(iNumBonuses);

            while (fChestUsed[iRandChest]) {
                if (++iRandChest >= iNumBonuses)
                    iRandChest = 0;
            }

            fChestUsed[iRandChest] = true;
            iChestOrder[iNumChests++] = iRandChest;
        }
    }

    float dSpacing = (384.0f - (float)(iNumBonuses * 64)) / (float)(iNumBonuses + 1);

    float dx = 128.0f + dSpacing;

    //float dx = 288.0f - (dSpacing * (float)(iNumBonuses - 1) / 2.0f);
    for (short iChest = 0; iChest < iNumBonuses; iChest++) {
        objectcontainer[0].add(new MO_BonusHouseChest(&rm->spr_worldbonushouse, (short)dx, 384, tsTourStop->wsbBonuses[iChestOrder[iChest]].iBonus));
        dx += dSpacing + 64.0f;
    }
}


void CGM_Bonus::draw_background()
{
    //Draw Toad
    rm->spr_worldbonushouse.draw(544, 256, list_players[0]->leftX() > 544 ? 224 : 192, 0, 32, 64);

    //Draw Bonus House Title
    rm->menu_plain_field.draw(0, 0, 0, 0, smw->ScreenWidth/2, 32);
    rm->menu_plain_field.draw(smw->ScreenWidth/2, 0, 192, 0, smw->ScreenWidth/2, 32);
    rm->game_font_large.drawCentered(smw->ScreenWidth/2, 5, tsTourStop->szName);

    //Draw Bonus House Text
    if (tsTourStop->iBonusTextLines > 0) {
        rm->spr_worldbonushouse.draw(128, 128, 0, 64, 384, 128);

        for (short iTextLine = 0; iTextLine < tsTourStop->iBonusTextLines; iTextLine++)
            rm->game_font_large.drawChopCentered(smw->ScreenWidth/2, 132 + 24 * iTextLine, 372, tsTourStop->szBonusText[iTextLine]);
    }
}


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

short CGM_Pipe_MiniGame::playerkilledplayer(CPlayer &player, CPlayer &other, killstyle style)
{
    //other.Score().AdjustScore(-2);
    return player_kill_normal;
}

short CGM_Pipe_MiniGame::playerkilledself(CPlayer &player, killstyle style)
{
    //player.Score().AdjustScore(-2);
    return player_kill_normal;
}

void CGM_Pipe_MiniGame::playerextraguy(CPlayer &player, short iType)
{
    if (!gameover) {
        player.Score().AdjustScore(iType);
        CheckWinner(&player);
    }
}

short CGM_Pipe_MiniGame::CheckWinner(CPlayer * player)
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

    return player_kill_normal;
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

/*
TODO
1) Create new box object that breaks on contact and either gives a coin/bonus/penalty
2) Create levels approprate for distributing boxes onto
3) Add level hazards like thwomps, podobos, fireballs
4) Add hurry up that kicks in after 3 minutes of play that adds more coins or lowers bar to win
*/

//Boxes Bonus Mini Game (used in world mode)
//Try to collect all coins from boxes and players
CGM_Boxes_MiniGame::CGM_Boxes_MiniGame() : CGameMode()
{
    goal = 10;
    gamemode = game_mode_boxes_minigame;

    SetupModeStrings("Boxes Minigame", "Lives", 5);
};

void CGM_Boxes_MiniGame::init()
{
    CGameMode::init();

    fReverseScoring = false;

    for (short iScore = 0; iScore < score_cnt; iScore++) {
        score[iScore]->SetScore(goal);
        score[iScore]->subscore[0] = 0;
    }
}


void CGM_Boxes_MiniGame::think()
{
    if (gameover) {
        displayplayertext();
        return;
    }
}

short CGM_Boxes_MiniGame::playerkilledplayer(CPlayer &inflictor, CPlayer &other, killstyle style)
{
    if (gameover)
        return player_kill_normal;

    ReleaseCoin(other);

    other.Score().AdjustScore(-1);

    if (!playedwarningsound) {
        short countscore = 0;
        for (short k = 0; k < score_cnt; k++) {
            if (&inflictor.Score() == score[k])
                continue;

            countscore += score[k]->score;
        }

        if (countscore <= 2) {
            playwarningsound();
        }
    }

    if (other.Score().score <= 0) {
        ReleaseAllCoinsFromTeam(other);
        RemoveTeam(other.getTeamID());
        return player_kill_removed;
    }

    return player_kill_normal;
}

short CGM_Boxes_MiniGame::playerkilledself(CPlayer &player, killstyle style)
{
    if (gameover)
        return player_kill_normal;

    ReleaseCoin(player);

    player.Score().AdjustScore(-1);

    if (!playedwarningsound) {
        short countscore = 0;
        bool playwarning = false;
        for (short j = 0; j < score_cnt; j++) {
            for (short k = 0; k < score_cnt; k++) {
                if (j == k)
                    continue;

                countscore += score[k]->score;
            }

            if (countscore <= 2) {
                playwarning = true;
                break;
            }

            countscore = 0;
        }

        if (playwarning)
            playwarningsound();
    }

    if (player.Score().score <= 0) {
        ReleaseAllCoinsFromTeam(player);
        RemoveTeam(player.getTeamID());
        return player_kill_removed;
    }

    return player_kill_normal;
}

void CGM_Boxes_MiniGame::playerextraguy(CPlayer &player, short iType)
{
    if (gameover)
        return;

    player.Score().AdjustScore(iType);
}

short CGM_Boxes_MiniGame::CheckWinner(CPlayer * player)
{
    if (player->Score().subscore[0] >= 5) {
        player->Score().subscore[0] = 5;

        winningteam = player->getTeamID();
        gameover = true;

        RemovePlayersButTeam(winningteam);
        SetupScoreBoard(false);
        ShowScoreBoard();
    } else if (player->Score().subscore[0] >= 4 && !playedwarningsound) {
        playwarningsound();
    }

    return player_kill_normal;
}

void CGM_Boxes_MiniGame::ReleaseCoin(CPlayer &player)
{
    if (player.Score().subscore[0] > 0) {
        player.Score().subscore[0]--;

        short ix = player.centerX() - 16;
        short iy = player.centerY() - 16;

        float vel = 7.0f + (float)RANDOM_INT(9) / 2.0f;
        float angle = -(float)RANDOM_INT(314) / 100.0f;
        float velx = vel * cos(angle);
        float vely = vel * sin(angle);

        ifSoundOnPlay(rm->sfx_coin);

        objectcontainer[1].add(new MO_Coin(&rm->spr_coin, velx, vely, ix, iy, 2, -1, 2, 30, false));
    }
}

void CGM_Boxes_MiniGame::ReleaseAllCoinsFromTeam(CPlayer &player)
{
    while (player.Score().subscore[0] > 0)
        ReleaseCoin(player);
}
