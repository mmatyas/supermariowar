#include "PU_ExtraHeartPowerup.h"

#include "GameMode.h"
#include "GameValues.h"
#include "player.h"
#include "ResourceManager.h"

extern CGameValues game_values;
extern CResourceManager* rm;

//------------------------------------------------------------------------------
// class special heart powerup for health mode
//------------------------------------------------------------------------------
PU_ExtraHeartPowerup::PU_ExtraHeartPowerup(gfxSprite *nspr, short x, short y) :
    MO_Powerup(nspr, x, y, 1, 0, 30, 30, 1, 1)
{
    velx = 0.0f;
}

bool PU_ExtraHeartPowerup::collide(CPlayer * player)
{
    if (state > 0) {
        if (game_values.gamemode->gamemode == game_mode_health) {
            if (player->Score().subscore[1] < game_values.gamemodesettings.health.maxlife)
                player->Score().subscore[1]++;

            if (player->Score().subscore[0] < game_values.gamemodesettings.health.maxlife)
                player->Score().subscore[0]++;
        }

        ifSoundOnPlay(rm->sfx_collectpowerup);
        dead = true;
    }

    return false;
}
