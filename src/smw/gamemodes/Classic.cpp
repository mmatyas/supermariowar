#include "Classic.h"

#include "GameValues.h"
#include "player.h"
#include "ResourceManager.h"
#include "Score.h"

extern CScore *score[4];
extern short score_cnt;
extern CGameValues game_values;
extern CResourceManager* rm;


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


PlayerKillType CGM_Classic::playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style)
{
    if (gameover)
        return PlayerKillType::Normal;

    //If we are playing classic "sumo" mode, then only score hazard kills
    if (game_values.gamemode->gamemode != game_mode_classic || game_values.gamemodesettings.classic.scoring == 0 || style == KillStyle::Push) {
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
                return PlayerKillType::Removed;
            }
        }
    }

    if (game_values.gamemode->gamemode == game_mode_classic && game_values.gamemodesettings.classic.style == DeathStyle::Shield) {
        ifSoundOnPlay(rm->sfx_powerdown);
        other.Shield().reset();
        return PlayerKillType::NonKill;
    }

    return PlayerKillType::Normal;
}

PlayerKillType CGM_Classic::playerkilledself(CPlayer &player, KillStyle style)
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
                return PlayerKillType::Removed;
            }
        }

        if (game_values.gamemode->gamemode == game_mode_classic && game_values.gamemodesettings.classic.style == DeathStyle::Shield) {
            ifSoundOnPlay(rm->sfx_powerdown);
            player.Shield().reset();
            return PlayerKillType::NonKill;
        }
    }

    return PlayerKillType::Normal;
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
