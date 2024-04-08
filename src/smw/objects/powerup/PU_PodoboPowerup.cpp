#include "PU_PodoboPowerup.h"

#include "player.h"

//------------------------------------------------------------------------------
// class podobo powerup
//------------------------------------------------------------------------------
PU_PodoboPowerup::PU_PodoboPowerup(gfxSprite* nspr, short x, short y, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY)
    : MO_Powerup(nspr, x, y, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY)
{
    velx = 0.0f;
}

bool PU_PodoboPowerup::collide(CPlayer* player)
{
    if (state > 0) {
        dead = true;
        player->SetStoredPowerup(22);
    }

    return false;
}
