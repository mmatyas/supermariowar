#include "CO_ThrowBlock.h"

#include "eyecandy.h"
#include "GameValues.h"
#include "player.h"
#include "ResourceManager.h"
#include "objects/carriable/CO_ThrowBox.h"

extern SpotlightManager spotlightManager;
extern CEyecandyContainer eyecandy[3];
extern CGameValues game_values;
extern CResourceManager* rm;

//------------------------------------------------------------------------------
// class throwable block projectile
//------------------------------------------------------------------------------
// State 1: Moving
// State 2: Holding
CO_ThrowBlock::CO_ThrowBlock(gfxSprite* nspr, short x, short y, short type)
    : MO_CarriedObject(nspr, x, y, 4, 2, 30, 30, 1, 1)
{
    state = 2;
    ih = 32;
    movingObjectType = movingobject_throwblock;
    iPlayerID = -1;
    iTeamID = -1;

    fDieOnBounce = type != 2;
    fDieOnPlayerCollision = type == 0;

    iType = type;

    iDeathTime = 0;
    iBounceCounter = 0;
    iNoOwnerKillTime = 0;

    iOwnerRightOffset = 14;
    iOwnerLeftOffset = -22;
    iOwnerUpOffset = 32;

    frozen = false;
    frozentimer = 0;
    frozenvelocity = 0.0f;
    frozenanimationspeed = 2;

    sSpotlight = NULL;
}

bool CO_ThrowBlock::collide(CPlayer* player)
{
    if (frozen) {
        ShatterDie();
        return false;
    }

    if (player->fOldY + PH <= iy && player->iy + PH >= iy)
        return HitTop(player);
    else
        return HitOther(player);
}

bool CO_ThrowBlock::HitTop(CPlayer* player)
{
    if (player->isInvincible() || player->shyguy) {
        Die();
    } else {
        if (state == 1) {  // moving
            return KillPlayer(player);
        } else if (state == 2) {  // Holding
            if (player != owner) {
                if (owner)
                    owner->carriedItem = NULL;

                Kick();

                player->setYi(iy - PH - 1);
                player->bouncejump();
                player->collisions.checktop(*player);
                player->platform = NULL;
            }
        }
    }

    return false;
}

bool CO_ThrowBlock::HitOther(CPlayer* player)
{
    if (state == 1) {  // Moving
        short flipx = 0;

        if (player->ix + PW < 320 && ix > 320)
            flipx = App::screenWidth;
        else if (ix + iw < 320 && player->ix > 320)
            flipx = -App::screenWidth;

        if (iNoOwnerKillTime == 0 || player->globalID != iPlayerID || (player->ix + flipx > ix + (iw >> 1) && velx > 0.0f) || (player->ix + flipx <= ix - (iw >> 1) && velx < 0.0f)) {
            return KillPlayer(player);
        }
    } else if (state == 2) {  // Holding
        if (player != owner) {
            iPlayerID = owner->globalID;
            iTeamID = owner->teamID;
            return KillPlayer(player);
        }
    }

    return false;
}

bool CO_ThrowBlock::KillPlayer(CPlayer* player)
{
    if (player->isInvincible() || player->shyguy) {
        Die();
        return false;
    }

    if (player->isShielded())
        return false;

    CheckAndDie();

    // Find the player that shot this shell so we can attribute a kill
    PlayerKilledPlayer(iPlayerID, player, PlayerDeathStyle::Jump, KillStyle::ThrowBlock, false, false);
    return true;
}

void CO_ThrowBlock::collide(IO_MovingObject* object)
{
    if (object->isDead())
        return;

    removeifprojectile(object, false, false);

    MovingObjectType type = object->getMovingObjectType();

    if (type == movingobject_throwblock) {
        CO_ThrowBlock* block = (CO_ThrowBlock*)object;

        Die();
        block->Die();
    } else if (type == movingobject_throwbox) {
        CO_ThrowBox* box = (CO_ThrowBox*)object;

        Die();
        box->Die();
    } else if (type == movingobject_explosion) {
        Die();
    } else if (type == movingobject_iceblast) {
        frozenvelocity = velx;
        velx = 0.0f;
        animationspeed = 0;

        frozen = true;
        frozentimer = 300;

        eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, ix - collisionOffsetX, iy - collisionOffsetY, 3, 8));
    }
}

void CO_ThrowBlock::update()
{
    if (iNoOwnerKillTime > 0)
        iNoOwnerKillTime--;

    if (frozen) {
        if (--frozentimer <= 0) {
            frozentimer = 0;
            frozen = false;

            velx = frozenvelocity;
            animationspeed = frozenanimationspeed;

            eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, ix - collisionOffsetX, iy - collisionOffsetY, 3, 8));
        }
    } else if (game_values.blueblockttl > 0 && ++iDeathTime >= game_values.blueblockttl) {
        eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, ix, iy, 3, 8));
        dead = true;

        if (owner) {
            owner->carriedItem = NULL;
            owner = NULL;
        }

        return;
    }

    if (owner) {
        MoveToOwner();
        inair = true;
    } else {
        if (iBounceCounter > 0)
            iBounceCounter--;

        fOldX = fx;
        fOldY = fy;

        collision_detection_map();
    }

    animate();

    if (game_values.spotlights && state == 1) {
        if (!sSpotlight) {
            sSpotlight = spotlightManager.AddSpotlight(ix - collisionOffsetX + (iw >> 1), iy - collisionOffsetY + (ih >> 1), 3);
        }

        if (sSpotlight) {
            sSpotlight->UpdatePosition(ix - collisionOffsetX + (iw >> 1), iy - collisionOffsetY + (ih >> 1));
        }
    }
}

void CO_ThrowBlock::draw()
{
    if (owner && owner->iswarping())
        spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, iType << 5, iw, ih, owner->GetWarpState(), owner->GetWarpPlane());
    else
        spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, iType << 5, iw, ih);

    if (frozen) {
        rm->spr_iceblock.draw(ix - collisionOffsetX, iy - collisionOffsetY, 0, 0, 32, 32);
    } else if (fSmoking) {
        eyecandy[0].add(new EC_SingleAnimation(&rm->spr_burnup, ix - collisionOffsetX + (iw >> 1) - 16, iy - collisionOffsetY + (ih >> 1) - 16, 5, 3));
    }
}

void CO_ThrowBlock::Drop()
{
    Kick();
}

void CO_ThrowBlock::Kick()
{
    /*
    if (superkick)
    {
        vel = 12.0f;
        fSmoking = true;
        ifSoundOnPlay(rm->sfx_cannon);
    }
    */

    iDeathTime = 0;

    float fVel, fPlayerBonusVel = owner->velx / 2.0f;
    if (owner->isFacingRight()) {
        fVel = 6.5f;
        if (fPlayerBonusVel > 0.0f)
            fVel += fPlayerBonusVel;

        // if (fVel >= 9.0f)
        //	fSmoking = true;
    } else {
        fVel = -6.5f;
        if (fPlayerBonusVel < 0.0f)
            fVel += fPlayerBonusVel;

        // if (fVel <= -9.0f)
        //	fSmoking = true;
    }

    velx = fVel;
    vely = 0.0f;

    iPlayerID = owner->globalID;
    iTeamID = owner->teamID;

    owner = NULL;
    iNoOwnerKillTime = 30;

    state = 1;

    if (collision_detection_checksides())
        Die();
    else
        ifSoundOnPlay(rm->sfx_kicksound);
}

void CO_ThrowBlock::CheckAndDie()
{
    if (fDieOnPlayerCollision)
        Die();
}

void CO_ThrowBlock::Die()
{
    if (frozen) {
        ShatterDie();
        return;
    }

    if (dead)
        return;

    eyecandy[2].add(new EC_FallingObject(&rm->spr_brokenblueblock, ix, iy, -1.5f, -7.0f, 6, 2, 0, iType << 4, 16, 16));
    eyecandy[2].add(new EC_FallingObject(&rm->spr_brokenblueblock, ix + 16, iy, 1.5f, -7.0f, 6, 2, 0, iType << 4, 16, 16));
    eyecandy[2].add(new EC_FallingObject(&rm->spr_brokenblueblock, ix, iy + 16, -1.5f, -4.0f, 6, 2, 0, iType << 4, 16, 16));
    eyecandy[2].add(new EC_FallingObject(&rm->spr_brokenblueblock, ix + 16, iy + 16, 1.5f, -4.0f, 6, 2, 0, iType << 4, 16, 16));

    DieHelper();
}

void CO_ThrowBlock::ShatterDie()
{
    if (dead)
        return;

    eyecandy[2].add(new EC_FallingObject(&rm->spr_brokeniceblock, ix, iy, -1.5f, -7.0f, 4, 2, 0, 0, 16, 16));
    eyecandy[2].add(new EC_FallingObject(&rm->spr_brokeniceblock, ix + 16, iy, 1.5f, -7.0f, 4, 2, 0, 0, 16, 16));
    eyecandy[2].add(new EC_FallingObject(&rm->spr_brokeniceblock, ix, iy + 16, -1.5f, -4.0f, 4, 2, 0, 0, 16, 16));
    eyecandy[2].add(new EC_FallingObject(&rm->spr_brokeniceblock, ix + 16, iy + 16, 1.5f, -4.0f, 4, 2, 0, 0, 16, 16));

    game_values.unlocksecret2part2++;

    DieHelper();
}

void CO_ThrowBlock::DieHelper()
{
    dead = true;
    ifSoundOnPlay(rm->sfx_breakblock);

    if (owner) {
        owner->carriedItem = NULL;
        owner = NULL;
    }
}

void CO_ThrowBlock::SideBounce(bool fRightSide)
{
    if (fDieOnBounce) {
        Die();
    } else if (state == 1) {
        if (iBounceCounter == 0) {
            eyecandy[2].add(new EC_SingleAnimation(&rm->spr_shellbounce, ix + (velx > 0 ? 0 : collisionWidth) - 21, iy + (collisionHeight >> 1) - 20, 4, 4));
            ifSoundOnPlay(rm->sfx_bump);

            iBounceCounter = 7;  // Allow bounce stars to show on each bounce on a 2x wide pit
        }
    }
}
