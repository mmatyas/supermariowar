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
    szModeName = szMode;
    szGoalName = szGoal;

    for (short iMode = 0; iMode < GAMEMODE_NUM_OPTIONS; iMode++) {
        if (iMode == GAMEMODE_NUM_OPTIONS - 1) {
            modeOptions[iMode].iValue = -1;
            modeOptions[iMode].szName = "Unlimited";
        } else {
            modeOptions[iMode].iValue = (iMode + 1) * iGoalSpacing;
            modeOptions[iMode].szName = std::to_string(modeOptions[iMode].iValue);
        }
    }
}

PlayerKillType CGameMode::playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style)
{
    //Penalize killing your team mates
    if (!gameover) {
        if (inflictor.getTeamID() == other.getTeamID())
            inflictor.Score().AdjustScore(-1);
        else
            inflictor.Score().AdjustScore(1);
    }

    return PlayerKillType::Normal;
};

PlayerKillType CGameMode::playerkilledself(CPlayer &player, KillStyle style)
{
    if (player.IsBobomb())
        player.SetCorpseType(2); //flag to use bobomb corpse sprite

    return PlayerKillType::Normal;
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

PlayerKillType CGM_Frag::playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style)
{
    if (gameover)
        return PlayerKillType::Normal;

    //Don't score if "sumo" style scoring is turned on
    if (game_values.gamemode->gamemode != game_mode_frag || game_values.gamemodesettings.frag.scoring == 0 || style == KillStyle::Push) {
        //Penalize killing your team mates
        if (inflictor.getTeamID() == other.getTeamID())
            inflictor.Score().AdjustScore(-1);
        else
            inflictor.Score().AdjustScore(1);
    }

    PlayerKillType iRet = CheckWinner(&inflictor);

    if (game_values.gamemode->gamemode == game_mode_frag && game_values.gamemodesettings.frag.style == 1) {
        ifSoundOnPlay(rm->sfx_powerdown);
        other.Shield().reset();
        return PlayerKillType::NonKill;
    }

    return iRet;
}

PlayerKillType CGM_Frag::playerkilledself(CPlayer &player, KillStyle style)
{
    CGameMode::playerkilledself(player, style);

    if (!gameover) {
        player.Score().AdjustScore(-1);

        if (game_values.gamemode->gamemode == game_mode_frag && game_values.gamemodesettings.frag.style == 1) {
            ifSoundOnPlay(rm->sfx_powerdown);
            player.Shield().reset();
            return PlayerKillType::NonKill;
        }
    }

    return PlayerKillType::Normal;
}

void CGM_Frag::playerextraguy(CPlayer &player, short iType)
{
    if (!gameover) {
        player.Score().AdjustScore(iType);
        CheckWinner(&player);
    }
}

PlayerKillType CGM_Frag::CheckWinner(CPlayer * player)
{
    if (goal > -1) {
        if (player->Score().score >= goal) {
            player->Score().SetScore(goal);
            winningteam = player->getTeamID();
            gameover = true;

            RemovePlayersButTeam(winningteam);
            SetupScoreBoard(false);
            ShowScoreBoard();

            return PlayerKillType::Removed;
        } else if (player->Score().score >= goal - 2 && !playedwarningsound) {
            playwarningsound();
        }
    }

    return PlayerKillType::Normal;
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


PlayerKillType CGM_Chicken::playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style)
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

    return PlayerKillType::Normal;
}

PlayerKillType CGM_Chicken::playerkilledself(CPlayer &player, KillStyle style)
{
    CGameMode::playerkilledself(player, style);

    if (chicken == &player) {
        player.SetCorpseType(1); //flag to use chocobo corpse sprite

        if (!gameover)
            chicken = NULL;
    }

    return PlayerKillType::Normal;
}

void CGM_Chicken::playerextraguy(CPlayer &player, short iType)
{
    if (!gameover) {
        player.Score().AdjustScore(10 * iType);
        CheckWinner(&player);
    }
}

PlayerKillType CGM_Chicken::CheckWinner(CPlayer * player)
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

PlayerKillType CGM_Tag::playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style)
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
            return PlayerKillType::Normal;
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
            return PlayerKillType::Removed;
        }
    }

    return PlayerKillType::Normal;
}

PlayerKillType CGM_Tag::playerkilledself(CPlayer &player, KillStyle style)
{
    CGameMode::playerkilledself(player, style);

    if (!gameover) {
        if (fReverseScoring) {
            player.Score().AdjustScore(5);
            return PlayerKillType::Normal;
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
            return PlayerKillType::Removed;
        }
    }

    return PlayerKillType::Normal;
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

PlayerKillType CGM_ShyGuyTag::playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style)
{
    if (gameover || other.shyguy)
        return PlayerKillType::Normal;

    if (CountShyGuys() == 0 || game_values.gamemodesettings.shyguytag.tagtransfer != 0) {
        SetShyGuy(other.getTeamID());
    }

    return PlayerKillType::Normal;
}

PlayerKillType CGM_ShyGuyTag::playerkilledself(CPlayer &player, KillStyle style)
{
    CGameMode::playerkilledself(player, style);

    if (!gameover && game_values.gamemodesettings.shyguytag.tagonsuicide) {
        SetShyGuy(player.getTeamID());
    }

    return PlayerKillType::Normal;
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

PlayerKillType CGM_ShyGuyTag::CheckWinner(CPlayer * player)
{
    if (gameover || goal == -1)
        return PlayerKillType::Normal;

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

    return PlayerKillType::Normal;
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


PlayerKillType CGM_Coins::playerkilledplayer(CPlayer &player, CPlayer &other, KillStyle style)
{
    if (gameover)
        return PlayerKillType::Normal;

    if (game_values.gamemodesettings.coins.penalty)
        other.Score().AdjustScore(-1);

    return PlayerKillType::Normal;
}

PlayerKillType CGM_Coins::playerkilledself(CPlayer &player, KillStyle style)
{
    if (gameover)
        return PlayerKillType::Normal;

    if (game_values.gamemodesettings.coins.penalty)
        player.Score().AdjustScore(-1);

    return PlayerKillType::Normal;
}

void CGM_Coins::playerextraguy(CPlayer &player, short iType)
{
    if (!gameover) {
        player.Score().AdjustScore(iType << 1);
        CheckWinner(&player);
    }
}

PlayerKillType CGM_Coins::CheckWinner(CPlayer * player)
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

    return PlayerKillType::Normal;
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

//Fireball:
//Frag limit death match, but powerup cards appear randomly
CGM_Frenzy::CGM_Frenzy() : CGM_Frag()
{
    gamemode = game_mode_frenzy;
    szModeName = "Frenzy";
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
CGM_Survival::CGM_Survival()
    : CGM_Classic()
{
    goal = 20;
    gamemode = game_mode_survival;
    szModeName = "Survival";
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
