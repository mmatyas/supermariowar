#include "MO_Spiny.h"

#include "eyecandy.h"
#include "GameMode.h"
#include "GameValues.h"
#include "ObjectContainer.h"
#include "player.h"
#include "ResourceManager.h"
#include "objects/carriable/CO_Shell.h"

extern CObjectContainer objectcontainer[3];
extern CEyecandyContainer eyecandy[3];
extern CGameValues game_values;
extern CResourceManager* rm;

//------------------------------------------------------------------------------
// class spiny
//------------------------------------------------------------------------------
MO_Spiny::MO_Spiny(gfxSprite* nspr, bool moveToRight)
    : MO_WalkingEnemy(nspr, 2, 8, 30, 20, 1, 11, 0, moveToRight ? 0 : 32, 32, 32, moveToRight, true, false, true)
{
    movingObjectType = movingobject_spiny;
    iSpawnIconOffset = 176;
    killStyle = KillStyle::Spiny;
}

void MO_Spiny::update()
{
    if (velx < 0.0f)
        animationOffsetY = 32;
    else
        animationOffsetY = 0;

    MO_WalkingEnemy::update();
}

bool MO_Spiny::hittop(CPlayer* player)
{
    // Kill player here
    if (player->isready() && !player->isShielded() && !player->isInvincible() && !player->kuriboshoe.is_on())
        return player->KillPlayerMapHazard(false, KillStyle::Environment, false) != PlayerKillType::NonKill;

    if (player->kuriboshoe.is_on()) {
        player->setYi(iy - PH - 1);
        player->bouncejump();
        player->collisions.checktop(*player);
        player->platform = NULL;

        dead = true;

        player->AddKillerAward(NULL, KillStyle::Spiny);

        if (game_values.gamemode->gamemode == game_mode_stomp && !game_values.gamemode->gameover)
            player->Score().AdjustScore(1);

        DropShell(false, false);

        ifSoundOnPlay(rm->sfx_mip);
    }

    return false;
}

void MO_Spiny::Die()
{
    if (frozen) {
        ShatterDie();
        return;
    }

    dead = true;
    eyecandy[2].add(new EC_FallingObject(&rm->spr_shelldead, ix, iy, 0.0f, -VELJUMP / 2.0f, 1, 0, 64, 0, 32, 32));
}

void MO_Spiny::DropShell(bool fBounce, bool fFlip)
{
    // Give the shell a state 2 so it is already spawned but sitting
    CO_Shell* shell = new CO_Shell(ShellType::Spiny, ix - 1, iy, false, true, false, false);
    shell->nospawn(iy, fBounce);

    if (fFlip)
        shell->Flip();

    objectcontainer[1].add(shell);
}
