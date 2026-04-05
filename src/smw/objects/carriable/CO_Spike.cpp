#include "CO_Spike.h"

#include "player.h"

//------------------------------------------------------------------------------
// class spike
//------------------------------------------------------------------------------
CO_Spike::CO_Spike(gfxSprite* nspr, Vec2s pos)
    : CO_Spring(nspr, pos, false)
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
