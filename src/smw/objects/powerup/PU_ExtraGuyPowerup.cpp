#include "PU_ExtraGuyPowerup.h"

#include "eyecandy.h"
#include "GameMode.h"
#include "GameValues.h"
#include "player.h"
#include "ResourceManager.h"

extern CEyecandyContainer eyecandy[3];

extern CGameValues game_values;
extern CResourceManager* rm;

//------------------------------------------------------------------------------
// class 1up powerup
//------------------------------------------------------------------------------
PU_ExtraGuyPowerup::PU_ExtraGuyPowerup(gfxSprite* nspr, short x, short y, short iNumSpr, bool moveToRight, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY, short type)
    : MO_Powerup(nspr, x, y, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY)
    , iType(type)
{
    if (moveToRight)
        velx = 1.0f + (float)type;
    else
        velx = -1.0f - (float)type;
}

bool PU_ExtraGuyPowerup::collide(CPlayer* player)
{
    if (state > 0) {
        game_values.gamemode->playerextraguy(*player, iType);
        ifSoundOnPlay(rm->sfx_extraguysound);

        eyecandy[2].add(new EC_FloatingObject(&rm->spr_extralife, player->ix + HALFPW - 19, player->iy - 16, 0.0f, -1.5f, 62, player->colorID * 38, (iType == 5 ? 3 : iType - 1) * 16, 38, 16));

        dead = true;
    }

    return false;
}
