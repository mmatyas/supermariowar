#include "PU_Tanooki.h"

#include "player.h"
#include "ResourceManager.h"

extern CResourceManager* rm;

//------------------------------------------------------------------------------
// tanooki suit
//------------------------------------------------------------------------------
PU_Tanooki::PU_Tanooki(Vec2s pos)
    : MO_Powerup(&rm->spr_tanooki, pos, 1, 0, 30, 30, 1, 1)
{
}

bool PU_Tanooki ::collide(CPlayer* player)
{
    if (state > 0) {
        player->SetPowerup(9);
        dead = true;
    }

    return false;
}
