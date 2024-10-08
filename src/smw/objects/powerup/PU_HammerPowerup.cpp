#include "PU_HammerPowerup.h"

#include "player.h"

//------------------------------------------------------------------------------
// class hammer powerup
//------------------------------------------------------------------------------
PU_HammerPowerup::PU_HammerPowerup(gfxSprite* nspr, Vec2s pos, short iNumSpr, bool, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY)
    : MO_Powerup(nspr, pos, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY)
{
    velx = 0.0f;
}

bool PU_HammerPowerup::collide(CPlayer* player)
{
    if (state > 0) {
        player->SetPowerup(2);
        dead = true;
    }

    return false;
}
