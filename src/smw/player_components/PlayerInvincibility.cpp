#include "PlayerInvincibility.h"

#include "GameMode.h"
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
    player.shield.abort();

    //Stop the invincible music if a player is already invincible
    //(we don't want two invincible music sounds playing at the same time)
    ifsoundonstop(rm->sfx_invinciblemusic);

    if (!game_values.gamemode->gameover) {
        game_values.flags.playinvinciblesound = true;

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
        if (++timer > 580) {
            timer = 0;
            invincible = false;
        }
    }
}

PlayerPalette PlayerInvincibility::getPlayerPalette() const
{
    PlayerPalette invincibility_animation_states[4] = {normal, invincibility_1, invincibility_2, invincibility_3};
    if (timer < 480) {
        return invincibility_animation_states[(timer / 4) % 4];
    } else {
        return invincibility_animation_states[((timer - 4) / 7) % 4];
    }
}
