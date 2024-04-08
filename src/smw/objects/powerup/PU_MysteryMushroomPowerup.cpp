#include "PU_MysteryMushroomPowerup.h"

#include "eyecandy.h"
#include "GameValues.h"
#include "player.h"
#include "ResourceManager.h"

extern CEyecandyContainer eyecandy[3];
extern CResourceManager* rm;

extern bool SwapPlayers(short iUsingPlayerID);

//------------------------------------------------------------------------------
// class mystery mushroom powerup
//------------------------------------------------------------------------------
PU_MysteryMushroomPowerup::PU_MysteryMushroomPowerup(gfxSprite* nspr, short x, short y, short iNumSpr, bool moveToRight, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY)
    : MO_Powerup(nspr, x, y, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY)
{
    if (moveToRight)
        velx = 2.0f;
    else
        velx = -2.0f;
}

bool PU_MysteryMushroomPowerup::collide(CPlayer* player)
{
    if (state > 0) {
        dead = true;

        if (!SwapPlayers(player->localID)) {
            eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, player->ix + (HALFPW)-16, player->iy + (HALFPH)-16, 3, 8));
            ifSoundOnPlay(rm->sfx_spit);
        }
    }

    return false;
}
