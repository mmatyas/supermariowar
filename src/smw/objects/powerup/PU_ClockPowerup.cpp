#include "PU_ClockPowerup.h"

#include "player.h"

//------------------------------------------------------------------------------
// class clock powerup
//------------------------------------------------------------------------------
PU_ClockPowerup::PU_ClockPowerup(gfxSprite* nspr, Vec2s pos, short iNumSpr, bool, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY)
    : MO_Powerup(nspr, pos, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY)
{
    velx = 0.0f;
}

bool PU_ClockPowerup::collide(CPlayer* player)
{
    if (state > 0) {
        player->SetStoredPowerup(PowerupType::Clock);
        dead = true;
    }

    return false;
}
