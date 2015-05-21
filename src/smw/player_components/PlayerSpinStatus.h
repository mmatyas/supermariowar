#ifndef PLAYER_SPIN_STATUS
#define PLAYER_SPIN_STATUS

#include <stdint.h>

class CPlayer;

struct PlayerSpinStatus
{
    int8_t timer;
    uint8_t state;

    void reset();
    void update(CPlayer& player);
    void spin(CPlayer& player);

    bool isReverseWalking();
    bool isSpinInProgress();

    uint8_t toSpriteID();
    uint8_t toCapeFrameX();
};

#endif // PLAYER_SPIN_STATUS
