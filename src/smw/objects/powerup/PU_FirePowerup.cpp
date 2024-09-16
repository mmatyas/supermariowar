#include "PU_FirePowerup.h"

#include "player.h"

//------------------------------------------------------------------------------
// class fire powerup
//------------------------------------------------------------------------------
PU_FirePowerup::PU_FirePowerup(gfxSprite* nspr, Vec2s pos, short iNumSpr, bool, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY)
    : MO_Powerup(nspr, pos, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY)
{
    velx = 0.0f;
}

bool PU_FirePowerup::collide(CPlayer* player)
{
    if (state > 0) {
        player->SetPowerup(1);
        dead = true;
    }

    return false;
}
