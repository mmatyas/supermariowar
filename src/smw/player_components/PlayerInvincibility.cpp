#include "PlayerInvincibility.h"

#include "gamemodes.h"
#include "GameValues.h"
#include "player.h"
#include "ResourceManager.h"

extern CGameValues game_values;
extern CResourceManager* rm;

void PlayerInvincibility::reset()
{
    invincible = false;
    timer = 0;
}

void PlayerInvincibility::turn_on(CPlayer& player)
{
    invincible = true;
    timer = 0;
    player.animationstate = 0;
    player.animationtimer = 0;
    player.shield.abort();

    //Stop the invincible music if a player is already invincible
    //(we don't want two invincible music sounds playing at the same time)
    ifsoundonstop(rm->sfx_invinciblemusic);

    if (!game_values.gamemode->gameover) {
        game_values.playinvinciblesound = true;

        if (game_values.music && game_values.sound)
            rm->backgroundmusic[0].stop();
    }
}

bool PlayerInvincibility::is_on() const
{
    return invincible;
}

void PlayerInvincibility::update(CPlayer& player)
{
    if (invincible) {
        player.animationtimer++;

        if ((player.animationtimer > 3 && timer < 480) || player.animationtimer > 6) {
            player.animationtimer = 0;

            player.animationstate += 32;
            if (player.animationstate > 96)
                player.animationstate = 0;
        }

        if (++timer > 580) {
            player.animationstate = 0;
            player.animationtimer = 0;
            timer = 0;
            invincible = false;
        }
    }
}
