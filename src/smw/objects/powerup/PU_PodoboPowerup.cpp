#include "PU_PodoboPowerup.h"

#include "player.h"

//------------------------------------------------------------------------------
// class podobo powerup
//------------------------------------------------------------------------------
PU_PodoboPowerup::PU_PodoboPowerup(gfxSprite* nspr, Vec2s pos, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY)
    : MO_Powerup(nspr, pos, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY)
{
    velx = 0.0f;
}

bool PU_PodoboPowerup::collide(CPlayer* player)
{
    if (state > 0) {
        dead = true;
        player->SetStoredPowerup(PowerupType::Podobo);
    }

    return false;
}
