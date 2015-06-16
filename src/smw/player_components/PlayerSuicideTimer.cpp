#include "PlayerSuicideTimer.h"

#include "GameValues.h"
#include "gamemodes.h"
#include "player.h"
#include "ResourceManager.h"

extern CGameValues game_values;
extern CResourceManager* rm;

void PlayerSuicideTimer::reset()
{
    timer = 0;
    counttimer = 0;
    displaytimer = 2;
}

void PlayerSuicideTimer::update(CPlayer& player)
{
    if (game_values.gamemode->gameover || game_values.singleplayermode >= 0)
        reset();

    if (!player.isInvincible()
        && !player.isFrozen()
        && game_values.suicidetime > 0
        && ++timer > game_values.suicidetime) {
        if (++counttimer > 62) {
            counttimer = 0;

            if (--displaytimer < 0)
                player.KillPlayerMapHazard(true, kill_style_environment, false);
        }
    }
}

void PlayerSuicideTimer::draw(CPlayer& player)
{
    if (timer > game_values.suicidetime)
        rm->spr_awardkillsinrow.draw(
            player.leftX() - PWOFFSET + 8,
            player.topY() - PHOFFSET + 8,
            displaytimer << 4,
            player.getColorID() << 4,
            16, 16);
}
