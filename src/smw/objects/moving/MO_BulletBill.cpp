#include "MO_BulletBill.h"

#include "eyecandy.h"
#include "Game.h"
#include "GameValues.h"
#include "gfx.h"
#include "ObjectContainer.h"
#include "player.h"
#include "ResourceManager.h"
#include "objects/carriable/CO_ThrowBox.h"
#include "objects/moving/MO_Explosion.h"

extern CEyecandyContainer eyecandy[3];
extern CObjectContainer objectcontainer[3];
extern CGameValues game_values;
extern CResourceManager* rm;

extern short LookupTeamID(short id);


//------------------------------------------------------------------------------
// class bulletbill
//------------------------------------------------------------------------------
MO_BulletBill::MO_BulletBill(gfxSprite* nspr, gfxSprite* nsprdead, short x, short y, float nspeed, short playerID, bool isspawned)
    : IO_MovingObject(nspr, x, y, 4, 8, 30, 28, 1, 2)
{
    spr_dead = nsprdead;

    velx = nspeed;
    vely = 0.0f;

    movingObjectType = movingobject_bulletbill;
    state = 1;

    fIsSpawned = isspawned;

    ih = 32;
    iw = 32;

    inair = true;

    if (fIsSpawned) {
        iPlayerID = -1;
        iColorID = 0;
        iTeamID = -1;

        animationspeed = 0;

        if (velx < 0.0f) {
            iHiddenDirection = 1;
            iHiddenPlane = ix;
        } else {
            iHiddenDirection = 3;
            iHiddenPlane = ix + TILESIZE;
        }
    } else {
        if (velx < 0.0f)
            setXi(App::screenWidth + iw);
        else
            setXi(-iw);

        iPlayerID = playerID;
        iColorID = game_values.colorids[iPlayerID];
        iTeamID = LookupTeamID(iPlayerID);
    }

    iColorOffsetY = 64 * iColorID;
    SetDirectionOffset();

    fObjectCollidesWithMap = false;
}

void MO_BulletBill::update()
{
    setXf(fx + velx);
    // setYf(fy + vely);

    animate();

    if ((velx < 0.0f && ix < -iw) || (velx > 0.0f && ix > App::screenWidth))
        dead = true;
}

void MO_BulletBill::draw()
{
    if (fIsSpawned)
        spr->draw(ix, iy, drawframe, iColorOffsetY + iDirectionOffsetY, iw, ih, iHiddenDirection, iHiddenPlane);
    else
        spr->draw(ix, iy, drawframe, iColorOffsetY + iDirectionOffsetY, iw, ih);
}

// For preview drawing
void MO_BulletBill::draw(short iOffsetX, short iOffsetY)
{
    if (fIsSpawned)
        gfx_drawpreview(spr->getSurface(), (ix >> 1) + iOffsetX, (iy >> 1) + iOffsetY, drawframe >> 1, (iColorOffsetY + iDirectionOffsetY) >> 1, iw >> 1, ih >> 1, iOffsetX, iOffsetY, 320, 240, false, iHiddenDirection, (iHiddenPlane >> 1) + iOffsetX);
    else
        gfx_drawpreview(spr->getSurface(), (ix >> 1) + iOffsetX, (iy >> 1) + iOffsetY, drawframe >> 1, (iColorOffsetY + iDirectionOffsetY) >> 1, iw >> 1, ih >> 1, iOffsetX, iOffsetY, 320, 240, false);
}

bool MO_BulletBill::collide(CPlayer* player)
{
    if (dead)
        return false;

    // if the bullet bill is off the screen, don't wrap it to collide
    if ((ix < 0 && velx < 0.0f && player->ix > ix + iw && player->ix + PW < App::screenWidth) || (ix + iw >= App::screenWidth && velx > 0.0f && player->ix + PW < ix && player->ix >= 0)) {
        return false;
    }

    if (player->isInvincible() || player->shyguy) {
        player->AddKillerAward(NULL, KillStyle::BulletBill);
        ifSoundOnPlay(rm->sfx_kicksound);

        Die();
    } else {
        if (player->fOldY + PH <= iy && player->iy + PH >= iy)
            return hittop(player);
        else
            return hitother(player);
    }

    return false;
}


bool MO_BulletBill::hittop(CPlayer* player)
{
    player->setYi(iy - PH - 1);
    player->bouncejump();
    player->collisions.checktop(*player);
    player->platform = NULL;

    player->AddKillerAward(NULL, KillStyle::BulletBill);

    ifSoundOnPlay(rm->sfx_mip);

    Die();

    return false;
}

bool MO_BulletBill::hitother(CPlayer* player)
{
    if (player->isShielded() || player->globalID == iPlayerID)
        return false;

    if (game_values.teamcollision != TeamCollisionStyle::On && iTeamID == player->teamID)
        return false;

    // Find the player that owns this bullet bill so we can attribute a kill
    PlayerKilledPlayer(iPlayerID, player, PlayerDeathStyle::Jump, KillStyle::BulletBill, false, false);

    return true;
}

void MO_BulletBill::collide(IO_MovingObject* object)
{
    removeifprojectile(object, true, false);

    MovingObjectType type = object->getMovingObjectType();

    if (type == movingobject_bulletbill) {
        MO_BulletBill* bulletbill = (MO_BulletBill*)object;

        // Same team bullet bills don't kill each other
        if (bulletbill->iTeamID == iTeamID)
            return;

        bulletbill->dead = true;
        dead = true;

        short iOffsetX = 0;
        if (ix + iw < bulletbill->ix)
            iOffsetX = App::screenWidth;
        else if (bulletbill->ix + bulletbill->iw < ix)
            iOffsetX = -App::screenWidth;

        short iCenterX = ((ix + iOffsetX - bulletbill->ix) >> 1) + (bulletbill->ix + (bulletbill->iw >> 1));
        short iCenterY = ((iy - bulletbill->iy) >> 1) + (bulletbill->iy + (bulletbill->ih >> 1));

        objectcontainer[2].add(new MO_Explosion(&rm->spr_explosion, iCenterX - 96, iCenterY - 64, 2, 4, -1, -1, KillStyle::BulletBill));
        ifSoundOnPlay(rm->sfx_bobombsound);
    } else if (type == movingobject_shell || type == movingobject_throwblock || type == movingobject_throwbox || type == movingobject_attackzone || type == movingobject_explosion) {
        // Don't kill things with shells that are sitting still
        if (type == movingobject_shell && object->state == 2)
            return;

        // Don't kill things with boxesx that aren't moving fast enough
        if (type == movingobject_throwbox && !((CO_ThrowBox*)object)->HasKillVelocity())
            return;

        if (type != movingobject_explosion) {
            object->Die();
        }

        ifSoundOnPlay(rm->sfx_kicksound);
        Die();
    }
}

void MO_BulletBill::Die()
{
    dead = true;
    eyecandy[2].add(new EC_FallingObject(spr_dead, ix, iy, 0.0f, -VELJUMP / 2.0f, 1, 0, velx > 0 ? 0 : 32, iColorID * 32, 32, 32));
}

void MO_BulletBill::SetDirectionOffset()
{
    iDirectionOffsetY = velx < 0 ? 0 : 32;
}
