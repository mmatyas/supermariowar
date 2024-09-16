#include "MO_CheepCheep.h"

#include "eyecandy.h"
#include "Game.h"
#include "GameMode.h"
#include "GameValues.h"
#include "player.h"
#include "RandomNumberGenerator.h"
#include "ResourceManager.h"
#include "objects/carriable/CO_Shell.h"
#include "objects/carriable/CO_ThrowBox.h"

extern CPlayer* GetPlayerFromGlobalID(short iGlobalID);

extern CEyecandyContainer eyecandy[3];
extern CGameValues game_values;
extern CResourceManager* rm;

//------------------------------------------------------------------------------
// class cheep cheep
//------------------------------------------------------------------------------
MO_CheepCheep::MO_CheepCheep(gfxSprite* nspr)
    : IO_MovingObject(nspr, {0, App::screenHeight}, 2, 8, 30, 28, 1, 3)
{
    ih = 32;
    setXi((short)(RANDOM_INT(608)));

    velx = 0.0f;
    while (velx == 0.0f)
        velx = float(RANDOM_INT(19) - 9) / 2.0f;

    // Cheep cheep up velocity is between 9.0 and 13.0 in 0.5 increments
    vely = -(float(RANDOM_INT(11)) / 2.0f) - 9.0f;

    movingObjectType = movingobject_cheepcheep;
    state = 1;

    iColorOffsetY = (short)(RANDOM_INT(3)) * 64;

    if (velx > 0.0f)
        iColorOffsetY += 32;

    fObjectCollidesWithMap = false;
    frozen = false;
}

void MO_CheepCheep::update()
{
    fOldX = fx;
    fOldY = fy;

    setXf(fx + velx);
    setYf(fy + vely);

    // Cheep cheep gravitation
    vely += 0.2f;

    animate();

    // Remove if cheep cheep has fallen below bottom of screen
    if (vely > 0.0f && iy > App::screenHeight)
        dead = true;
}

void MO_CheepCheep::draw()
{
    spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, iColorOffsetY, iw, ih);

    if (frozen) {
        rm->spr_iceblock.draw(ix - collisionOffsetX, iy - collisionOffsetY, 0, 0, 32, 32);
    }
}

bool MO_CheepCheep::collide(CPlayer* player)
{
    if (player->isInvincible() || frozen) {
        player->AddKillerAward(NULL, KillStyle::CheepCheep);

        if (game_values.gamemode->gamemode == game_mode_stomp && !game_values.gamemode->gameover)
            player->Score().AdjustScore(1);

        if (frozen) {
            ShatterDie();
        } else {
            ifSoundOnPlay(rm->sfx_kicksound);
            Die();
        }
    } else {
        if (player->fOldY + PH <= fOldY && player->iy + PH >= iy)
            return hittop(player);
        else
            return hitother(player);
    }

    return false;
}


bool MO_CheepCheep::hittop(CPlayer* player)
{
    player->setYi(iy - PH - 1);
    player->bouncejump();
    player->collisions.checktop(*player);
    player->platform = NULL;

    player->AddKillerAward(NULL, KillStyle::CheepCheep);

    if (game_values.gamemode->gamemode == game_mode_stomp && !game_values.gamemode->gameover)
        player->Score().AdjustScore(1);

    ifSoundOnPlay(rm->sfx_mip);

    Die();

    return false;
}

bool MO_CheepCheep::hitother(CPlayer* player)
{
    if (player->isShielded())
        return false;

    return player->KillPlayerMapHazard(false, KillStyle::Environment, false) != PlayerKillType::NonKill;
}

void MO_CheepCheep::collide(IO_MovingObject* object)
{
    if (!object->isDead()) {
        removeifprojectile(object, false, false);

        MovingObjectType type = object->getMovingObjectType();

        if (type == movingobject_fireball || type == movingobject_hammer || type == movingobject_boomerang || type == movingobject_shell || type == movingobject_throwblock || type == movingobject_throwbox || type == movingobject_bulletbill || type == movingobject_podobo || type == movingobject_attackzone || type == movingobject_explosion || type == movingobject_sledgehammer) {
            // Don't kill goombas with non-moving shells
            if (type == movingobject_shell && object->GetState() == 2)
                return;

            if (type == movingobject_throwbox && !((CO_ThrowBox*)object)->HasKillVelocity())
                return;

            if (game_values.gamemode->gamemode == game_mode_stomp && !game_values.gamemode->gameover) {
                // Find the player that shot this projectile so we can attribute a kill
                CPlayer* killer = GetPlayerFromGlobalID(object->iPlayerID);

                if (killer) {
                    killer->AddKillerAward(NULL, KillStyle::CheepCheep);
                    killer->Score().AdjustScore(1);

                    if (type == movingobject_shell)
                        ((CO_Shell*)object)->AddMovingKill(killer);
                }
            }

            if (frozen) {
                ShatterDie();
            } else {
                ifSoundOnPlay(rm->sfx_kicksound);
                Die();
            }

            if (type == movingobject_shell || type == movingobject_throwblock) {
                object->CheckAndDie();
            } else if (type == movingobject_bulletbill || type == movingobject_attackzone || type == movingobject_throwbox) {
                object->Die();
            }
        } else if (type == movingobject_iceblast) {
            animationspeed = 0;
            frozen = true;

            eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, ix - collisionOffsetX, iy - collisionOffsetY, 3, 8));
        }
    }
}

void MO_CheepCheep::Die()
{
    if (frozen) {
        ShatterDie();
        return;
    }

    dead = true;
    eyecandy[2].add(new EC_FallingObject(&rm->spr_cheepcheepdead, ix, iy, 0.0f, -VELJUMP / 2.0f, 1, 0, 0, iColorOffsetY, 32, 32));
}

void MO_CheepCheep::ShatterDie()
{
    ifSoundOnPlay(rm->sfx_breakblock);
    dead = true;

    short iBrokenIceX = ix - collisionOffsetX, iBrokenIceY = iy - collisionOffsetY;
    eyecandy[2].add(new EC_FallingObject(&rm->spr_brokeniceblock, iBrokenIceX, iBrokenIceY, -1.5f, -7.0f, 4, 2, 0, 0, 16, 16));
    eyecandy[2].add(new EC_FallingObject(&rm->spr_brokeniceblock, iBrokenIceX + 16, iBrokenIceY, 1.5f, -7.0f, 4, 2, 0, 0, 16, 16));
    eyecandy[2].add(new EC_FallingObject(&rm->spr_brokeniceblock, iBrokenIceX, iBrokenIceY + 16, -1.5f, -4.0f, 4, 2, 0, 0, 16, 16));
    eyecandy[2].add(new EC_FallingObject(&rm->spr_brokeniceblock, iBrokenIceX + 16, iBrokenIceY + 16, 1.5f, -4.0f, 4, 2, 0, 0, 16, 16));

    game_values.unlocksecret2part2++;
}

///////////////////////DEBUG!  REMOVE THIS WHEN DONE/////////////////////////////
#ifdef _DEBUG
extern bool fDebugShowBossSettings;
#endif
///////////////////////DEBUG!  REMOVE THIS WHEN DONE/////////////////////////////
