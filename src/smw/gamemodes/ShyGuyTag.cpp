#include "ShyGuyTag.h"

#include "eyecandy.h"
#include "GameValues.h"
#include "player.h"
#include "ResourceManager.h"
#include "Score.h"

extern CScore *score[4];
extern short score_cnt;

extern CEyecandyContainer eyecandy[3];

extern CPlayer* list_players[4];
extern short list_players_cnt;

extern CResourceManager* rm;
extern CGameValues game_values;

extern short CountAliveTeams(short * lastteam);


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
