#include "PU_BombPowerup.h"

#include "player.h"

//------------------------------------------------------------------------------
// class bomb powerup
//------------------------------------------------------------------------------
PU_BombPowerup::PU_BombPowerup(gfxSprite* nspr, Vec2s pos, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY)
    : MO_Powerup(nspr, pos, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY)
{
    velx = 0.0f;
}

bool PU_BombPowerup::collide(CPlayer* player)
{
    if (state > 0) {
        player->SetPowerup(6);
        dead = true;
    }

    return false;
}
