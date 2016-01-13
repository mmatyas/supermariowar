#include "PlayerSuperStomp.h"

#include "GameValues.h"
#include "eyecandy.h"
#include "ObjectContainer.h"
#include "objectgame.h"
#include "player.h"
#include "ResourceManager.h"

#include <cassert>

extern CGameValues game_values;
extern CResourceManager* rm;

extern CEyecandyContainer eyecandy[3];
extern CObjectContainer objectcontainer[3];

static const unsigned short SSTOMP_STRENGTH_TIMER = 40;

PlayerSuperStomp::PlayerSuperStomp() {
    reset();
}

void PlayerSuperStomp::reset() {
    is_stomping = false;
    floating_timer = 0;
    iSuperStompExitTimer = 0;
    superstomp_lock = false;
}

void PlayerSuperStomp::update_onGroundHit(CPlayer &player)
{
    if (player.inair)
        return;

    //If the player is touching the ground, then free the lock on super stomping
    superstomp_lock = false;

    //If they are touching the ground and they are not the statue, allow them to become the statue again
    if (player.tanookisuit.notStatue())
        player.tanookisuit.allowStatue();

    //If they were super stomping and they are not in the air anymore (i.e. on the ground), then create the
    //super stomp attack zone, play the sound and show the stomp gfx
    if (is_stomping) {
        eyecandy[2].add(new EC_SuperStompExplosion(&rm->spr_superstomp, player.centerX(), player.centerY(), 4));
        ifSoundOnPlay(rm->sfx_bobombsound);
        is_stomping = false;

        objectcontainer[1].add(new MO_AttackZone(player.getGlobalID(), player.getTeamID(), player.leftX() - 32, player.topY() + 10, 32, 15, 8, kill_style_kuriboshoe, false));
        objectcontainer[1].add(new MO_AttackZone(player.getGlobalID(), player.getTeamID(), player.rightX(), player.topY() + 10, 32, 15, 8, kill_style_kuriboshoe, false));
    }
}

void PlayerSuperStomp::startSuperStomping(CPlayer &player) {
    // this function should be called to start the super stomping sequence
    assert(superstomp_lock == false);
    assert(is_stomping == false);
    assert(floating_timer <= 0);

    floating_timer = 8;
    superstomp_lock = true; // Stop player from super stomping twice before touching the ground
    player.lockfall = true;
}

void PlayerSuperStomp::update(CPlayer &player) {
    //Hold super stomping player in mid air then accelerate them downwards
    if (floating_timer > 0) {
        if (--floating_timer <= 0) {
            is_stomping = true;
            iSuperStompExitTimer = SSTOMP_STRENGTH_TIMER;
            player.vely = VELSUPERSTOMP;

            player.lockfall = true;
        } else {
            player.velx = 0.0f;
            player.vely = 0.0f;
        }
    }

    if (is_stomping && --iSuperStompExitTimer <= 0)
        is_stomping = false;
}

// Player is in the stomping phase
bool PlayerSuperStomp::isStomping() const {
    return is_stomping;
}

// Player is in any of the SuperStomping phases (floating/stomping/falling)
bool PlayerSuperStomp::isInSuperStompState() const {
    return superstomp_lock;
}
