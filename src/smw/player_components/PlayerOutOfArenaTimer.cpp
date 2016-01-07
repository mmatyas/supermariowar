#include "PlayerOutOfArenaTimer.h"

#include "GameValues.h"
#include "player.h"
#include "PlayerKillStyles.h"
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
        if ((player.bottomY() < -1 || (player.bottomY() <= 1 && player.vely <= 0.8f))
            && game_values.outofboundstime > 0
            && !player.isInvincible())
        {
            if (++timer > 62) { // 62 = fps
                timer = 0;

                if (--displaytimer < 0) {
                    player.KillPlayerMapHazard(false, kill_style_environment, false);
                }
            }
        }
    }
}

void PlayerOutOfArenaTimer::draw(CPlayer& player)
{
    if (!player.isready())
        return;

    if (player.topY() < 0) {
        if (player.bottomY() < -1 || (player.bottomY() <= 0 && player.vely <= 1)) {
            rm->spr_abovearrows.draw(player.leftX() - PWOFFSET, 0, player.getColorID() * 32, 0, 32, 26);

            //This displays the out of arena timer before the player is killed
            if (game_values.outofboundstime > 0 && displaytimer >= 0)
                rm->spr_awardkillsinrow.draw(player.leftX() - PWOFFSET + 8, 18, displaytimer << 4, player.getColorID() << 4, 16, 16);
        }
    }
}
