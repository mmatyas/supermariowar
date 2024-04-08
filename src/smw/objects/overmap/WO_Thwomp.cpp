#include "WO_Thwomp.h"

#include "Game.h"
#include "GameMode.h"
#include "GameValues.h"
#include "player.h"

extern CGameValues game_values;

//------------------------------------------------------------------------------
// class thwomp (for thwomp mode)
//------------------------------------------------------------------------------
OMO_Thwomp::OMO_Thwomp(gfxSprite* nspr, short x, float nspeed)
    : IO_OverMapObject(nspr, x, (short)-nspr->getHeight(), 1, 0)
{
    objectType = object_thwomp;
    vely = nspeed;
}

void OMO_Thwomp::update()
{
    IO_OverMapObject::update();

    if (iy > App::screenHeight - 1)
        dead = true;
}

bool OMO_Thwomp::collide(CPlayer* player)
{
    if (!player->isInvincible() && !player->isShielded() && (player->Score().score > 0 || game_values.gamemode->goal == -1))
        return player->KillPlayerMapHazard(false, KillStyle::Environment, false) != PlayerKillType::NonKill;

    return false;
}
