#pragma once

#include "objects/powerup/Powerup.h"

class CPlayer;
class gfxSprite;


enum class CoinColor : unsigned char {
    Red,
    Green,
    Yellow,
    Blue,
};


class PU_CoinPowerup : public MO_Powerup {
public:
    PU_CoinPowerup(gfxSprite* nspr, Vec2s pos, CoinColor color, short value);

    void update() override;
    void draw() override;
    bool collide(CPlayer* player) override;

protected:
    const short iColorOffsetY;
    const short iValue;

    short sparkleanimationtimer = 0;
    short sparkledrawframe = 0;
};
