#include "PU_PWingsPowerup.h"

#include "player.h"

//------------------------------------------------------------------------------
// pwings
//------------------------------------------------------------------------------
PU_PWingsPowerup::PU_PWingsPowerup(gfxSprite* nspr, Vec2s pos)
    : MO_Powerup(nspr, pos, 1, 0, 30, 30, 1, 1)
{}

bool PU_PWingsPowerup::collide(CPlayer* player)
{
    player->SetPowerup(8);
    dead = true;
    return false;
}
