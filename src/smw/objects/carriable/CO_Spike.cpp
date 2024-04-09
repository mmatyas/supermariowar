#include "CO_Spike.h"

#include "player.h"

//------------------------------------------------------------------------------
// class spike
//------------------------------------------------------------------------------
CO_Spike::CO_Spike(gfxSprite* nspr, short iX, short iY)
    : CO_Spring(nspr, iX, iY, false)
{
    iw = 32;
    ih = 32;

    movingObjectType = movingobject_carried;
}

void CO_Spike::hittop(CPlayer* player)
{
    if (player->isready() && !player->isShielded() && !player->isInvincible() && !player->kuriboshoe.is_on() && !player->shyguy)
        player->KillPlayerMapHazard(false, KillStyle::Environment, false);
}
