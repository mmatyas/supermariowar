#include "Chicken.h"

#include "eyecandy.h"
#include "GameValues.h"
#include "player.h"
#include "ResourceManager.h"

extern CEyecandyContainer eyecandy[3];
extern CGameValues game_values;
extern CResourceManager* rm;


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

void CGM_Chicken::init()  //called once when the game is started
{
    CGameMode::init();
    clearChicken();
};

void CGM_Chicken::clearChicken()
{
    m_chicken = nullptr;
}

void CGM_Chicken::think()
{
    if (gameover) {
        displayplayertext();
        return;
    }

    if (m_chicken) {
        if ( m_chicken->getVelX() > VELMOVING_CHICKEN )
            m_chicken->velx = VELMOVING_CHICKEN;
        else if (m_chicken->getVelX() < -VELMOVING_CHICKEN)
            m_chicken->velx = -VELMOVING_CHICKEN;

        static short counter = 0;

        if (m_chicken->isready() && !m_chicken->IsTanookiStatue()) {
            if (++counter >= game_values.pointspeed) {
                counter = 0;
                m_chicken->Score().AdjustScore(1);
                CheckWinner(m_chicken);
            }
        }
    }
}

void CGM_Chicken::draw_foreground()
{
    //Draw the chicken indicator around the chicken
    if (game_values.gamemodesettings.chicken.usetarget && !gameover && m_chicken) {
        if (m_chicken->iswarping())
            rm->spr_chicken.draw(m_chicken->leftX() - PWOFFSET - 16, m_chicken->topY() - PHOFFSET - 16, 0, 0, 64, 64, m_chicken->GetWarpState(), m_chicken->GetWarpPlane());
        else if (m_chicken->isready())
            rm->spr_chicken.draw(m_chicken->centerX() - 32, m_chicken->centerY() - 32);
    }
}


PlayerKillType CGM_Chicken::playerkilledplayer(CPlayer &inflictor, CPlayer &other, KillStyle style)
{
    if (!m_chicken || &other == m_chicken) {
        m_chicken = &inflictor;
        eyecandy[2].add(new EC_GravText(&rm->game_font_large, inflictor.centerX(), inflictor.bottomY(), "Chicken!", -VELJUMP*1.5));
        //eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, inflictor.centerX() - 16, inflictor.centerY() - 16, 3, 8));
        eyecandy[2].add(new EC_SingleAnimation(&rm->spr_poof, inflictor.centerX() - 24, inflictor.centerY() - 24, 4, 5));
        ifSoundOnPlay(rm->sfx_transform);

        if (&other == m_chicken)
            other.SetCorpseType(1); //flag to use chicken corpse sprite
    } else if (&inflictor == m_chicken) {
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

    if (m_chicken == &player) {
        player.SetCorpseType(1); //flag to use chocobo corpse sprite

        if (!gameover)
            m_chicken = NULL;
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
