#include "PlayerSpinStatus.h"

#include "../player.h"

#include "GameValues.h"
extern CGameValues game_values;

void PlayerSpinStatus::reset() {
    timer = 0;
    state = 0;
}

void PlayerSpinStatus::update(CPlayer& player) {
    if (isSpinInProgress()) {
        if (++timer > 3) {
            timer = 0;
            state++;
            if (state == 6 || state == 12) {
                state = 0;

                if (player.powerup == 3 && game_values.featherlimit > 0)
                    player.DecreaseProjectileLimit();

            }
        }
    }
}

void PlayerSpinStatus::spin(CPlayer& player) {
    state = player.isFacingRight() ? (game_values.reversewalk ? 7 : 1) : (game_values.reversewalk ? 1 : 7);
    timer = 0;
}

bool PlayerSpinStatus::isSpinInProgress() const {
    return state > 0;
}

bool PlayerSpinStatus::isReverseWalking() const {
    if ((state >= 2 && state <= 4) || state == 7 || state == 11)
        return true;

    return false;
}

uint8_t PlayerSpinStatus::toSpriteID() {
    static uint8_t spinStateToSprite[12] = {PGFX_JUMPING_R, PGFX_STOPPING_R, PGFX_STOPPING_L, PGFX_STANDING_L, PGFX_STOPPING_L, PGFX_STOPPING_R, PGFX_JUMPING_L, PGFX_STOPPING_L, PGFX_STOPPING_R, PGFX_STANDING_R, PGFX_STOPPING_R, PGFX_STOPPING_L};

    if (game_values.reversewalk)
        return spinStateToSprite[state] & 0x1 ? spinStateToSprite[state] - 1 : spinStateToSprite[state] + 1;
    else
        return spinStateToSprite[state];
}

uint8_t PlayerSpinStatus::toCapeFrameX() {
    static uint8_t spinStateToCapeSprite[12] = {0, 32, 32, 0, 32, 32, 0, 32, 32, 0, 32, 32};
    return spinStateToCapeSprite[state];
}
