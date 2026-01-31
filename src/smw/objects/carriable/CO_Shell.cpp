#include "CO_Shell.h"

#include "eyecandy.h"
#include "GameValues.h"
#include "map.h"
#include "player.h"
#include "RandomNumberGenerator.h"
#include "ResourceManager.h"
#include "objects/carriable/CO_ThrowBlock.h"
#include "objects/carriable/CO_ThrowBox.h"

extern CPlayer* GetPlayerFromGlobalID(short iGlobalID);

extern SpotlightManager spotlightManager;
extern CMap* g_map;
extern CEyecandyContainer eyecandy[3];
extern CGameValues game_values;
extern CResourceManager* rm;

//------------------------------------------------------------------------------
// class shell projectile
//------------------------------------------------------------------------------
// state 0: Shell is being spawned
// state 1: Shell is moving
// state 2: Shell is waiting to be picked up
// state 3: Shell is being held
CO_Shell::CO_Shell(ShellType type, Vec2s pos, bool dieOnMovingPlayerCollision, bool dieOnHoldingPlayerCollision, bool dieOnFire, bool killBouncePlayer)
    : MO_CarriedObject(&rm->spr_shell, pos, 4, 4, 30, 20, 1, 11, 0, static_cast<int>(type) * 32, 32, 32)
{
    iShellType = type;

    state = 0;

    movingObjectType = movingobject_shell;

    iPlayerID = -1;
    iTeamID = -1;

    iIgnoreBounceTimer = 0;
    iBounceCounter = 0;

    fDieOnMovingPlayerCollision = dieOnMovingPlayerCollision;
    fDieOnHoldingPlayerCollision = dieOnHoldingPlayerCollision;
    fDieOnFire = dieOnFire;
    fKillBouncePlayer = killBouncePlayer;

    iDeathTime = 0;

    iDestY = iy - collisionHeight;
    fy = iDestY + 32.0f;
    iColorOffsetY = static_cast<int>(type) * 32;

    iKillCounter = 0;
    iNoOwnerKillTime = 0;

    fFlipped = false;
    iFlippedOffset = 0;

    iOwnerRightOffset = 14;
    iOwnerLeftOffset = -22;
    iOwnerUpOffset = 32;

    frozen = false;
    frozentimer = 0;
    frozenvelocity = 0.0f;
    frozenanimationspeed = 4;

    sSpotlight = NULL;
}

bool CO_Shell::collide(CPlayer* player)
{
    if (player->isInvincible() || player->shyguy || frozen) {
        if (frozen) {
            ShatterDie();
            return false;
        } else {
            if (state == 0 || state == 2) {  // If sitting or spawning then just die
                Die();
                return false;
            } else if (state == 3) {  // if held, but not by us then die
                if (owner != player) {
                    Die();
                    return false;
                }
            } else if (state == 1) {  // If moving, see if it is actually hitting us before we kill it
                short flipx = 0;

                if (player->ix + PW < 320 && ix > 320)
                    flipx = App::screenWidth;
                else if (ix + iw < 320 && player->ix > 320)
                    flipx = -App::screenWidth;

                if ((player->ix + HALFPW + flipx >= ix + (iw >> 1) && velx > 0.0f) || (player->ix + HALFPW + flipx < ix + (iw >> 1) && velx < 0.0f)) {
                    Die();
                    return false;
                }
            }
        }
    }

    if (player->tanookisuit.notStatue()) {
        if (player->fOldY + PH <= iy && player->iy + PH >= iy)
            return HitTop(player);
        else
            return HitOther(player);
    }

    return false;
}

bool CO_Shell::HitTop(CPlayer* player)
{
    if (player->isInvincible() || player->kuriboshoe.is_on() || player->shyguy) {
        Die();
        fSmoking = false;
        return false;
    }

    if (fKillBouncePlayer && !fFlipped) {
        KillPlayer(player);
    } else if (state == 2) {  // Sitting
        owner = player;
        Kick();
        fSmoking = false;
        if (player->ix + HALFPW < ix + (iw >> 1))
            velx = 5.0f;
        else
            velx = -5.0f;

        iIgnoreBounceTimer = 10;
    } else if (state == 1 && iIgnoreBounceTimer == 0) {  // Moving
        Stop();

        player->setYi(iy - PH - 1);
        player->bouncejump();
        player->collisions.checktop(*player);
        player->platform = NULL;
    } else if (state == 3) {  // Holding
        if (player != owner && (game_values.teamcollision == TeamCollisionStyle::On || player->teamID != owner->teamID)) {
            if (owner)
                owner->carriedItem = NULL;

            Kick();
            fSmoking = false;

            player->setYi(iy - PH - 1);
            player->bouncejump();
            player->collisions.checktop(*player);
            player->platform = NULL;
        }
    }

    return false;
}

bool CO_Shell::HitOther(CPlayer* player)
{
    if (state == 2) {  // Sitting
        if (owner == NULL && player->isready()) {
            if (player->AcceptItem(this)) {
                owner = player;
                iPlayerID = owner->globalID;
                iTeamID = owner->teamID;
                state = 3;
            } else {
                short flipx = 0;

                if (player->ix + PW < 320 && ix > 320)
                    flipx = App::screenWidth;
                else if (ix + iw < 320 && player->ix > 320)
                    flipx = -App::screenWidth;

                owner = player;
                Kick();
                if (player->ix + HALFPW + flipx < ix + (iw >> 1))
                    velx = 5.0f;
                else
                    velx = -5.0f;
            }
        }
    } else if (state == 1) {  // Moving
        short flipx = 0;

        if (player->ix + PW < 320 && ix > 320)
            flipx = App::screenWidth;
        else if (ix + iw < 320 && player->ix > 320)
            flipx = -App::screenWidth;

        if (iNoOwnerKillTime == 0 || player->globalID != iPlayerID || (player->ix + HALFPW + flipx >= ix + (iw >> 1) && velx > 0.0f) || (player->ix + HALFPW + flipx < ix + (iw >> 1) && velx < 0.0f))
            return KillPlayer(player);
    } else if (state == 3) {  // Holding
        if (player != owner && (game_values.teamcollision == TeamCollisionStyle::On || player->teamID != owner->teamID)) {
            iPlayerID = owner->globalID;
            iTeamID = owner->teamID;
            return KillPlayer(player);
        }
    }

    return false;
}

void CO_Shell::UsedAsStoredPowerup(CPlayer* player)
{
    owner = player;
    MoveToOwner();

    if (player->AcceptItem(this))
        state = 3;
    else
        Kick();
}

bool CO_Shell::KillPlayer(CPlayer* player)
{
    if (player->isShielded() || player->isInvincible() || player->shyguy)
        return false;

    CheckAndDie();

    // Find the player that shot this shell so we can attribute a kill
    PlayerKilledPlayer(iPlayerID, player, PlayerDeathStyle::Jump, KillStyle::Shell, false, false);

    CPlayer* killer = GetPlayerFromGlobalID(iPlayerID);
    if (killer && iPlayerID != player->globalID) {
        AddMovingKill(killer);
    }

    return true;
}

void CO_Shell::AddMovingKill(CPlayer* killer)
{
    if (state == 1 && game_values.awardstyle != AwardStyle::None) {  // If the shell is moving, the keep track of how many people we kill in a row with it
        if (++iKillCounter > 1)
            killer->AddKillsInRowInAirAward();
    }
}

void CO_Shell::collide(IO_MovingObject* object)
{
    if (object->isDead())
        return;

    // Don't allow shells to die if they are warping
    if (owner && owner->iswarping())
        return;

    removeifprojectile(object, false, false);

    MovingObjectType type = object->getMovingObjectType();

    if (type == movingobject_shell) {
        CO_Shell* shell = (CO_Shell*)object;

        // Green shells should die on collision, other shells should not,
        // except if they also hit a non dead on collision shell

        if (frozen || shell->frozen) {
            Die();
            shell->Die();
        }
        if (shell->fSmoking && !fSmoking) {
            Die();
        } else if (!shell->fSmoking && fSmoking) {
            shell->Die();
        } else {
            if (fDieOnMovingPlayerCollision || state == 2 || (!shell->fDieOnMovingPlayerCollision && shell->state != 2))
                Die();

            if (shell->fDieOnMovingPlayerCollision || shell->state == 2 || (!fDieOnMovingPlayerCollision && state != 2))
                shell->Die();
        }
    } else if (type == movingobject_throwblock) {
        CO_ThrowBlock* block = (CO_ThrowBlock*)object;

        Die();
        block->Die();
    } else if (type == movingobject_throwbox) {
        CO_ThrowBox* box = (CO_ThrowBox*)object;

        Die();
        box->Die();
    } else if (type == movingobject_explosion || type == movingobject_sledgehammer || type == movingobject_superfireball) {
        Die();
    } else if (type == movingobject_fireball || type == movingobject_hammer || type == movingobject_boomerang) {
        if (fDieOnFire)
            Die();
    } else if (type == movingobject_iceblast) {
        frozenvelocity = velx;
        velx = 0.0f;
        animationspeed = 0;

        frozen = true;
        frozentimer = 300;

        eyecandy[2].emplace<EC_SingleAnimation>(&rm->spr_fireballexplosion, ix - collisionOffsetX, iy - collisionOffsetY, 3, 8);
    }
}

void CO_Shell::update()
{
    if (iNoOwnerKillTime > 0)
        iNoOwnerKillTime--;

    if (frozen) {
        if (--frozentimer <= 0) {
            frozentimer = 0;
            frozen = false;

            velx = frozenvelocity;
            animationspeed = frozenanimationspeed;

            eyecandy[2].emplace<EC_SingleAnimation>(&rm->spr_fireballexplosion, ix - collisionOffsetX, iy - collisionOffsetY, 3, 8);
        }
    } else {
        if (state == 1) {
            if (game_values.shellttl > 0 && ++iDeathTime >= game_values.shellttl) {
                eyecandy[2].emplace<EC_SingleAnimation>(&rm->spr_fireballexplosion, ix, iy, 3, 8);
                dead = true;
                ifSoundOnPlay(rm->sfx_kicksound);

                if (owner) {
                    owner->carriedItem = NULL;
                    owner = NULL;
                }

                return;
            }
        } else {
            iDeathTime = 0;
        }
    }

    // Have the powerup grow out of the powerup block
    if (state == 0) {
        setYf(fy - 2.0f);

        if (fy <= iDestY) {
            state = 2;
            vely = GRAVITATION;
            setYf(iDestY);
        }

        return;
    }

    if (iIgnoreBounceTimer > 0)
        iIgnoreBounceTimer--;

    if (iBounceCounter > 0)
        iBounceCounter--;

    if (owner) {
        MoveToOwner();
        inair = true;
    } else {
        IO_MovingObject::update();
    }

    if (game_values.spotlights) {
        if (state == 1) {
            if (!sSpotlight) {
                sSpotlight = spotlightManager.AddSpotlight(ix - collisionOffsetX + (iw >> 1), iy - collisionOffsetY + (ih >> 1), 3);
            }

            if (sSpotlight) {
                sSpotlight->UpdatePosition(ix - collisionOffsetX + (iw >> 1), iy - collisionOffsetY + (ih >> 1));
            }
        } else {
            sSpotlight = NULL;
        }
    }
}

void CO_Shell::draw()
{
    if (state == 0) {
        spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, 0, iColorOffsetY, iw, (short)(ih - fy + iDestY));
    } else if (owner) {
        if (owner->iswarping())
            spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, 0, iColorOffsetY + iFlippedOffset, iw, ih, owner->GetWarpState(), owner->GetWarpPlane());
        else
            spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, 0, iColorOffsetY + iFlippedOffset, iw, ih);
    } else {
        if (state == 2)
            spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, 0, iColorOffsetY + iFlippedOffset, iw, ih);
        else if (state == 1)
            spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, iColorOffsetY + iFlippedOffset, iw, ih);
    }

    if (frozen) {
        rm->spr_iceblock.draw(ix - collisionOffsetX, iy - collisionOffsetY, 0, 0, 32, 32);
    } else if (fSmoking) {
        eyecandy[0].emplace<EC_SingleAnimation>(&rm->spr_burnup, ix - collisionOffsetX + (iw >> 1) - 16, iy - collisionOffsetY + (ih >> 1) - 16, 5, 3);
    }
}

void CO_Shell::Drop()
{
    if (owner) {
        owner->carriedItem = NULL;
        setXi(owner->ix + (owner->isFacingRight() ? PW + 1 : -31));
    }

    if (collision_detection_checksides()) {
        // Move back to where it was before checking sides, then kill it
        setXi(owner->ix + (owner->isFacingRight() ? PW + 1 : -31));
        setYi(owner->iy + PH - 32 + collisionOffsetY);
        Die();
    } else {
        owner = NULL;
        state = 2;
    }
}

void CO_Shell::Kick()
{
    /*
    if (superkick)
    {
        vel = 10.0f;
        fSmoking = true;
        ifSoundOnPlay(rm->sfx_cannon);
    }
    */

    float fVel, fPlayerBonusVel = owner->velx / 2.0f;
    if (owner->isFacingRight()) {
        fVel = 5.0f;
        if (fPlayerBonusVel > 0.0f)
            fVel += fPlayerBonusVel;

        // if (fVel >= 7.5f)
        //	fSmoking = true;
    } else {
        fVel = -5.0f;
        if (fPlayerBonusVel < 0.0f)
            fVel += fPlayerBonusVel;

        // if (fVel <= -7.5f)
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

void CO_Shell::CheckAndDie()
{
    if ((fDieOnMovingPlayerCollision && state == 1) || ((fDieOnHoldingPlayerCollision || fFlipped) && state == 3))
        Die();
    else if (!fDieOnHoldingPlayerCollision && state == 3 && (RANDOM_INT(5)) == 0)
        Die();
}

void CO_Shell::Die()
{
    if (frozen) {
        ShatterDie();
        return;
    }

    eyecandy[2].emplace<EC_FallingObject>(&rm->spr_shelldead, ix, iy, -velx / 4.0f, -VELJUMP / 2.0f, 1, 0, static_cast<int>(iShellType) * 32, 0, 32, 32);
    dead = true;
    ifSoundOnPlay(rm->sfx_kicksound);
    iKillCounter = 0;

    if (owner) {
        owner->carriedItem = NULL;
        owner = NULL;
    }
}

void CO_Shell::ShatterDie()
{
    dead = true;
    ifSoundOnPlay(rm->sfx_breakblock);
    iKillCounter = 0;

    if (owner) {
        owner->carriedItem = NULL;
        owner = NULL;
    }

    short iBrokenIceX = ix - collisionOffsetX, iBrokenIceY = iy - collisionOffsetY;
    eyecandy[2].emplace<EC_FallingObject>(&rm->spr_brokeniceblock, iBrokenIceX, iBrokenIceY, -1.5f, -7.0f, 4, 2, 0, 0, 16, 16);
    eyecandy[2].emplace<EC_FallingObject>(&rm->spr_brokeniceblock, iBrokenIceX + 16, iBrokenIceY, 1.5f, -7.0f, 4, 2, 0, 0, 16, 16);
    eyecandy[2].emplace<EC_FallingObject>(&rm->spr_brokeniceblock, iBrokenIceX, iBrokenIceY + 16, -1.5f, -4.0f, 4, 2, 0, 0, 16, 16);
    eyecandy[2].emplace<EC_FallingObject>(&rm->spr_brokeniceblock, iBrokenIceX + 16, iBrokenIceY + 16, 1.5f, -4.0f, 4, 2, 0, 0, 16, 16);
}

void CO_Shell::SideBounce(bool fRightSide)
{
    if (state == 1) {
        if (iBounceCounter == 0) {
            eyecandy[2].emplace<EC_SingleAnimation>(&rm->spr_shellbounce, ix + (velx > 0 ? 0 : collisionWidth) - 21, iy + (collisionHeight >> 1) - 20, 4, 4);
            ifSoundOnPlay(rm->sfx_bump);

            iBounceCounter = 7;  // Allow bounce stars to show on each bounce on a 2x wide pit
        }
    }
}

void CO_Shell::Flip()
{
    if (frozen) {
        ShatterDie();
        return;
    }

    if (owner) {
        Die();
        return;
    }

    if (!fFlipped) {
        fFlipped = true;
        iFlippedOffset = 128;
    }

    Stop();
    vely = -VELJUMP / 2.0;
}

void CO_Shell::Stop()
{
    owner = NULL;
    velx = 0.0f;
    state = 2;
    fSmoking = false;
    ifSoundOnPlay(rm->sfx_kicksound);
    iKillCounter = 0;
}

void CO_Shell::nospawn(short y, bool fBounce)
{
    state = 2;
    setYi(y);

    if (fBounce)
        vely = -VELJUMP / 2.0;
}
