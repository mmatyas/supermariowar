#pragma once

#include <stdint.h>

class CPlayer;

class PlayerSpinStatus {
public:
    void reset();
    void update(CPlayer& player);
    void spin(CPlayer& player);

    bool isReverseWalking() const;
    bool isSpinInProgress() const;

    uint8_t toSpriteID();
    uint8_t toCapeFrameX();

private:
    int8_t timer = 0;
    uint8_t state = 0;
};
