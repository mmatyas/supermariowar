#include "PU_BoomerangPowerup.h"

#include "player.h"

//------------------------------------------------------------------------------
// class boomerang powerup
//------------------------------------------------------------------------------
PU_BoomerangPowerup::PU_BoomerangPowerup(gfxSprite* nspr, Vec2s pos, short iNumSpr, bool, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY)
    : MO_Powerup(nspr, pos, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY)
{
    velx = 0.0f;
}

bool PU_BoomerangPowerup::collide(CPlayer* player)
{
    if (state > 0) {
        player->SetPowerup(4);
        dead = true;
    }

    return false;
}
