#include "MO_SpinAttack.h"

#include "GameValues.h"
#include "IO_Block.h"
#include "map.h"
#include "player.h"
#include "ResourceManager.h"
#include "objects/carriable/CO_Shell.h"
#include "objects/carriable/CO_ThrowBlock.h"
#include "objects/carriable/CO_ThrowBox.h"

extern CPlayer* GetPlayerFromGlobalID(short iGlobalID);

extern CMap* g_map;
extern CResourceManager* rm;

//------------------------------------------------------------------------------
// class spin death (spinning cape or tail)
//------------------------------------------------------------------------------
MO_SpinAttack::MO_SpinAttack(short playerId, short teamId, KillStyle style, bool direction, short offsety)
    : MO_AttackZone(playerId, teamId, Vec2s::zero(), {24, 12}, 16, style, true)
{
    fDirection = direction;
    iOffsetY = offsety;

    state = 0;
    objectType = object_moving;
}

bool MO_SpinAttack::collide(CPlayer* player)
{
    if (iTimer > 11)
        return false;

    return MO_AttackZone::collide(player);
}

/*
void MO_SpinAttack::draw()
{
        if (iTimer <= 11 && !dead)
        {
                SDL_Rect r = {ix, iy, collisionWidth, collisionHeight};
                SDL_FillRect(blitdest, &r, 0xff00);
        }
}
*/

void MO_SpinAttack::update()
{
    MO_AttackZone::update();

    if (iTimer <= 11)
        state = 1;

    CPlayer* owner = GetPlayerFromGlobalID(iPlayerID);

    if (owner) {
        // Move to the owner
        setXi(owner->ix - PWOFFSET + (fDirection ? 24 : -16));
        setYi(owner->iy + PH - iOffsetY);

        if (iTimer < 5 || iy + collisionHeight < 0)
            return;

        // Check block collisions
        short iTop = iy / TILESIZE;
        short iBottom = (iy + collisionHeight) / TILESIZE;

        short iLeft;
        if (ix < 0)
            iLeft = (ix + App::screenWidth) / TILESIZE;
        else
            iLeft = ix / TILESIZE;

        short iRight = (ix + collisionWidth) / TILESIZE;

        if (iLeft < 0)
            iLeft += 20;

        if (iLeft >= 20)
            iLeft -= 20;

        if (iRight < 0)
            iRight += 20;

        if (iRight >= 20)
            iRight -= 20;

        IO_Block* topleftblock = NULL;
        IO_Block* toprightblock = NULL;
        IO_Block* bottomleftblock = NULL;
        IO_Block* bottomrightblock = NULL;

        if (iTop >= 0 && iTop < 15) {
            topleftblock = g_map->block(iLeft, iTop);
            toprightblock = g_map->block(iRight, iTop);
        }

        if (iBottom >= 0 && iBottom < 15) {
            bottomleftblock = g_map->block(iLeft, iBottom);
            bottomrightblock = g_map->block(iRight, iBottom);
        }

        bool fHitBlock = false;
        if (topleftblock && !topleftblock->isTransparent() && !topleftblock->isHidden())
            fHitBlock = topleftblock->collide(this, 3);

        if (!fHitBlock && toprightblock && !toprightblock->isTransparent() && !toprightblock->isHidden())
            fHitBlock = toprightblock->collide(this, 1);

        if (!fHitBlock && bottomleftblock && !bottomleftblock->isTransparent() && !bottomleftblock->isHidden())
            fHitBlock = bottomleftblock->collide(this, 3);

        if (!fHitBlock && bottomrightblock && !bottomrightblock->isTransparent() && !bottomrightblock->isHidden())
            fHitBlock = bottomrightblock->collide(this, 1);

        if (fHitBlock)
            dead = true;
    } else {
        dead = true;
    }
}

void MO_SpinAttack::collide(IO_MovingObject* object)
{
    if (dead || iTimer > 11)
        return;

    MovingObjectType type = object->getMovingObjectType();

    if (type == movingobject_shell || type == movingobject_throwblock || type == movingobject_throwbox) {
        if (type == movingobject_shell) {
            CO_Shell* shell = (CO_Shell*)object;

            if (shell->frozen) {
                shell->ShatterDie();
            } else {
                shell->Flip();
                ifSoundOnPlay(rm->sfx_kicksound);
            }

            Die();
        } else if (type == movingobject_throwblock) {
            CO_ThrowBlock* block = (CO_ThrowBlock*)object;

            if (block->frozen) {
                block->ShatterDie();
            } else if (!block->owner || block->owner->globalID != iPlayerID) {
                block->Die();
                ifSoundOnPlay(rm->sfx_kicksound);
                Die();
            }
        } else if (type == movingobject_throwbox) {
            CO_ThrowBox* box = (CO_ThrowBox*)object;

            if (box->frozen) {
                box->ShatterDie();
            } else if (!box->owner || box->owner->globalID != iPlayerID) {
                box->Die();
                ifSoundOnPlay(rm->sfx_kicksound);
                Die();
            }
        }
    }
}
