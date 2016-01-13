#ifndef PLAYER_TAIL
#define PLAYER_TAIL

#include <stdint.h>

class CPlayer;

enum PlayerTailState {
    TAIL_NOT_IN_USE = 0,
    TAIL_SHAKE = 1,
    TAIL_SPIN_AND_SHAKE = 2
};

class PlayerTail
{
public:
    void reset();
    void shake(CPlayer& player);
    void spin(CPlayer& player);

    //If player is shaking tail, slow decent
    void slowDescent(CPlayer& player);

    bool isInUse() const;
    void draw(CPlayer& player);

private:
    int8_t iTailTimer;
    uint16_t iTailFrame;
    PlayerTailState iTailState;
};

#endif // PLAYER_TAIL
