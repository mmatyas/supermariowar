#include "PU_PoisonPowerup.h"

#include "player.h"

//------------------------------------------------------------------------------
// class poison powerup
//------------------------------------------------------------------------------
PU_PoisonPowerup::PU_PoisonPowerup(gfxSprite* nspr, short x, short y, short iNumSpr, bool moveToRight, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY)
    : MO_Powerup(nspr, x, y, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY)
{
    if (moveToRight)
        velx = 2.0f;
    else
        velx = -2.0f;

    movingObjectType = movingobject_poisonpowerup;
}

bool PU_PoisonPowerup::collide(CPlayer* player)
{
    if (state > 0) {
        if (player->isInvincible() || player->shyguy) {
            dead = true;
            return false;
        }

        if (player->isShielded())
            return false;

        dead = true;

        return player->KillPlayerMapHazard(false, KillStyle::PoisonMushroom, false) != PlayerKillType::NonKill;
    }

    return false;
}
