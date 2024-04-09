#include "MO_Goomba.h"

#include "eyecandy.h"
#include "GameMode.h"
#include "GameValues.h"
#include "player.h"
#include "ResourceManager.h"

extern CEyecandyContainer eyecandy[3];
extern CGameValues game_values;
extern CResourceManager* rm;

//------------------------------------------------------------------------------
// class goomba
//------------------------------------------------------------------------------
MO_Goomba::MO_Goomba(gfxSprite *nspr, bool moveToRight, bool fBouncing) :
    MO_WalkingEnemy(nspr, 2, 8, 30, 20, 1, 11, 0, moveToRight ? 0 : 32, 32, 32, moveToRight, true, fBouncing, true)
{
    movingObjectType = movingobject_goomba;
    iSpawnIconOffset = 64;
    killStyle = KillStyle::Goomba;

    if (fBouncing) {
        iw = 40;
        ih = 48;

        fOldY = fy - ih;

        collisionOffsetX = 5;
        collisionOffsetY = 27;

        animationOffsetY = moveToRight ? 0 : ih;
    }
}

void MO_Goomba::draw()
{
    //if frozen, just draw shell, not entire koopa
    if (frozen) {
        rm->spr_goomba.draw(ix - collisionOffsetX + iw - 32, iy - collisionOffsetY + ih - 32, 0, 0, 32, 32);
        rm->spr_iceblock.draw(ix - collisionOffsetX + iw - 32, iy - collisionOffsetY + ih - 32, 0, 0, 32, 32);
    } else {
        MO_WalkingEnemy::draw();
    }
}

void MO_Goomba::update()
{
    if (velx < 0.0f)
        animationOffsetY = ih;
    else
        animationOffsetY = 0;

    MO_WalkingEnemy::update();
}

bool MO_Goomba::hittop(CPlayer * player)
{
    player->setYi(iy - PH - 1);
    player->bouncejump();
    player->collisions.checktop(*player);
    player->platform = NULL;

    if (game_values.gamemode->gamemode == game_mode_stomp && !game_values.gamemode->gameover)
        player->Score().AdjustScore(1);

    if (fBouncing) {
        fBouncing = false;
        bounce = GRAVITATION;

        if (vely < GRAVITATION)
            vely = GRAVITATION;

        iw = 32;
        ih = 32;

        collisionOffsetX = 1;
        collisionOffsetY = 11;

        animationWidth = 64;
        drawframe = 0;

        animationOffsetY = velx > 0.0f ? 0 : ih;
        spr = &rm->spr_goomba;
    } else {
        dead = true;

        player->AddKillerAward(NULL, killStyle);

        eyecandy[0].add(new EC_Corpse(&rm->spr_goombadead, (float)(ix - collisionOffsetX), (float)(iy + collisionHeight - 32), 0));
    }

    ifSoundOnPlay(rm->sfx_mip);

    return false;
}


void MO_Goomba::Die()
{
    if (frozen) {
        ShatterDie();
        return;
    }

    dead = true;
    eyecandy[2].add(new EC_FallingObject(&rm->spr_goombadeadflying, ix, iy, 0.0f, -VELJUMP / 2.0f, 1, 0, 0, 0, 0, 0));
}

