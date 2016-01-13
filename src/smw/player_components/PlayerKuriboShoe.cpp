#include "PlayerKuriboShoe.h"

#include "../player.h"

#include "eyecandy.h"
#include "GameValues.h"
#include "ObjectContainer.h"
#include "objectgame.h"
#include "ResourceManager.h"

extern CGameValues game_values;
extern CResourceManager* rm;

extern CEyecandyContainer eyecandy[3];
extern CObjectContainer objectcontainer[3];

static const uint8_t kuriboShoe_exitCode[4] = {4, 8, 4, 8};

void PlayerKuriboShoe::reset()
{
    type = NONE;
    animationTimer = 0;
    animationFrame = 0;
    exitTimer = 0;
    exitIndex = 0;
}

// Return true if the player is inside a shoe
bool PlayerKuriboShoe::is_on() const
{
    return type > 0;
}

KuriboShoeType PlayerKuriboShoe::getType() const
{
    return type;
}

void PlayerKuriboShoe::setType(KuriboShoeType newtype)
{
    type = newtype;
}

void PlayerKuriboShoe::update(CPlayer& player, uint8_t keymask)
{
    update_gettingOutOfTheShoe(player, keymask); //Free player from the kuribo shoe
    update_SuperStomp(player);
    update_animation();
}

// Free player from the kuribo shoe
void PlayerKuriboShoe::update_gettingOutOfTheShoe(CPlayer& player, uint8_t keymask)
{
    if (is_on() && player.isready() && !player.frozen) {
        if (exitIndex > 0) {
            if (++exitTimer >= 32) {
                exitIndex = 0;
                exitTimer = 0;
            }
        }

        if (keymask & kuriboShoe_exitCode[exitIndex]) {
            exitIndex++;
        } else if (keymask & ~kuriboShoe_exitCode[exitIndex]) {
            exitIndex = 0;
            exitTimer = 0;
        }

        if (exitIndex == 4 && exitTimer < 32) {
            CO_KuriboShoe * shoe = new CO_KuriboShoe(&rm->spr_kuriboshoe, player.ix - PWOFFSET, player.iy - PHOFFSET - 2, type == STICKY);
            shoe->collision_detection_checksides();
            objectcontainer[1].add(shoe);
            eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, player.centerX() - 16, player.centerY() - 16, 3, 8));

            exitIndex = 0;
            exitTimer = 0;
            ifSoundOnPlay(rm->sfx_transform);
            type = NONE;

            player.superstomp.reset();
        }
    }
}

void PlayerKuriboShoe::update_SuperStomp(CPlayer& player) {
    if (is_on() && player.canSuperStomp() && player.wantsToSuperStomp()) {
        if (player.highJumped()) {
            player.superstomp.startSuperStomping(player);
        }
    }
}

void PlayerKuriboShoe::update_animation()
{
    if (type > 0) {
        if (++animationTimer > 7) {
            animationTimer = 0;
            animationFrame += 32;

            if (animationFrame > 32)
                animationFrame = 0;
        }
    }
}

void PlayerKuriboShoe::draw(CPlayer& player)
{
    if (type > 0) {
        if (player.iswarping())
            rm->spr_kuriboshoe.draw(
                player.leftX() - PWOFFSET, player.topY() - PHOFFSET,
                animationFrame + (type == STICKY ? 64 : 0), (player.sprite_state & 0x1) == 0 ? 0 : 32,
                32, 32,
                (short)player.state % 4, player.GetWarpPlane());
        else
            rm->spr_kuriboshoe.draw(
                player.leftX() - PWOFFSET, player.topY() - PHOFFSET,
                animationFrame + (type == STICKY ? 64 : 0), (player.sprite_state & 0x1) == 0 ? 0 : 32,
                32, 32);
    }
}
