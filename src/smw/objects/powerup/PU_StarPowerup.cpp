#include "PU_StarPowerup.h"

#include "player.h"

//------------------------------------------------------------------------------
// class star powerup
//------------------------------------------------------------------------------
PU_StarPowerup::PU_StarPowerup(gfxSprite* nspr, short x, short y, short iNumSpr, bool moveToRight, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY)
    : MO_Powerup(nspr, x, y, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY)
{
    if (moveToRight)
        velx = 2.0f;
    else
        velx = -2.0f;

    bounce = -VELPOWERUPBOUNCE;
}

bool PU_StarPowerup::collide(CPlayer* player)
{
    if (state > 0) {
        dead = true;

        if (!player->isInvincible() && !player->shyguy) {
            player->makeinvincible();
        } else {
            player->SetStoredPowerup(6);
        }
    }

    return false;
}
