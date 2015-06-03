#include "PlayerTanookiSuit.h"

#include "eyecandy.h"
#include "GameValues.h"
#include "input.h"
#include "player.h"
#include "ResourceManager.h"

extern CGameValues game_values;
extern CResourceManager* rm;

extern CEyecandyContainer eyecandy[3];

PlayerTanookiSuit::PlayerTanookiSuit() {
    reset();
}

void PlayerTanookiSuit::reset() {
    tanooki_on = false;
    statue_lock = false;
    statue_timer = 0;
    statue_uses_left = 0;
}

bool PlayerTanookiSuit::canTurnIntoStatue(CPlayer &player) {
    return player.playerKeys->game_turbo.fPressed
        && player.playerKeys->game_down.fDown
        && !statue_lock
        && player.powerupused == -1
        && !player.lockfire
        && !player.kuriboshoe.is_on()
        && !player.tail.isInUse();
}

bool PlayerTanookiSuit::canSuperStomp(CPlayer &player) {
    return player.inair
        && !player.fSuperStomp
        && player.iSuperStompTimer <= 0
        && !player.superstomp_lock;
}

void PlayerTanookiSuit::startSuperStomping(CPlayer &player) {
    player.iSuperStompTimer = 8;
    player.lockfall = true;

    // Become soft shielded (with stomp ability)
    player.shield = 2;

    // Stop player from super stomping twice before touching the ground
    player.superstomp_lock = true;
}

void PlayerTanookiSuit::update(CPlayer &player)
{
    if (!tanooki_on || !player.isready())
        return;

    // If the down key is ever released, manually untransform by releasing the down key
    if (!player.playerKeys->game_down.fDown && statue_timer) {
        // Untransform from statue
        statue_timer = 1;
    }

    // Become the tanooki
    else if (canTurnIntoStatue(player)) {
        // set the amount of time you get to remain in statue form
        statue_timer = 123;

        // perform tansformation effects
        eyecandy[2].add(new EC_SingleAnimation(&rm->spr_poof, player.ix + HALFPW - 24, player.iy + HALFPH - 24, 4, 5));
        ifSoundOnPlay(rm->sfx_transform);

        // Neutralize lateral velocity
        const float tv = 1.6f;
        if (player.velx > tv)
            player.velx = tv;
        else if (player.velx < -tv)
            player.velx = -tv;

        // Cause statue to super stomp to ground
        if (canSuperStomp(player)) {
            startSuperStomping(player);
        }

        // Prevent you from shooting
        player.lockfire = true;

        // Prevent you from falling through solid-on-top blocks
        player.lockfall = true;

        // Prevent you from becoming the statue twice before touching the ground
        statue_lock = true;

        //If we were flying or spinning when we became the statue, clear those states
        player.ClearPowerupStates();
    }

    // If not super stomping currently
    if (!player.fSuperStomp && player.iSuperStompTimer <= 0) {
        // Count down the statue timer, which leads to a forced detransformation
        if (statue_timer == 1) {
            // Untransform from statue
            statue_timer = 0;

            // Release invincibility
            player.shield = 0;

            // Slight upward velocity to escape spikes / lava
            if (!player.inair)
                player.vely = -8.0;

            // perform transformation effects
            eyecandy[2].add(new EC_SingleAnimation(&rm->spr_poof, player.ix + HALFPW - 24, player.iy + HALFPH - 24, 4, 5));
            ifSoundOnPlay(rm->sfx_transform);

            //Decrease the amount of tanooki uses, if feature is turned on
            if (game_values.tanookilimit > 0 || statue_uses_left > 0) {
                if (--statue_uses_left == 0) {
                    tanooki_on = false;
                    statue_uses_left = 0;
                    ifSoundOnPlay(rm->sfx_powerdown);
                }
            }
        }

        // Player is a statue
        else if (statue_timer > 0) {
            // Prevent player from shooting while a statue
            player.lockfire = true;

            // Prevent player from jumping
            player.lockjump = true;

            // Don't fall through passable platforms
            player.fallthrough = false;

            // Decrement statue timer3
            statue_timer--;

            // Become soft shielded (with stomp ability)
            player.shield = 2;

            statue_lock = true;
        }
    }
}

bool PlayerTanookiSuit::isOn() {
    return tanooki_on;
}

bool PlayerTanookiSuit::isStatue() {
    return !notStatue();
}

bool PlayerTanookiSuit::notStatue() {
    return statue_timer == 0;
}

void PlayerTanookiSuit::onPickup() {
    ifSoundOnPlay(rm->sfx_collectpowerup);
    tanooki_on = true;

    if (game_values.tanookilimit > 0)
        statue_uses_left = game_values.tanookilimit;
}

bool PlayerTanookiSuit::isBlinking() {
    return (statue_timer < 50) && (statue_timer / 3 % 2);
}

void PlayerTanookiSuit::allowStatue() {
    statue_lock = false;
}
