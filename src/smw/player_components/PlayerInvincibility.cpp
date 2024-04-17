#include "PlayerInvincibility.h"

#include <array>
#include "GameMode.h"
#include "GameValues.h"
#include "player.h"
#include "ResourceManager.h"

extern CGameValues game_values;
extern CResourceManager* rm;

static const std::array<PlayerPalette, 4> INVINCIBILITY_ANIMATION_STATES = {normal, invincibility_1, invincibility_2, invincibility_3};
static const short INVINCIBILITY_TIME = 580;
static const short INVINCIBILITY_HURRYUP_TIME = 100;
static const short INVINCIBILITY_ANIMATION_DELAY = 4;
static const short INVINCIBILITY_HURRYUP_ANIMATION_DELAY = 7;

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
        if (++timer > INVINCIBILITY_TIME) {
            timer = 0;
            invincible = false;
        }
    }
}

PlayerPalette PlayerInvincibility::getPlayerPalette() const
{
    if (timer < INVINCIBILITY_TIME - INVINCIBILITY_HURRYUP_TIME) {
        return INVINCIBILITY_ANIMATION_STATES[(timer / INVINCIBILITY_ANIMATION_DELAY) % INVINCIBILITY_ANIMATION_STATES.size()];
    } else {
        return INVINCIBILITY_ANIMATION_STATES[((timer - (INVINCIBILITY_TIME - INVINCIBILITY_HURRYUP_TIME)) / INVINCIBILITY_HURRYUP_ANIMATION_DELAY) % INVINCIBILITY_ANIMATION_STATES.size()];
    }
}
