#include "PlayerSecretCode.h"

//#include "objectgame.h" // for CheckSecret(), FIXME
#include "player.h"

extern void CheckSecret(short id);

static const uint8_t SECRET_CODE[18] = {4, 8, 4, 8, 1, 1, 4, 8, 4, 8, 1, 1, 4, 8, 4, 8, 1, 1};
static const unsigned short SECRET_CODE_TIME_MAX = 186;

PlayerSecretCode::PlayerSecretCode() {
    reset();
}

void PlayerSecretCode::reset() {
    timer = 0;
    index = 0;
}

void PlayerSecretCode::update(CPlayer& player, const uint8_t keymask) {
    if (!player.isready())
        return;

    if (index > 0) {
        if (++timer >= SECRET_CODE_TIME_MAX) {
            index = 0;
            timer = 0;
        }
    }

    if (keymask & SECRET_CODE[index]) {
        index++;
    } else if (keymask & ~SECRET_CODE[index]) {
        index = 0;
        timer = 0;
    }

    if (index == 18 && timer < SECRET_CODE_TIME_MAX) {
        index = 0;
        timer = 0;
        CheckSecret(3);
    }
}
