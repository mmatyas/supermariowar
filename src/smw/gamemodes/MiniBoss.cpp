#include "MiniBoss.h"

#include "GameValues.h"
#include "ObjectContainer.h"
#include "player.h"
#include "RandomNumberGenerator.h"
#include "ResourceManager.h"
#include "objects/moving/MO_FrenzyCard.h"
#include "objects/moving/MO_Podobo.h"
#include "objects/moving/MO_SledgeBrother.h"
#include "objects/walkingenemy/WE_Koopa.h"

extern std::vector<CPlayer*> players;
extern short score_cnt;
extern CScore *score[4];
extern CObjectContainer objectcontainer[3];
extern CGameValues game_values;
extern CResourceManager* rm;


//Boss Mode
//Person to score fatal hit to boss wins!
CGM_Boss_MiniGame::CGM_Boss_MiniGame() : CGameMode()
{
    gamemode = game_mode_boss_minigame;
    SetupModeStrings("Boss", "Lives", 5);
}

void CGM_Boss_MiniGame::init()
{
    CGameMode::init();

    fReverseScoring = false;

    enemytimer = (short)(RANDOM_INT(120) + 120);
    poweruptimer = 120;

    for (short iScore = 0; iScore < score_cnt; iScore++)
        score[iScore]->SetScore(goal);

    objectcontainer[0].add(new MO_SledgeBrother(&rm->spr_sledgebrothers, (iBossType == Boss::Hammer ? 256 : (iBossType == Boss::Bomb ? 256 : App::screenWidth/2)), iBossType));
}


void CGM_Boss_MiniGame::think()
{
    if (!gameover && players.size() == 0) {
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
        if (iBossType == Boss::Hammer) {
            //Randomly spawn koopas
            if (--enemytimer <= 0) {
                objectcontainer[0].add(new MO_Koopa(&rm->spr_koopa, RANDOM_BOOL(), false, false, true));
                enemytimer = (short)RANDOM_INT(120) + 120;  //Spawn koopas slowly
            }
        } else if (iBossType == Boss::Bomb) {

        } else if (iBossType == Boss::Fire) {
            //Only create podobos if the difficulty is moderate or greater
            if (--enemytimer <= 0 && game_values.gamemodesettings.boss.difficulty >= 2) {
                objectcontainer[2].add(new MO_Podobo(&rm->spr_podobo, {(short)RANDOM_INT(App::screenWidth * 0.95f), App::screenHeight}, -(float(RANDOM_INT(9)) / 2.0f) - 9.0f, -1, -1, -1, false));
                enemytimer = (short)(RANDOM_INT(80) + 60);
            }

            if (--poweruptimer <= 0) {
                poweruptimer = (short)(RANDOM_INT(80) + 60);

                if (objectcontainer[1].countTypes(object_frenzycard) < players.size()) {
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
            rm->game_font_large.drawCentered(App::screenWidth/2, 96, "You Failed To Defeat");

            switch (iBossType) {
                case Boss::Hammer:
                    rm->game_font_large.drawCentered(App::screenWidth/2, 118, "The Mighty Sledge Brother");
                    break;
                case Boss::Bomb:
                    rm->game_font_large.drawCentered(App::screenWidth/2, 118, "The Mighty Bomb Brother");
                    break;
                case Boss::Fire:
                    rm->game_font_large.drawCentered(App::screenWidth/2, 118, "The Mighty Flame Brother");
                    break;
            }
        }
    }
}

PlayerKillType CGM_Boss_MiniGame::playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style)
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
            return PlayerKillType::Removed;
        }
    }

    return PlayerKillType::Normal;
}

PlayerKillType CGM_Boss_MiniGame::playerkilledself(CPlayer &player, KillStyle style)
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
            return PlayerKillType::Removed;
        }
    }

    return PlayerKillType::Normal;
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
    ShowScoreBoard();

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

void CGM_Boss_MiniGame::SetBossType(Boss bosstype)
{
    iBossType = bosstype;
}
