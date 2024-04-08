#include "PU_ClockPowerup.h"

#include "player.h"

//------------------------------------------------------------------------------
// class clock powerup
//------------------------------------------------------------------------------
PU_ClockPowerup::PU_ClockPowerup(gfxSprite* nspr, short x, short y, short iNumSpr, bool, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY)
    : MO_Powerup(nspr, x, y, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY)
{
    velx = 0.0f;
}

bool PU_ClockPowerup::collide(CPlayer* player)
{
    if (state > 0) {
        player->SetStoredPowerup(7);
        dead = true;
    }

    return false;
}
