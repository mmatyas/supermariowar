#include "PU_LeafPowerup.h"

#include "player.h"

//------------------------------------------------------------------------------
// class leaf powerup
//------------------------------------------------------------------------------
PU_LeafPowerup::PU_LeafPowerup(gfxSprite* nspr, short x, short y, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY)
    : PU_FeatherPowerup(nspr, x, y, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY)
{
}

bool PU_LeafPowerup::collide(CPlayer* player)
{
    if (state > 0) {
        player->SetPowerup(7);
        dead = true;
    }

    return false;
}
