#include "PlayerShield.h"

#include "gamemodes.h"
#include "GameValues.h"

extern CGameValues game_values;

void PlayerShield::reset()
{
    abort();

    if (game_values.gamemode->getgamemode() == game_mode_survival) {
        if (game_values.gamemodesettings.survival.shield) {
            timer = game_values.shieldtime;
            type = (PlayerShieldType)game_values.shieldstyle;
        }
    } else if (game_values.shieldstyle > 0) {
        timer = game_values.shieldtime;
        type = (PlayerShieldType)game_values.shieldstyle;
    }
}

void PlayerShield::turn_on()
{
    type = game_values.shieldstyle > 0 ? (PlayerShieldType)game_values.shieldstyle : SOFT;
    timer = 60;
}

void PlayerShield::abort()
{
    type = OFF;
    timer = 0;
}

bool PlayerShield::is_on() const {
    return type > OFF;
}

PlayerShieldType PlayerShield::getType() const {
    return type;
}

void PlayerShield::setType(PlayerShieldType type) {
    this->type = type;
}

void PlayerShield::update()
{
    //If player is shielded, count down that timer
    if (timer > 0 && --timer == 0)
        type = OFF;
}

unsigned short PlayerShield::time_left() const
{
    return timer;
}
