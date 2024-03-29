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
