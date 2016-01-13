#include "PlayerTanookiSuit.h"

#include "eyecandy.h"
#include "GameValues.h"
#include "input.h"
#include "player.h"
#include "ResourceManager.h"

#include <cassert>

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

bool PlayerTanookiSuit::canTurnIntoStatue(CPlayer& player) {
    return player.playerKeys->game_turbo.fPressed
        && player.playerKeys->game_down.fDown
        && !statue_lock
        && player.powerupused == -1
        && !player.lockfire
        && !player.kuriboshoe.is_on()
        && !player.tail.isInUse();
}

void PlayerTanookiSuit::startSuperStomping(CPlayer& player) {
    player.superstomp.startSuperStomping(player);

    // Become soft shielded (with stomp ability)
    player.shield.setType(SOFT_WITH_STOMP);
}

void PlayerTanookiSuit::update(CPlayer& player)
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
        eyecandy[2].add(new EC_SingleAnimation(&rm->spr_poof, player.centerX() - 24, player.centerY() - 24, 4, 5));
        ifSoundOnPlay(rm->sfx_transform);

        // Neutralize lateral velocity
        const float tv = 1.6f;
        if (player.velx > tv)
            player.velx = tv;
        else if (player.velx < -tv)
            player.velx = -tv;

        // Cause statue to super stomp to ground
        if (player.canSuperStomp()) {
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
    if (!player.superstomp.isInSuperStompState()) {
        // Count down the statue timer, which leads to a forced detransformation
        if (statue_timer == 1) {
            // Untransform from statue
            statue_timer = 0;

            // Release invincibility
            player.shield.setType(OFF);

            // Slight upward velocity to escape spikes / lava
            if (!player.inair)
                player.vely = -8.0;

            // perform transformation effects
            eyecandy[2].add(new EC_SingleAnimation(&rm->spr_poof, player.centerX() - 24, player.centerY() - 24, 4, 5));
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
            player.shield.setType(SOFT_WITH_STOMP);

            statue_lock = true;
        }
    }
}

bool PlayerTanookiSuit::isOn() const {
    return tanooki_on;
}

bool PlayerTanookiSuit::isStatue() const {
    return !notStatue();
}

bool PlayerTanookiSuit::notStatue() const {
    return statue_timer == 0;
}

bool PlayerTanookiSuit::isBlinking() const {
    return (statue_timer < 50) && (statue_timer / 3 % 2);
}

void PlayerTanookiSuit::onPickup() {
    ifSoundOnPlay(rm->sfx_collectpowerup);
    tanooki_on = true;

    if (game_values.tanookilimit > 0)
        statue_uses_left = game_values.tanookilimit;
}

void PlayerTanookiSuit::allowStatue() {
    statue_lock = false;
}

void PlayerTanookiSuit::drawStatue(CPlayer& player)
{
    assert(isStatue());

    //Blink the statue if the time is almost up
    if (player.isready() && isBlinking())
        return;

    //Draw the statue
    if (player.iswarping())
        rm->spr_statue.draw(
            player.leftX() - PWOFFSET,
            player.topY() - 31,
            player.getColorID() << 5,
            0, 32, 58,
            (short)player.state % 4, player.GetWarpPlane());
    else
        rm->spr_statue.draw(
            player.leftX() - PWOFFSET,
            player.topY() - 31,
            player.getColorID() << 5,
            0, 32, 58);
}
