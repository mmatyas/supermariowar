#include "PlayerOutOfArenaTimer.h"

#include "GameValues.h"
#include "player.h"
#include "PlayerKillTypes.h"
#include "ResourceManager.h"

extern CGameValues game_values;
extern CResourceManager* rm;

void PlayerOutOfArenaTimer::reset()
{
    timer = 0;
    displaytimer = game_values.outofboundstime - 1;
}

void PlayerOutOfArenaTimer::update(CPlayer& player)
{
    if (player.iy < 0) {
        if ((player.iy + PH < -1 || (player.iy + PH <= 1 && player.vely <= 0.8f))
            && game_values.outofboundstime > 0
            && !player.invincible)
        {
            if (++timer > 62) { // 62 = fps
                timer = 0;

                if (--displaytimer < 0) {
                    if (player_kill_nonkill != player.KillPlayerMapHazard(false, kill_style_environment, false))
                        return;
                }
            }
        }
    }
}

void PlayerOutOfArenaTimer::draw(CPlayer& player)
{
    if (!player.isready())
        return;

    if (player.iy < 0) {
        if (player.iy + PH < -1 || (player.iy + PH <= 0 && player.vely <= 1)) {
            rm->spr_abovearrows.draw(player.ix - PWOFFSET, 0, player.getColorID() * 32, 0, 32, 26);

            //This displays the out of arena timer before the player is killed
            if (game_values.outofboundstime > 0 && displaytimer >= 0)
                rm->spr_awardkillsinrow.draw(player.ix - PWOFFSET + 8, 18, displaytimer << 4, player.getColorID() << 4, 16, 16);
        }
    }
}
