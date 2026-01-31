#include "Tag.h"

#include "eyecandy.h"
#include "GameValues.h"
#include "player.h"
#include "ResourceManager.h"
#include "Score.h"

extern CScore *score[4];
extern short score_cnt;

extern CEyecandyContainer eyecandy[3];

extern CGameValues game_values;
extern CResourceManager* rm;


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

    setTagged(nullptr);
}

void CGM_Tag::setTagged(CPlayer* player)
{
    m_tagged = player;
}

void CGM_Tag::think()
{
    if (gameover) {
        displayplayertext();
    } else {
        if (!m_tagged) {
            m_tagged = GetHighestScorePlayer(!fReverseScoring);
        }

        static short counter = 0;

        if (m_tagged->isready()) {
            if (++counter >= game_values.pointspeed) {
                counter = 0;

                if (fReverseScoring)
                    m_tagged->Score().AdjustScore(1);
                else
                    m_tagged->Score().AdjustScore(-1);
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

        if (m_tagged->Score().score <= 0) {
            RemoveTeam(m_tagged->getTeamID());
            m_tagged = NULL;
        }
    }
}

PlayerKillType CGM_Tag::playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style)
{
    if (&inflictor == m_tagged) {
        m_tagged = &other;
        inflictor.Shield().reset();
        eyecandy[2].emplace<EC_GravText>(&rm->game_font_large, other.centerX(), other.bottomY(), "Tagged!", -VELJUMP*1.5);
        eyecandy[2].emplace<EC_SingleAnimation>(&rm->spr_fireballexplosion, other.centerX() - 16, other.centerY() - 16, 3, 8);
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
