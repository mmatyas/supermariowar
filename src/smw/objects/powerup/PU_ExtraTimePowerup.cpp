#include "PU_ExtraTimePowerup.h"

#include "GameMode.h"
#include "GameValues.h"
#include "ResourceManager.h"
#include "gamemodes/TimeLimit.h"

extern CGameValues game_values;
extern CResourceManager* rm;

//------------------------------------------------------------------------------
// class special extra time powerup for timed or star mode
//------------------------------------------------------------------------------
PU_ExtraTimePowerup::PU_ExtraTimePowerup(gfxSprite* nspr, short x, short y)
    : MO_Powerup(nspr, x, y, 1, 0, 30, 30, 1, 1)
{
    velx = 0.0f;
}

bool PU_ExtraTimePowerup::collide(CPlayer* player)
{
    if (state > 0) {
        if (game_values.gamemode->gamemode == game_mode_timelimit || game_values.gamemode->gamemode == game_mode_star) {
            CGM_TimeLimit* timelimitmode = (CGM_TimeLimit*)game_values.gamemode;
            timelimitmode->addtime(timelimitmode->goal / 5);
        }

        ifSoundOnPlay(rm->sfx_collectpowerup);
        dead = true;
    }

    return false;
}
