#include "GameMode.h"

#include "eyecandy.h"
#include "GameValues.h"
#include "net.h"
#include "ObjectContainer.h"
#include "player.h"
#include "RandomNumberGenerator.h"
#include "ResourceManager.h"
#include "Score.h"
#include "objects/powerup/PU_TreasureChestBonus.h"

extern CScore *score[4];
extern short score_cnt;

extern CObjectContainer objectcontainer[3];
extern CEyecandyContainer eyecandy[3];

extern CPlayer* list_players[4];
extern short list_players_cnt;

extern CResourceManager* rm;
extern CGameValues game_values;


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
    if (game_values.matchtype == MatchType::World && game_values.gamemode->winningteam > -1 && game_values.gamemode->gamemode != game_mode_bonus) {
        TourStop * tourStop = game_values.tourstops[game_values.tourstopcurrent];
        short iNumBonuses = tourStop->iNumBonuses;

        for (short iBonus = 0; iBonus < iNumBonuses; iBonus++) {
            if (tourStop->wsbBonuses[iBonus].iWinnerPlace == 0) {
                short iBonusType = tourStop->wsbBonuses[iBonus].iBonus;
                objectcontainer[0].add(new PU_TreasureChestBonus(&rm->spr_bonuschest, 1, 0, 30, 30, 1, 1, iBonusType));
                game_values.flags.noexittimer = 0;
                game_values.flags.noexit = false;
            }
        }
    }
}

void ShowScoreBoard()
{
    game_values.flags.showscoreboard = true;

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
        return game_values.flags.teamdeadcounter == score_cnt - 1;

    //kill all players on the dead team
    short iAnnouncementColor = -1;
    for (short iPlayer = 0; iPlayer < list_players_cnt; iPlayer++) {
        if (list_players[iPlayer]->getTeamID() == teamid) {
            if (iAnnouncementColor == -1)
                iAnnouncementColor = list_players[iPlayer]->getColorID();

            list_players[iPlayer]->state = player_dead;
        }
    }

    score[teamid]->order = game_values.flags.teamdeadcounter++;

    //Announce that a team was removed
    if (game_values.deadteamnotice && game_values.flags.teamdeadcounter < score_cnt - 1) {
        eyecandy[2].add(new EC_Announcement(&rm->game_font_large, &rm->spr_announcementicons, "Team Removed!", iAnnouncementColor, 90, 200));
        ifsoundonandreadyplay(rm->sfx_announcer[iAnnouncementColor + 16]);
    }

    return game_values.flags.teamdeadcounter == score_cnt - 1;
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
